/**
Copyright (c) 2018 Gombe.

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _3DCONFIG
#define _3DCONFIG

const static int window_width = 320;
const static int window_height = 240;

//#define DISABLE_ANIMATION

#define DRAW_NLINES (window_height/2)
#define MAXPROC_POLYNUM (300)

#define USE_K210

#ifndef USE_K210
#ifndef PC //for esp32
  #define ENDIAN_LITTLE
  #define OMIT_ZBUFFER_CONFLICT
//comment out if U use M5Stack.
  #define ILI9341
  #define PROCESSNUM 2
#else
//  #define OUTPUTTERMINAL
//  #define USE_SDL
  #define PROCESSNUM 1
#endif
#else
// #define ENDIAN_LITTLE
 #define OMIT_ZBUFFER_CONFLICT
 #define PROCESSNUM 1
#endif


// DO NOT CHANGE HERE
#ifdef OUTPUTTERMINAL
#undef ENDIAN_LITTLE
#endif

#endif
