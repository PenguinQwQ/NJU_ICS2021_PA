#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *fd = fopen(filename,"rb");
  fseek(fd, 0, SEEK_END);
  int sz = ftell(fd);
  unsigned char * buf = malloc(sz);
  fseek(fd,0,SEEK_SET);
//  printf("File %s,IMG_SIZE:%d\n",filename,size);
  assert(fread(buf, 1, sz, fd) == sz);
//  for(int i=0;i<size;++i) putchar(buf[i]);putchar('\n');
  SDL_Surface * IMG = STBIMG_LoadFromMemory(buf, sz);
//  printf("About to Complete!\n");
  assert(IMG);
  fclose(fd);
  free(buf);
//  printf("Complete!\n");
  return IMG;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
