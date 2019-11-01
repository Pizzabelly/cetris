#ifdef _WIN32
#include <SDL.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#define format_str snprintf
#endif

#include "audio.h"

SDL_AudioDeviceID load_audio() {
  SDL_AudioSpec spec;
  SDL_memset(&spec, 0, sizeof(spec));
  spec.freq = 44100;
  spec.format = AUDIO_S16;
  spec.channels = 2;
  spec.samples = 4096;
  spec.callback = NULL;

  return SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
}

void load_multiple_audio(char* name, char* dir_name, int *count, audio_clip_t* list) {
  char file[120];

  int index = 0;
  for (int i = 0; i < (*count); i++) {
    format_str(file, 120, "%s/%s_%i.wav", dir_name, name, i);

    if (SDL_LoadWAV(file, &list[index].wav_spec, 
        &list[index].wav_buffer, &list[index].wav_length) == NULL) {
      (*count)--;
    } else index++;
  }
}
