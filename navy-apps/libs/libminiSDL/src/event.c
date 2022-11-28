/*
#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static char buf[64];
static int KEY_NUM = sizeof(keyname) / sizeof(char *);
static uint8_t key_st[256];
static bool flag = false;


int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {   
    if(ev == NULL) return 1;
    memset(buf, 0, sizeof(buf));
    if(NDL_PollEvent(buf, sizeof(buf)) == false) return 0;
    switch (buf[1])
    {
    case 'u': ev->type = SDL_KEYUP; break;
    case 'd': ev->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
    for(int i = 0;i < KEY_NUM;i++)
    if(strcmp(buf + 3, keyname[i]) == 0){       
      ev->key.keysym.sym = i;
      key_st[i] = 1 ^ ev->type;
      break;
    }
    return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
    if(event == NULL) return 0;
    memset(buf, 0, sizeof(buf));
    event->type = SDL_USEREVENT;
    event->key.keysym.sym = 0;
    while(NDL_PollEvent(buf, sizeof(buf)) == false);   
    switch (buf[1])
    {
    case 'u': event->type = SDL_KEYUP; break;
    case 'd': event->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
      for(int i = 0;i < KEY_NUM;i++)
      if(strcmp(buf + 3, keyname[i]) == 0){       
        event->key.keysym.sym = i;
        key_st[i] = event->type;
        break;
      }
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}
uint8_t* SDL_GetKeyState(int *numkeys) {
    if(!flag) 
    {
      memset(key_st,0,sizeof(key_st));
      flag = true;
    }
    return key_st;
}
*/

#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t key_state[128];
static char buf[256];
static int sz = sizeof(keyname);
static bool flag = false;

int SDL_PushEvent(SDL_Event *ev) {
    if(ev == NULL) return 0;
//    panic("SDL_PushEVENT not implemented!!");
    return 0;
}

int SDL_PollEvent(SDL_Event *ev) {   
    memset(buf, 0, sizeof(buf));
   if(NDL_PollEvent(buf,sizeof(buf)) == false) return 0;
   if(ev == NULL) return 1;
    switch (buf[1])
    {
    case 'u': ev->type = SDL_KEYUP; break;
    case 'd': ev->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
    buf[strlen(buf) - 1] = '\0';
    for(int i = 0 ; i < sz ; i++)
    if(strcmp(buf + 3,keyname[i]) == false){       
      ev->key.keysym.sym = i;
      break;
    }
    key_state[ev->key.keysym.sym] = 1 - ev->type;
    return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
    if(event == NULL) return 0;
    event->type = SDL_USEREVENT;
    event->key.keysym.sym = 0;
    memset(buf, 0, sizeof(buf));
    while(NDL_PollEvent(buf,sizeof(buf)) == false);   
    switch (buf[1])
    {
    case 'u': event->type = SDL_KEYUP; break;
    case 'd': event->type = SDL_KEYDOWN; break;
    default : printf("Cannot identify the type of event!!!\n"); break;
    }
      buf[strlen(buf) - 1] = '\0';
      for(int i = 0;i < sz;i++)
      if(strcmp(buf+3,keyname[i]) == false){       
        event->key.keysym.sym = i;
        break;
      }
      key_state[event->key.keysym.sym] = event->type;
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}
uint8_t* SDL_GetKeyState(int *numkeys) {
    if(flag == false) memset(key_state,0,sizeof(key_state));
    flag = true;
    return key_state;
}

