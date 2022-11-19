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
//  char *str = (char *)buf;
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);
  if(kbd.keycode == AM_KEY_NONE) return 0;
  bool isdown = kbd.keydown;
  if(isdown) strcat(buf,"kd ");//This means down keyboard!
  else strcat(buf,"ku "); //This means up keyboard!
  strcat(buf, keyname[kbd.keycode]);
  assert(strlen(buf) <= len);
  return strlen(buf);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
