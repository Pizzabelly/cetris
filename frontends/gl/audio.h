#pragma once

#ifdef _WIN32
#include <SDL.h>
#include <SDL_mixer.h>
#define format_str sprintf_s
#else
#include <SDL2/SDL.h>
#define format_str snprintf
#endif

void load_multiple_audio(char* name, char* dir_name, int *count, Mix_Chunk** list);
