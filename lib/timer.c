#include <cetris.h>

#ifdef _WIN32
#include <window.h>

#if CETRIS_HI_RES
DWORD WINAPI cetris_game_loop(void* data) {
  cetris_game* game = (cetris_game*)data;
  LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
  LARGE_INTEGER Frequency;
  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&StartingTime);
  while(1) {
    if (game->waiting) break;
    QueryPerformanceCounter(&EndingTime); 
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    game->timer = ElapsedMicroseconds.QuadPart;
    game->tick = ElapsedMicroseconds.QuadPart / 1000;
    if (!update_game_tick(game)) {
	    break;
    }
    Sleep(1);
  }
  return 0;
}
#else
DWORD WINAPI cetris_game_loop(void* data) {
  cetris_game* game = (cetris_game*)data;
  while(1) {
    if (game->waiting) break;
    game->tick += 16;
    if (!update_game_tick(game)) {
	    break;
    }
    Sleep(16); // little less than 60hz
  }
}
#endif
CETRIS_EXPORT void cetris_start_game(cetris_game *g) {
  g->waiting = false;
  HANDLE thread = CreateThread(NULL, 0, cetris_game_loop, g, 0, NULL);
}
CETRIS_EXPORT void cetris_stop_game(cetris_game *g) {
  init_game(g, NULL);
}
#else
#include <pthread.h>
#include <unistd.h>

#if CETRIS_HI_RES
#include <time.h>
void *cetris_game_loop(void* data) {
  cetris_game *game = (cetris_game*)data;
  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
  while (1) {
    if (game->waiting) break;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    long nsec_elapsed = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    game->timer = nsec_elapsed / 1000;
    game->tick = nsec_elapsed / 1000000;
    if (!update_game_tick(game)) {
	    break;
    }
    usleep(1000);
  }
  return 0;
}
#else
void *cetris_game_loop(void *data) {
  cetris_game* game = (cetris_game*)data;
  while(1) {
    if (game->waiting) break;
    game->tick += 16;
    if (!update_game_tick(game)) {
	    break;
    }
    // could be more accurate, keeping
    // it consistant with windows
    usleep(16000);
  }
}
#endif
CETRIS_EXPORT void cetris_start_game(cetris_game *g) {
  g->waiting = false;
  pthread_t thread;
  pthread_create(&thread, NULL, cetris_game_loop, (void*)g);
}
CETRIS_EXPORT void cetris_stop_game(cetris_game *g) {
  init_game(g, NULL);
}
#endif
