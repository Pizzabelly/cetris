#ifdef _WIN32
#include <windows.h>
#else
#define _GNU_SOURCE
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#endif

#include "timer.h"
#include "drawable.h"

#include "ui.h"
#include <SDL.h>
#include <SDL_mixer.h>

void handle_game_event(cetris_ui *ui) {
  if (ui->board.game.waiting) {
    SDL_Delay(100);
    cetris_start_game(&ui->board.game);
  }

  if (ui->board.game.lock_event > 0) {
    Mix_PlayChannel( 1, ui->board.skin.lock_sound, 0 );
    ui->board.game.lock_event--;
  }

  if (ui->board.game.line_event > 0) {
    int index = ui->board.game.line_event - 1;
    ui->board.skin.overlay_shine = 0.25f;
    Mix_PlayChannel( 1, ui->board.skin.clear_sound[index], 0 );
    ui->board.game.line_event = 0;
  }

  if (ui->board.game.combo_event > 0) {
    int index = ui->board.game.line_combo - 1;
    if (index >= ui->board.skin.combo_sound_count) {
      index = ui->board.skin.combo_sound_count - 1;
    }
    Mix_PlayChannel( 2, ui->board.skin.combo_sound[index], 0 );
    ui->board.game.combo_event--;
  }

  if (ui->board.game.tetris_event > 0) {
    int index = rand() % ui->board.skin.tetris_sound_count;
    Mix_PlayChannel( 2, ui->board.skin.tetris_sound[index], 0 );
    ui->board.game.tetris_event--;
  }

  if (ui->board.game.move_event > 0) {
    Mix_PlayChannel( 0, ui->board.skin.move_sound, 0);
    ui->board.game.move_event--;
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

