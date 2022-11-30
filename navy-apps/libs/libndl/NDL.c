#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canva_w = 0, canva_h = 0, canva_x = 0, canva_y = 0;
static char file_buf[128];
#define FILE_BUF_SIZE 128
static int frame_buffer_fd = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int flag = read(fd, buf, len);
  close(fd);
  return flag ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if(*w == 0 && *h == 0){
    *w = screen_w;
    *h = screen_h;
  }
  canva_h = *h;
  canva_w = *w;
  assert(canva_h <= screen_h && canva_w <= screen_w);
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }

}


void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  assert(w >= 0 && h >= 0);
  assert(x + w <= canva_w && y + h <= canva_h);
  canva_x = (screen_w - canva_w) >> 1;
  canva_y = (screen_h - canva_h) >> 1;
  if(w == 0 && h == 0){
    w = canva_w;
    h = canva_h;
  }
  uint32_t *pix = pixels + y * canva_w + x;
  uint32_t offset = ((canva_y + y) * screen_w + (canva_x + x)) * sizeof(uint32_t);
  for(int i = 0; i < h; i++){
    lseek(frame_buffer_fd, offset, SEEK_SET);
    write(frame_buffer_fd, pix, w << 2);
    pix += canva_w;
    offset += screen_w << 2; 
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  int fd = open("/proc/dispinfo", 0, 0);
  assert(read(fd, file_buf, FILE_BUF_SIZE));

//  FILE *fp = fopen("/proc/dispinfo", "r");
//  fscanf(fp, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
//  printf("screen_w is %d, screen_h is %d \n", screen_w, screen_h);

  strtok(file_buf,":\n");
  //Get screen_w
  screen_w = atoi(strtok(NULL,":\n"));
  strtok(NULL,":\n");
  //Get screen_h
  screen_h = atoi(strtok(NULL,":\n"));

  close(fd);

  frame_buffer_fd = open("/dev/fb", 0, 0);
  return 0;
}

void NDL_Quit() {
  close(frame_buffer_fd);
}

