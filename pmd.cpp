#include "pmd.hpp"
#include <stdio.h> //for poor C++ implementation at k210:(
#include "3dconfig.hpp"
#include <string.h>
#include "texturepoly.hpp"
#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif
#include <unistd.h>
#include "fileio.h"
#define ZNEAR 0.1f
#include "images.hpp"
#include "skeletal-animation.hpp"


static
bonelist bl;

imgs::images il(16);
pmd::pmd(const char *pathname){
  fil f;
  if(filopen(pathname,&f)!=0){
    printf("%s cannot open\n",pathname);
  }

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
    for(uint32_t i=0;i<vertexcount;i+=READNUM){
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
  materiallist=(imgs::image*)malloc(sizeof(imgs::image)*materialcount);
  materialfacecountlist=(uint16_t*)malloc(sizeof(uint16_t)*materialcount);
  {
    for(uint32_t i=0;i<materialcount;i++){
      pmdmaterial m;
      filread(f,&m,sizeof(pmdmaterial));
      materiallist[i]=il.get_or_add(m.texture_filename);
      materialfacecountlist[i]=m.facevert_count/3;
    }
  }

  filread(f,&bonecount,2);
  bonelist = (bone_t*)malloc(sizeof(bone_t)*bonecount);
  bonenamelist = (char**)malloc(sizeof(char*)*bonecount);
  ikknee[0] = 0;
  {// load bone and animation
    //bone name list is not needed after loading materials.
    pmdbone b;
    for(int i=0;i<bonecount;i++){
      filread(f,&b,sizeof(pmdbone));
      b.name[19]=0;//fix me, support up to 20 charactors.(19 right now.)
      bonenamelist[i] = (char*)malloc(sizeof(char)*(strlen(b.name)+1));
      strcpy(bonenamelist[i],b.name);
      bonelist[i].parent = b.parent;
      bonelist[i].pos.x = b.headpos[0];
      bonelist[i].pos.y = b.headpos[1];
      bonelist[i].pos.z = b.headpos[2];
      if(bonenamelist[i][0]==0x82|| //ひ
	 bonenamelist[i][0]==0x50||
	 bonenamelist[i][1]==0x82|| //ざ
	 bonenamelist[i][0]==0x22){//"ひざ" means knee.
	if(ikknee[0] == 0){
	  ikknee[0] = i;
	}else{
	  ikknee[1] = i;
	}
      }
    }
  }
  
  v.load("/home/gombe/workspace/program/pc/vmdtest/wavefile_v2.vmd",this);

  filread(f,&ikcount,sizeof(uint16_t));
  printf("ik=%d\n",ikcount);
  iklist = (ik_t**)malloc(sizeof(ik_t*)*ikcount);
  {
    pmdik ik;
    for(int i=0;i<ikcount;i++){
      filread(f,&ik,sizeof(pmdik));
      iklist[i] = (ik_t*)malloc(sizeof(ik_t)+(ik.len-1/*default length*/)*sizeof(uint16_t));
      iklist[i]->rootid = ik.root;
      iklist[i]->targetid = ik.target;
      iklist[i]->len = ik.len;
      iklist[i]->looptimes = ik.iterations;
      iklist[i]->anglelimit = ik.ctrlweight;
      filread(f,iklist[i]->data,sizeof(uint16_t)*ik.len);
    }
  }
}

void pmd::calcvertexes(){
  fvector4 f;
  for(unsigned int j=0;j<vertexcount;j++){
#ifdef DISABLE_ANIMATION
    f =  m.mul_fv4(fvector3(vertexlist[j]));
#else
    f =  bl.boneworld[matrixnolist[j]&0xFF].mul_fv4(fvector3(vertexlist[j]));
#endif
    tvertexlist[j].x=f.x*window_width+window_width/2;
    tvertexlist[j].y=f.y*window_height+window_height/2;
    tvertexlist[j].z = f.z;
    tvertexlist[j].w = f.w;
  }
}

void pmd::draw(uint16_t *drawbuff,uint16_t *zbuff){
  texturetriangle t;
  fvector4 v[3];
  fvector2 uv[3];
  int zovercount = 0;
  int face[3];
  unsigned mati=0;
  int nextmati=0;

  nextmati = materialfacecountlist[0];

  for(uint32_t i=0;i<facecount;i++){
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
    while(i >= nextmati){
      mati ++;
      nextmati += materialfacecountlist[mati];
    }

    t.triangle_set(v,1,&(materiallist[mati]),uv);
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
  //todo, destruct all elements.
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

#ifndef DISABLE_ANIMATION
  bl.init(p.bonelist,p.bonecount,p.v.motionlist,&p);
#endif
  
  while(1){
#ifdef PC
    usleep(1000);
#endif
#ifdef VISUALDEBUG
    vdb_begin();
    vdb_frame();
#endif

    bl.calcall(p.m);
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
