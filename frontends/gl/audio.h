#pragma once

#ifdef _WIN32
#define format_str sprintf_s
#else
#define format_str snprintf
#endif

#include <SDL_mixer.h>

void load_multiple_audio(char* name, char* dir_name, int *count, Mix_Chunk*** list);
void load_audio(char* name, char* dir_name, Mix_Chunk** chunk);
