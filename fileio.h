#ifndef _FILE_IO_H
#define _FILE_IO_H


#if PC
typedef void* fil;
#include <unistd.h>
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
int filread(fil fp,void *buff,size_t byte);

//fail
static inline
void fail(){
  //todo
  while(1)usleep(114514);
}

#if defined(__cplusplus)
};
#endif

#endif//_FILE_IO_H
