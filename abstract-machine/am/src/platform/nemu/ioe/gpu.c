#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
   int i;
   int w = 400;  // TODO: get the correct width
   int h = 300;  // TODO: get the correct height
   uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
   for (i = 0; i < w * h; i ++) fb[i] = i;
   outl(SYNC_ADDR, 1);
}
 
void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = 400, .height = 300,
    .vmemsz = 400 * 300 * 4
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  if(ctl->sync)
  {
  for (int i = ctl->x ; i <= ctl->x + ctl->w - 1 ; i++)
    for (int j = ctl->y ; j <= ctl->y + ctl->h - 1 ; j++)
      ctl->pixels = fb + i * 400 + j * 4;
  outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
