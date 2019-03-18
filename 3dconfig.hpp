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
//#define DISABLE_OUTPUT

#define DRAW_NLINES (window_height)
#define MAXPROC_POLYNUM (300)

#define USE_K210

#if defined(PC)
#undef USE_K210
#define PROCESSNUM 2
#define PTHREAD
#endif


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
#endif
#else//k210
// #define ENDIAN_LITTLE
 #define OMIT_ZBUFFER_CONFLICT
 #define PROCESSNUM 2
// #define OVER_VOLTAGE
#endif


#ifdef OUTPUTTERMINAL
#undef ENDIAN_LITTLE
#endif

#endif


//k210 configulation reports
// 2540 triangles
// pure drawing framerate
//400MHz
// 1core,45fps, 2core:71fps
//500MHz
//              2core:85fps
//600MHz
//              2core:107fps~
//700MHz~ does not working :(

//160x120 600 dual core, onscreen, 2540 triangles, 155fps
