/**
Copyright (c) 2018 Gombe.

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/
#include "pmd.hpp"
#include "vmd.hpp"
#include "fileio.h"
#include "mmdstructure.h"

void vmd::load(const char *path,pmd* model){
  fil f;
  
  if(filopen(path,&f)!=0){
    printf("file cannot open(%s)\n",path);
    fail();
  }

  {
    vmdheader h;
    filread(&f,&h,sizeof(vmdheader));
  }

  {// load and convert bone name to bone index
#define N 16
    vmdmotion m[N];
    int idx=0;
    filread(&f,&motioncount,sizeof(uint32_t));
    motionlist = (motion_t*)malloc(sizeof(motion_t)*motioncount);
    if(motionlist == NULL){
      fail();
    }
    for(unsigned int i=0;i<motioncount;i+=N){
      int l = motioncount-i;
      if(l>N)l=N;
      filread(&f,m,sizeof(vmdmotion)*l);
      for(int j=0;j<l;j++){
	m[j].bonename[14]='\0';
	int k;
	for(k=0;k<model->bonecount;k++){
	  if(strcmp(m[j].bonename,model->bonenamelist[k])==0)break;
	}
	if(k==model->bonecount){
	  // not found same name => bone not found
	  // skip remaining tasks.
	  continue;
	}
	motionlist[idx].frame = m[j].frame;
	motionlist[idx].pos.x = m[j].location[0];
	motionlist[idx].pos.y = m[j].location[1];
	motionlist[idx].pos.z = m[j].location[2];
	motionlist[idx].rotation.qx = m[j].rotation[0];
	motionlist[idx].rotation.qy = m[j].rotation[1];
	motionlist[idx].rotation.qz = m[j].rotation[2];
	motionlist[idx].rotation.qw = m[j].rotation[3];
	motionlist[idx].boneid = k;
	idx++;
      }
    }
    //update motioncount, removed some unused motion.
    motioncount = idx;
    printf("count:%d\n",motioncount);
    motionlist = (motion_t*)realloc(motionlist,sizeof(motion_t)*motioncount);
    if(motionlist==NULL)fail();
  }
#undef N
#define TIME_MAX 5000
  {// sort motionlist into need order
    int *endframe;
    int *motionid;
    int donemotioncount=0;
    static int pdc;
    int time=0;
    endframe = (int*)calloc(model->bonecount,sizeof(int));
    motionid = (int*)calloc(model->bonecount,sizeof(int));
    if(endframe==NULL)fail();
    for(int i=0;i<model->bonecount;i++){
      endframe[i] = -1;
    }
    for(int f=0;;f++){
      for(int i=0;i<model->bonecount;i++){
	motionid[i] = -1;
      }
      for(unsigned int j=donemotioncount;j<motioncount;j++){
	if((motionid[motionlist[j].boneid]==-1||motionlist[motionid[motionlist[j].boneid]].frame>motionlist[j].frame)
	   &&f>=endframe[motionlist[j].boneid]){
	  motionid[motionlist[j].boneid] = j;
	}
      }

      //collision detection and workaround
      for(int i=0;i<model->bonecount;i++){
	for(int j=i;j<model->bonecount;j++){
	  if(i!=j&&motionid[j]==donemotioncount){
	    motionid[j] = motionid[i];
	  }
	}
	if(motionid[i]!=-1){
	  endframe[motionlist[motionid[i]].boneid] = motionlist[motionid[i]].frame;
	  {//swap donemotioncount, motionid[i]
	    motion_t tmp;
	    tmp = motionlist[donemotioncount];
	    motionlist[donemotioncount] = motionlist[motionid[i]];
	    motionlist[motionid[i]]=tmp;
	    donemotioncount++;
	  }
	}
      }
      if(donemotioncount == (int)motioncount-1)break;
      if(pdc==donemotioncount){
	if(time > 100)break;
	time++;
      }
      else{time=0;pdc=donemotioncount;}
      // {
      // 	int tm=endframe[0];
      // 	for(int i=1;i<model->bonecount;i++){
      // 	  if(endframe[i]>0&&tm>endframe[i])tm=endframe[i];
      // 	}
      // 	printf("f=%d,tm=%d\n",f,tm);
      // 	f=tm+1;
      // }
    }
    free(endframe);
  }
}

