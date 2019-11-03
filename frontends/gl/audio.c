#ifdef _WIN32
#define format_str sprintf_s
#else
#define format_str snprintf
#endif

#include <SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include "audio.h"

void load_multiple_audio(char* name, char* dir_name, int *count, Mix_Chunk*** list) {
  char file[120];

  *count = 0;
  *list = (Mix_Chunk **)malloc(sizeof(Mix_Chunk *) * 2);

  while(1) {
    format_str(file, 120, "%s/%s_%i.wav", dir_name, name, *count);
    
    (*list)[*count] = Mix_LoadWAV(file);

    if ((*list)[*count]) {
      (*count)++;
      *list = (Mix_Chunk **)realloc(*list, sizeof(Mix_Chunk *) * (*count + 1));
    } else break;
  }
}

void load_audio(char* name, char* dir_name, Mix_Chunk** chunk) {
  char file[120];
  format_str(file, 120, "%s/%s.wav", dir_name, name);
  *chunk = Mix_LoadWAV(file);
}
