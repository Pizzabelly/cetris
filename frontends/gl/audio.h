#pragma once

#ifdef _WIN32
#include <SDL.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#define format_str snprintf
#endif

typedef struct {
  SDL_AudioSpec wav_spec;
  uint32_t wav_length;
  uint8_t *wav_buffer;
} audio_clip_t;

void load_multiple_audio(char* name, char* dir_name, int *count, audio_clip_t* list);
SDL_AudioDeviceID load_audio();
