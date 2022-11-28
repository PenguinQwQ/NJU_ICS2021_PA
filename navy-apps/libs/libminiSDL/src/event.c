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
static bool flag = false;

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
    if(strcmp(buf + 3,keyname[i]) == 0)
    {       
      ev->key.keysym.sym = i;
      break;
    }
    key_state[ev->key.keysym.sym] = 1 ^ ev->type;
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

/*
#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t kty_st[128];

int SDL_PushEvent(SDL_Event *ev) {
    if(ev==NULL) return 0;
    printf("No implemented!\n");assert(0);
    return 0;
}

int SDL_PollEvent(SDL_Event *ev) {   
    char buf[64];memset(buf,0,sizeof(buf));
    bool flag=NDL_PollEvent(buf,sizeof(buf));
    if(!flag) return 0;
    if(ev==NULL) return 1;
    if(buf[1]=='u') ev->type=SDL_KEYUP;
    else if(buf[1]=='d') ev->type=SDL_KEYDOWN;
    int len=strlen(buf);
    buf[len-1]='\0';
    int size=sizeof(keyname);
    for(int i=0;i<size;i++)
    if(!strcmp(buf+3,keyname[i])){       
      ev->key.keysym.sym=i;break;
    }
    kty_st[ev->key.keysym.sym]=1^ev->type;
   // printf("%s,%d\n",buf,kty_st[ev->key.keysym.sym]);
    return 1;
    //printf("No implemented!\n");assert(0);return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
    if(event==NULL) return 0;
    event->type=SDL_USEREVENT;event->key.keysym.sym=0;
    char buf[64];memset(buf,0,sizeof(buf));
    while(!NDL_PollEvent(buf,sizeof(buf)));   
      if(buf[1]=='u') event->type=SDL_KEYUP;
      else if(buf[1]=='d') event->type=SDL_KEYDOWN;
      int len=strlen(buf);
      buf[len-1]='\0';
      int size=sizeof(keyname);
      for(int i=0;i<size;i++)
      if(!strcmp(buf+3,keyname[i])){       
        event->key.keysym.sym=i;
        //printf("%d,%d\n",SDLK_0,i);
        break;
      }
      kty_st[event->key.keysym.sym]=event->type;
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

static bool is_called=0;
uint8_t* SDL_GetKeyState(int *numkeys) {
    if(!is_called) memset(kty_st,0,sizeof(kty_st));
    is_called=1;
    return kty_st;
}
*/