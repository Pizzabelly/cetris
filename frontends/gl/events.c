#ifdef _WIN32
#include <windows.h>
#else
#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#endif

#include "ui.h"
#include <SDL_mixer.h>

void handle_game_event(cetris_ui *ui) {
/*
  if (board->game.lock_event > 0) {
    Mix_PlayChannel( 1, board->skin.lock_sound, 0 );
    board->game.lock_event--;
  }
  */

  //if (board->game.line_event > 0) {
  
  /*
  if (ui->board.game.tetris_event > 0) {
    //int index = rand() % ui->skin.tetris_sound_count;
    Mix_PlayChannel( 1, ui->board.skin.tetris_sound[0], 0 );
    ui->board.game.tetris_event--;
  }
  */

  if (ui->board.game.move_event > 0) {
    //int index = rand() % ui->skin.tetris_sound_count;
    ///Mix_HaltChannel(0);
    Mix_PlayChannel( 0, ui->board.skin.move_sound, ui->board.game.move_event - 1);
    ui->board.game.move_event = 0;
    printf("sound\n");
  }
}

#ifdef _WIN32
DWORD WINAPI game_event_loop(void* data) {
  cetris_ui *ui = (cetris_ui*)data;
  while (1) {
    handle_game_event(ui);
    Sleep(1);
  }
}
void start_event_thread(cetris_ui *ui) {
  HANDLE thread = CreateThread(NULL, 0, game_event_loop, ui, 0, NULL);
}
#else
void *game_event_loop(void* data) {
  cetris_ui *ui = (cetris_ui*)data;
  while (1) {
    handle_game_event(ui);
    usleep(1);
  }
  return 0;
}
void start_event_thread(cetris_ui *ui) {
  pthread_t thread;
  pthread_create(&thread, NULL, game_event_loop, (void*)ui);
}
#endif

