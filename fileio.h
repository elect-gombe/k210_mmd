#ifndef _FILE_IO_H
#define _FILE_IO_H

#include "3dconfig.hpp"
#if defined(PC)
typedef void* fil;
#include <unistd.h>
#elif defined(USE_K210)
 #include "ff.h"
 typedef FIL fil;
#endif


#if defined(__cplusplus)
extern "C"{
#endif
#if 0
}
#endif
//file open
//return 0 if success
int filopen(const char *pathname,fil *f);

//file read
int filread(fil *fp,void *buff,size_t byte);

//fail
static inline
void fail(){
  //todo
  while(1)
#if defined(PC)
    usleep(114514);
#else
  ;
#endif
}

#if defined(__cplusplus)
};
#endif

#endif//_FILE_IO_H
