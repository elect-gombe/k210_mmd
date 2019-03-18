/**
Copyright (c) 2018 Gombe.

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/
#ifndef _VMD_H
#define _VMD_H

#include "quaternion.hpp"
#include "fvector3.hpp"
#include "pmd.hpp"

struct motion_t{
  int frame;
  fvector3_t pos;
  quaternion_t rotation;
  int boneid;
  /*todo implement non linear interporation*/
};

/*==help wanted==
 * implement demand loading while drawing, for save memory.
 *
 * 1min30sec motion data => 15k motions, 1motion=36byte, 36B*15k=540Kbyte
 * message queue and real-time controller?
 * 
 * or just write to spi-flash and read in place.
 */
class pmd;

class vmd{
public:
  uint32_t motioncount;
  motion_t *motionlist;

  vmd(){
    motionlist = NULL;
  }

  void load(const char *path,pmd* model); /*model specified loader*/
  
  ~vmd(){
    if(motionlist!=NULL)
      free(motionlist);
    motionlist = NULL;
  }
};

#endif //_VMD_H
