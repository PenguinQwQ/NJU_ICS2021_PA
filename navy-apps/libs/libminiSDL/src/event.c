#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  Log("Not implemented SDL_PushEvent!");
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  Log("Not implemented SDL_PollEvent!");
  assert(0);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  Log("Not implemented SDL_PushEvent!");
  assert(0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  Log("Not implemented SDL_PeepEvent!");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  Log("Not implemented SDL_GetKeystate!");
  assert(0);
  return NULL;
}
