#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define DISPLAY_ADDR (DEVICE_BASE + 0x01000000)

void __am_gpu_init() {
   int i;
   int w = 400;  // TODO: get the correct width
   int h = 300;  // TODO: get the correct height
   uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
   for (i = 0; i < w * h; i ++) 
    fb[i] = i;
   outl(SYNC_ADDR, 1);
}
 
void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = 400, .height = 300,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if(ctl->sync)
  {
      for (int i = x ; i <= x + w - 1 ; i++)
        for (int j = y ; j <= y + h - 1 ; j++)
          {
            outl(DISPLAY_ADDR + 1600 * i + 4 * j, fb[400 * i + j]);
          }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
