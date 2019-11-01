#ifdef _WIN32
#include <SDL.h>
#include <SDL_mixer.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#define format_str snprintf
#endif

#include "audio.h"

void load_multiple_audio(char* name, char* dir_name, int *count, Mix_Chunk** list) {
  char file[120];

  int index = 0;
  for (int i = 0; i < (*count); i++) {
    format_str(file, 120, "%s/%s_%i.wav", dir_name, name, i);
    
    list[index] = Mix_LoadWAV(file);
    if (!list[index]) {
      (*count)--;
    } else index++;
  }
}
