/**
Copyright (c) 2018 Gombe.

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/
#ifndef _PMD_HPP
#define _PMD_HPP

#include "mmdstructure.h"
#include "fvector4.hpp"
#include "fvector3.hpp"
#include "fvector2.hpp"
#include "matrix4.hpp"
#include "images.hpp"
#include "vmd.hpp"

struct ik_t{
  uint16_t rootid;
  uint16_t targetid;
  uint16_t len;
  uint8_t looptimes;
  float anglelimit;
  uint16_t data[1];//variable size, depends on len
};

struct  bone_t{
  int16_t parent;
  fvector3_t pos;
};

struct mixedbone{
  uint8_t weight;
  uint16_t bone[2];
};

class pmd{
public:
  char *name;

  uint32_t vertexcount;
  fvector3_t *vertexlist;
  fvector4 *tvertexlist;//transformed vertexes
  fvector2_t *uvlist;

  /*1 vertex, 4+4*5=24byte, */
  uint32_t *matrixnolist;//[0:7] bone number0, [8:15] bone number 1, [16:23] ratio(0-100, 0...bone number 0(100%), 100...bone number 1(100%), 50...half half.

  uint32_t facecount;
  uint16_t *facelist; // polygonlist[polycount*3]<- it's always triangle only! (mmd restriction)

  uint16_t materialcount;
  imgs::image *materiallist;
  uint16_t *materialfacecountlist;
  
  uint32_t texturecount;
  uint16_t **texture;

  uint16_t bonecount;
  bone_t *bonelist;
  char **bonenamelist;

  // uint32_t mixedbonecount;
  // mixedbone *mixedbonelist;
  
  uint16_t ikcount;
  ik_t **iklist;//ik[0].data[(variable size)]

  uint16_t ikknee[2];/*knee angle limit*/

  Matrix4 m;

  vmd v;
  
  void init(const char *pathname,const char *motionpathname);
  void loadvmd(const char *pathname);
  void calcvertexes(int ps);
  void draw(uint16_t *drawbuff,uint16_t *zbuff,int ps);
};

  
#endif //_PMD_HPP
