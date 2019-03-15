#include "pmd.hpp"
#include <stdio.h> //for poor C++ implementation at k210:(
#include "3dconfig.hpp"
#include <string.h>
#include "texturepoly.hpp"
#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif
#include <unistd.h>

#define ZNEAR 0.1f
//for pc implementation:)
typedef void* fil;
static
fil filopen(const char *pathname){
  return (fil)fopen(pathname,"r");
}

static 
int filread(fil fp,void *buff,size_t byte){
  return fread(buff,1,byte,(FILE*)fp);
}

void fail(){
  //todo
  while(1);
}

pmd::pmd(const char *pathname){
  fil f;
  f = filopen(pathname);

  {//read headder
    pmdheader d;
    filread(f,&d,sizeof(d));
    d.name[19] = '\0';//handle force 19 charactor limit :) fix me.
    name = (char*)malloc(strlen(d.name)+1);
    if(name==NULL)fail();
    strcpy(name,d.name);
  }

  //read vertexes
  filread(f,&vertexcount,4);
  vertexlist = (fvector3_t*)malloc(sizeof(fvector3_t)*vertexcount);
  if(vertexlist==NULL)fail();
  tvertexlist = (fvector4*)malloc(sizeof(fvector4)*vertexcount);
  if(vertexlist==NULL)fail();
  uvlist = (fvector2_t*)malloc(sizeof(fvector2_t)*vertexcount);
  if(uvlist==NULL)fail();
  matrixnolist = (uint32_t*)malloc(sizeof(uint32_t)*vertexcount);
  if(matrixnolist==NULL)fail();
  
  {
#define READNUM 32
    pmdvertex b[READNUM];
    int ln;
    for(int i=0;i<vertexcount;i+=READNUM){
      if(vertexcount-i<READNUM){
	ln = vertexcount-i;
      }else{
	ln = READNUM;
      }
      filread(f,&b,ln*sizeof(pmdvertex));
      for(int j=0;j<ln;j++){
	int idx;
	idx = i+j;
	vertexlist[idx].x = b[j].pos[0];
	vertexlist[idx].y = b[j].pos[1];
	vertexlist[idx].z = b[j].pos[2];
	uvlist[idx].x = b[j].uv[0];
	uvlist[idx].y = b[j].uv[1];
	if(b[j].boneweight<=5){
	  matrixnolist[idx]=b[j].bonenum[1];
	}else if(b[j].boneweight>=95){
	  matrixnolist[idx]=b[j].bonenum[0];
	  //	  printf("%d,//%d\n",boneidx[i.bone_num[0]],i.bone_num[0]);
	}else{
	  matrixnolist[idx] = b[j].bonenum[1]|(b[j].bonenum[0]<<8)|(b[j].boneweight<<16);
	}
      }
    }
  }
  
  //load faces
  filread(f,&facecount,4);
  facecount/=3;//triangle only
  facelist = (uint16_t*)malloc(3*sizeof(uint16_t)*facecount);
  filread(f,facelist,3*sizeof(uint16_t)*facecount);

  //load material
  filread(f,&materialcount,4);
  {
    char texture_buff[16][20];//texture name buffer;
    for(int i=0;i<materialcount;i++){
      pmdmaterial m;
      filread(f,&m,sizeof(pmdmaterial));
    }
  }    
}

void pmd::calcvertexes(){
  fvector4 f;
  for(unsigned int j=0;j<vertexcount;j++){
#ifdef DISABLE_ANIMATION
    f =  m.mul_fv4(fvector3(vertexlist[j]));
#else
#error has not implemented yet, ;)
    //    f =  bl.boneworld[bone_index[j]].mul_fv4(fvector3(pointvec[j]));
#endif
    tvertexlist[j].x=f.x*window_width+window_width/2;
    tvertexlist[j].y=f.y*window_height+window_height/2;
    tvertexlist[j].z = f.z;
    tvertexlist[j].w = f.w;
  }
}

const uint16_t tex_body[65536] =
  #include "texture-body"
  ;

void pmd::draw(uint16_t *drawbuff,uint16_t *zbuff){
  texturetriangle t;
  fvector4 v[3];
  fvector2 uv[3];
  int zovercount = 0;
  int face[3];

  for(int i=0;i<facecount;i++){
    for(int j=0;j<3;j++){
      face[j]=facelist[i*3+j];
    }
    for(int j=0;j<3;j++){
      v[j] = tvertexlist[face[j]];
      if(v[j].w < 0){
	zovercount++;
      }
    }
    //    if(zovercount==0&&unvisible(v))continue;
    for(int j=0;j<3;j++){
      #define SIZE_TEX 256
      uv[j].x = (1.f-uvlist[face[j]].x)*SIZE_TEX;
      uv[j].y = (uvlist[face[j]].y)*SIZE_TEX;
    }
    texture_t tex={
      tex_body
    };

    t.triangle_set(v,1,&tex,uv);
    t.draw(zbuff,drawbuff,0);
  }
}

#ifdef __cplusplus
extern "C"{
#endif
void* vTask(void* prm);
int main3d(void);
#ifdef __cplusplus
};
#endif

pmd::~pmd(){
  //todo, deconstruct all elements.
}

int main3d(void){
  Matrix4 projection;
  Matrix4 obj;

  uint16_t g_drawbuff[2][window_width*DRAW_NLINES];
  uint16_t g_zbuff[window_width*DRAW_NLINES];
  //  printf("%lu\n",sizeof(float));
  pmd p("/home/gombe/workspace/program/pc/mmdtest/初音ミク@七葉1052式.pmd");
#ifdef USE_SDL
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* sdlWindow;
  SDL_Event	ev;
  SDL_Texture *sdlTexture;
  SDL_Renderer *sdlRenderer;

  SDL_CreateWindowAndRenderer(window_width, window_height, SDL_WINDOW_OPENGL, &sdlWindow, &sdlRenderer);
  sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, window_width, DRAW_NLINES);
#endif

  //透視投影行列
  projection=loadPerspective(0.25f,float(window_height)/window_width,ZNEAR,40.f,0,0)*projection;

  fvector3 viewdir,veye;
  float dist = 3.f;
  fvector2 np = fvector2(150.f,0);
  // float fps = 0.f;

  fvector3 n;

  float disttarget = 25.f;
  fvector3 transtarget = fvector3(0,-12,-1.2);
  fvector3 trans = fvector3(0,-12,-1.2);
  char fpsstr[60];

  int lastbuff=0;
  veye = fvector3(0,0,-15.5f);
  while(1){
#ifdef PC
    usleep(1000);
#endif
#ifdef VISUALDEBUG
    vdb_begin();
    vdb_frame();
#endif

#ifdef USE_SDL
    /* usleep(20000); */
    if(SDL_PollEvent(&ev)){
      if(ev.type == SDL_QUIT)	{
	break;
      }
    }
#endif

    //視点計算
    dist = dist*0.7+disttarget*0.3;// + 1.4f*cosf(np.x/150.f*3.14159265358979324f);
    trans = trans*0.7+transtarget*0.3;

    veye = -fvector3(cosf(np.x/300.f*3.14159265f)*cosf(np.y/300.f*3.14159265f),sinf(np.y/300.f*3.14159265f),sinf(np.x/300.f*3.14159265f)*cosf(np.y/300.f*3.14159265f));
    //透視投影行列とカメラ行列の合成
    Matrix4 m=projection*lookat(fvector3(0,0,0),veye*dist)*obj*translation(trans);
#ifndef DISABLE_ANIMATION
    bl.calcall(m);
#endif
    
    for(int i=0;i<window_width*DRAW_NLINES;i++){
      g_zbuff[i]=0xFFFF;
      g_drawbuff[lastbuff][i]=0x8410;/*RGB*/
    }

    //    initialize_g_draw_buffer();

    //描画ステージ
    p.m=m;
    p.calcvertexes();
    p.draw(g_drawbuff[lastbuff],g_zbuff);
#ifndef DISABLE_OUTPUT
#ifdef USE_SDL
    SDL_UpdateTexture(sdlTexture, NULL, (uint8_t*)g_drawbuff[lastbuff], window_width*2);
    {
      SDL_Rect dstrect;
      dstrect.x = 0;
      dstrect.y = 0*DRAW_NLINES;
      dstrect.w = window_width;
      dstrect.h = DRAW_NLINES;
      SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &dstrect);
    }
#elif !defined(PC) 
#ifdef USE_K210
    {
      static int ptime;
      float fps;
      fps = 1000000.f/(get_time()-ptime);
      sprintf(fpsstr,"%5.1ffps",fps);
      // lcd_boxfill(0xFFFF,0,240-12,64,12);
      lcd_ram_draw_string_width( fpsstr, (uint32_t*)g_drawbuff[lastbuff], 0xF800,0xFFFF,window_width/2);
      ptime = get_time();
    }
#endif
    send_line(draw_y*DRAW_NLINES,(uint8_t*)(g_drawbuff[lastbuff]));
#endif
#endif//disable output flag
    lastbuff = 1-lastbuff;
#ifdef USE_SDL
    SDL_RenderPresent(sdlRenderer);
#endif
  }

#ifdef USE_SDL
  SDL_Quit();
#endif
  return 0;
}