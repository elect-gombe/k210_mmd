/**
Copyright (c) 2018 Gombe.

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/
#ifndef __VECTOR2
#define __VECTOR2
#include <stdint.h>
#include "fvector2.hpp"

struct vector2_t{
  int32_t x;
  int32_t y;
};

struct vector2{
  int32_t x;
  int32_t y;

  vector2(){
    x=0;
    y=0;
  }

  vector2(int32_t x,int32_t y){
    this->x = x;
    this->y = y;
  }

  vector2(const vector2& v){
    this->x = v.x;
    this->y = v.y;
  }

  vector2(const fvector2& v){
    this->x = v.x;
    this->y = v.y;
  }

  vector2& operator=(const vector2& v){
    this->x = v.x;
    this->y = v.y;
    return *this;
  }

  vector2 operator+(){
    return *this;
  }
  vector2 operator-(){
    return vector2(-x,-y);
  }

  vector2& operator+=(const vector2& v){
    this->x += v.x;
    this->y += v.y;
    return *this;
  }

  vector2& operator-=(const vector2& v){
    this->x -= v.x;
    this->y -= v.y;
    return *this;
  }

  void print(void);
};

inline
vector2 operator+(const vector2& v1,const vector2& v2){
  vector2 w;

  w.x = v1.x+v2.x;
  w.y = v1.y+v2.y;

  return w;
}

inline
vector2 operator*(const vector2& v1,int32_t n){
  vector2 w;

  w.x = v1.x*n;
  w.y = v1.y*n;

  return w;
}

inline
vector2 operator-(const vector2& v1,const vector2& v2){
  vector2 w;

  w.x = v1.x-v2.x;
  w.y = v1.y-v2.y;

  return w;
}

#endif
