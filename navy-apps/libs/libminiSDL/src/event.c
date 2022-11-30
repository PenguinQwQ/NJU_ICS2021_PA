#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define BUF_SIZE 256
static uint8_t key_state[128];
static char buf[256];
static int sz = sizeof(keyname);

int SDL_PushEvent(SDL_Event *ev) {
    if(ev == NULL) return 0;
    printf("Not Implemented!!!");
    assert(0);
    return 0;
}

int SDL_PollEvent(SDL_Event *ev) {   
   memset(buf, 0, BUF_SIZE);
   if(NDL_PollEvent(buf, BUF_SIZE) == false) return 0;
   if(ev == NULL) return 1;
    switch (buf[1])
    {
    case 'u': ev->type = SDL_KEYUP; break;
    case 'd': ev->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
    buf[strlen(buf) - 1] = '\0';
    for(size_t i = 0 ; i < sz ; i++)
    if(strcmp(buf + 3, keyname[i]) == 0)
    {       
      ev->key.keysym.sym = i;
      key_state[i] = (ev->type == SDL_KEYDOWN) ? 1 : 0;
      break;
    }
    return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
    if(event == NULL) return 0;
    event->type = SDL_USEREVENT;
    event->key.keysym.sym = 0;
    memset(buf, 0, BUF_SIZE);
    while(NDL_PollEvent(buf, BUF_SIZE) == false);   
    switch (buf[1])
    {
    case 'u': event->type = SDL_KEYUP; break;
    case 'd': event->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
    buf[strlen(buf) - 1] = '\0';
      for(size_t i = 0 ;i < sz;i++)
      if(strcmp(buf + 3,keyname[i]) == 0){       
        event->key.keysym.sym = i;
        key_state[i] = (event->type == SDL_KEYDOWN) ? 1 : 0;
        break;
      }
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}
uint8_t* SDL_GetKeyState(int *numkeys) {
    return key_state;
}
