// for pc test,
//see <devicename>.c for device specified source code

// k210...k210.c
// esp32...esp32.c

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "3dconfig.hpp"
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

#ifndef USE_K210
#ifdef __cplusplus
extern "C"{
#endif
void* vTask(void* prm);
  int main3d(const char *model,const char *motion);
#ifdef __cplusplus
};
#endif

//for pc implementation:)
int filopen(const char *pathname,fil *f){
  *f = fopen(pathname,"r");
  return *f==0; //not 0...fail, 0...success
}

int filread(fil *fp,void *buff,size_t byte){
  return fread(buff,1,byte,(FILE*)*fp);
}

#ifdef OUTPUTTERMINAL
void send_line(int ypos,uint8_t *data){
  int r,g,b;
  size_t id = 0;
  uint8_t pv[2]={1,100};
  char tmp[64];
  char *buff;
  buff = (char*)calloc(1024*1024,1);
  for(int dy=0;dy<DRAW_NLINES;dy++){
    sprintf(tmp,"\033[%d;1H",ypos+dy);strcat(buff,tmp);
    for(int i=0;i<window_width;i++){
      if(pv[0] == data[id+0]&&pv[1]==data[id+1]){
	sprintf(tmp,"  ");strcat(buff,tmp);
	id+=2;
      }else{
	r = data[id+0]>>3;
	g = ((data[id+0]&0x7)<<3)|(data[id+1]>>5);
	b = data[id+1]&0x1F;
	pv[0] = data[id+0];
	pv[1] = data[id+1];
	id+=2;
	sprintf(tmp,"\033[48;2;%d;%d;%dm  ",r*8,g*4,b*8);strcat(buff,tmp);
      }
    }
  }
  printf("%s",buff);
  fflush(stdout);
  free(buff);  
}
#endif


#include <sys/time.h>
extern "C"{
  uint64_t get_time(void);

  uint64_t get_time(void){
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
  }
};

int psnum[PROCESSNUM];
int main(int argc,const char **argv){
#ifdef PTHREAD
  pthread_t pthread;
  static int i;
  for(i=0;i<PROCESSNUM-1;i++){
    psnum[i]=i;
    pthread_create( &pthread, NULL, &vTask, &psnum[i]);
  }
#endif
  if(argc!=3)printf("arg err\n$ %s <.pmd (model file)> <.vmd (motion file)>\n",argv[0]);
  main3d(argv[1],argv[2]);
}
#endif
