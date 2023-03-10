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
  FILE* fp = fopen(filename,"rb");

  assert(fp != NULL);
  fseek(fp, 0, SEEK_END);
  size_t f_size = ftell(fp);
  unsigned char *buf = SDL_malloc(f_size);
  fseek(fp, 0, SEEK_SET);
  assert(fread(buf, 1, f_size, fp) == f_size);
  SDL_Surface* img_mem = STBIMG_LoadFromMemory(buf, f_size);
  fclose(fp);
  SDL_free(buf);
  return img_mem;
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
