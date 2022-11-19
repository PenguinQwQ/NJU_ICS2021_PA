#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char *str = (const char *)buf;
  for (int i = 0 ; i < len ; i++)
    putch(*str++);
  return len;
}


size_t events_read(void *buf, size_t offset, size_t len) {
  *((char *)buf) = 0;
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);
  if(kbd.keycode == AM_KEY_NONE) return 0;
  if(kbd.keydown) strcat(buf,"kd ");//This means down keyboard!
  else strcat(buf,"ku "); //This means up keyboard!
  strcat(buf, keyname[kbd.keycode]);
  strcat(buf, "\n");
  assert(strlen(buf) <= len);
  return strlen(buf);
}


size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  sprintf(buf, "w:%d h:%d\n", gpu.width, gpu.height);
  int ret = strlen(buf);
  assert(ret <= len);
  return ret;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  int x = (offset >> 2) % gpu.width, y = (offset >> 2) / gpu.width;
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len >> 2, 1, true);
  return len;
}


void init_device() {
  Log("Initializing devices...");
  ioe_init();
}


