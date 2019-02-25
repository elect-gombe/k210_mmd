#ifndef __3DCONFIG
#define __3DCONFIG

#ifdef __cplusplus
extern "C"{
#endif

  int main3d();
  void *vTask(void*);
  int kbhit(void);
  void send_line(int ypos, uint8_t *line);
  uint64_t get_time(void);  
#ifdef __cplusplus
}
#endif
#endif
