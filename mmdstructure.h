#ifndef _MMD_STRUCTURE_H
#define _MMD_STRUCTURE_H

#ifdef __cplusplus
extern "C"{
#endif

#if 0// to trick text editor auto indent :)
}
#endif

#include <stdint.h>

typedef struct _pmdheader {
  uint8_t magic[3];
  float version;
  char name[20];
  char comment[256];
} __attribute__((__packed__)) pmdheader;

typedef struct _pmdvertex {
  float pos[3];
  float normalvec[3];
  float uv[2];
  uint16_t bonenum[2];
  uint8_t boneweight;
  uint8_t edgeflag;
} __attribute__((__packed__)) pmdvertex;

typedef struct _pmdmaterial {// material, including texture
  float diffusecolor[3];
  float alpha;
  float specularity;
  float specularcolor[3];
  float mirrorcolor[3];
  uint8_t toonindex;
  uint8_t edgeflag;
  uint32_t facevert_count;
  char texture_filename[20];
} __attribute__((__packed__)) pmdmaterial;

typedef struct pmdbone {// for skeletal animation; bone based animation
  char name[20];
  uint16_t parent;	// parent bone index, 0xFFFF if nothing
  uint16_t tailpos;	// 0xFFFF if nothing
  uint8_t type;			// 0:rotate 1:rotate and translate 2:IK 3:unknown 4:IK effected 5:rotate effected 6:IK connected 7:unvisible 8:twist 9:rotation and translation? (8, 9: MMD v4.0 or later)
  uint16_t ikparent;// 0 if nothing
  float headpos[3];
} __attribute__((__packed__)) pmdbone;

typedef struct _pmdik { //for inverse kinematics
  uint16_t root;
  uint16_t target;
  uint8_t len;
  uint16_t iterations;
  float ctrlweight;// control weight is to limit angle / cycle
} __attribute__((__packed__)) pmdik;

typedef struct _pmdmorphs {
  char name[20];
  uint32_t vertcount;
  uint8_t type;
} __attribute__((__packed__)) pmdmorphs;

typedef struct _pmdbonedisp {
  uint16_t index;
  uint8_t frameindex;
} __attribute__((__packed__)) pmdbonedisp;

//Physics not implemented yet... todo? execute fron flash needed?

/// VMD構造体定義
typedef struct _vmdheader {
  uint8_t header[30];
  char motionname[20];
}__attribute__((__packed__)) vmdheader;

typedef struct _vmdmotion {
  char bonename[15];
  uint32_t frame;
  float location[3];    // model local
  float rotation[4];	// model local
  uint8_t interpolation[64]; /*Bejier curve interporation*/
} __attribute__((__packed__)) vmdmotion;

#ifdef __cplusplus
}
#endif

#endif
