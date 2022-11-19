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

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int bytes = read(fd, buf, len);
  close(fd);
  if(bytes) return 1;
  else return 0;
}
static int canva_h, canva_w, canva_x, canva_y;
void NDL_OpenCanvas(int *w, int *h) {
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
  if(*w == 0 && *h == 0){
    canva_h = screen_h;
    canva_w = screen_w;
    *w = screen_w;
    *h = screen_h;
  }
  else{
    canva_h = *h;
    canva_w = *w;
  }
//  printf("The canvas width is %d , height is %d\n", canva_w, canva_h);
//  assert(canva_h <= screen_h && canva_w <= screen_w);
  canva_x = (screen_w - canva_w) >> 1;
  canva_y = (screen_h - canva_h) >> 1;
}

static int frame_buffer_fd = 0;
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if(w == 0 && h == 0){
    w = canva_w;
    h = canva_h;
  }
  uint32_t *pix= pixels + y * canva_w + x;
  uint32_t scroff = ((canva_y + y) * screen_w + (canva_x + x)) << 2;
  for(int i = 0; i < h; i++){
    lseek(frame_buffer_fd, scroff, SEEK_SET);
    write(frame_buffer_fd, pix, w << 2);
    pix = pix + canva_w;
    scroff += screen_w << 2; 
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
  char buf[64];
  assert(read(fd,buf,sizeof(buf)));
  strtok(buf,":\n");
  screen_w = atoi(strtok(NULL,":\n"));
  strtok(NULL,":\n");
  screen_h = atoi(strtok(NULL,":\n"));
  close(fd);
  frame_buffer_fd = open("/dev/fb", 0, 0);
  //printf("screen_h = %d, screen_w = %d\n",screen_h,screen_w);
  return 0;
}

void NDL_Quit() {
  close(frame_buffer_fd);
}


/*
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

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_usec / 1000 + tv.tv_sec*1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int bytes = read(fd, buf, len);
  close(fd);
  if(bytes) return 1;
  else return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
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
    //close(fbctl);
  }
  if(*w == 0 && *h == 0){
    canva_h = screen_h;
    canva_w = screen_w;
    *w = screen_w;
    *h = screen_h;
  }
  else{
    canva_h = *h;
    canva_w = *w;
  }
  assert(canva_h <= screen_h && canva_w <= screen_w);
  canva_x = (screen_w - canva_w) >> 1;
  canva_y = (screen_h - canva_h) >> 1;
  //printf("cx = %d, cy = %d, cw = %d, ch = %d\n",canva_x,canva_y,canva_w,canva_h);
}
int fbfd = 0;
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if(w == 0 && h == 0){
    w = canva_w;
    h = canva_h;
  }
  //assert(0);
  
  uint32_t *fixoff = pixels + y*canva_w + x;
  uint32_t scroff = ( (canva_y + y) * screen_w + (canva_x + x) ) << 2;
  for(int i = 0; i < h; i++){
    //lseek(fd,(canva_y+i+y)*screen_w+canva_x+x,0);
    //printf("scroff = %d\n",scroff);  
    lseek(fbfd, scroff, SEEK_SET);
    write(fbfd, fixoff, w << 2);
    fixoff = fixoff + canva_w;
    scroff += screen_w << 2; 
  }
  //close(fd);
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
  char buf[64];
  assert(read(fd,buf,sizeof(buf)));
  strtok(buf,":\n");
  screen_w = atoi(strtok(NULL,":\n"));
  strtok(NULL,":\n");
  screen_h = atoi(strtok(NULL,":\n"));
  close(fd);
  fbfd = open("/dev/fb", 0, 0);
  //printf("screen_h = %d, screen_w = %d\n",screen_h,screen_w);
  return 0;
}

void NDL_Quit() {
  close(fbfd);
}

*/