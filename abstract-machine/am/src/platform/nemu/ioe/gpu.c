#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define DISPLAY_ADDR (DEVICE_BASE + 0x01000000)
static uint32_t w, h;
void __am_gpu_init() {
  w = (uint32_t)inw(VGACTL_ADDR + 2);
  h = (uint32_t)inw(VGACTL_ADDR);
}
 
void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w * h * 4
  }; 
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, width = ctl->w, height = ctl->h;
  uint32_t* p = ctl->pixels;
  for(int i = 0;i < height ; i++)
  	for(int j = 0;j < width ; j++)
  		outl((w * (y + i) + (x + j)) * 4 + FB_ADDR , p[i * width + j]);
  if(ctl->sync)
  {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
