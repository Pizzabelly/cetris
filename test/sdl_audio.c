#include <SDL2/SDL.h>

int main(void) {
  int i, count = SDL_GetNumAudioDevices(0);

  printf("count: %i\n", count);
  for (i = 0; i < count; ++i) {
      SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
      printf("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
  }
  return 0;
}
