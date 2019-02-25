#ifndef __PROJPOINT_H
#define __PROJPOINT_H

struct proj_point{
  float x;
  float y;
  float z;
  float w;
};


static inline
bool culling(proj_point vertices[]) {
    float P1x = vertices[1].x - vertices[0].x;
    float P1y = vertices[1].y - vertices[0].y;
    float P2x = vertices[2].x - vertices[0].x;
    float P2y = vertices[2].y - vertices[0].y;
    float OPz = P1x*P2y - P1y*P2x;
    return OPz < 0;
}

#endif //__PROJPOINT_H
