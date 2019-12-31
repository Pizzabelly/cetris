#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "drawable.h"

void refresh_current(cetris_ui *ui) {
  for (int i = 0; i < 4; i++) {
    ui->board.ghost[i].visable = false;
    ui->board.current[i].visable = false;
  }
}

void load_tetris_board(cetris_ui *ui, tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  board->block_width = w / (GLfloat)board->game.config.board_x;
  board->block_height = h / board->game.config.board_visible;

  board->x_offset = x;
  board->y_offset = y;
  board->block_offset = (board->game.config.board_y - board->game.config.board_visible);
  
  board->board_count = 0;
  board->board = malloc(sizeof(drawable_t) * 100);
  memset(board->board, 0, sizeof(drawable_t) * 100);

  board->update_current = false;
  board->lock_current = false;
  board->refresh_board = false;

  memset(&ui->board.current, 0, sizeof(drawable_t) * 4);
  memset(&ui->board.ghost, 0, sizeof(drawable_t) * 4);
  
  for (int i = 0; i < 4; i++) {
    new_rectangle(&ui->board.current[i]);
    new_rectangle(&ui->board.ghost[i]);
  }

  refresh_current(ui);

  update_rect(&board->skin.playboard, board->x_offset, board->y_offset - 10 * ui->res_scale,
      board->game.config.board_x * board->block_width,
      (board->game.config.board_visible * board->block_height) + 10 * ui->res_scale,
      ui->window_width, ui->window_height);

  update_rect(&board->skin.border, board->x_offset - (110 * ui->res_scale), 
      board->y_offset - (45 * ui->res_scale), 380 * ui->res_scale, 600 * ui->res_scale,
      ui->window_width, ui->window_height);
}

void load_held_piece(tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  board->held.block_width = w / 4.0f;
  board->held.block_height = h / 4.0f;

  board->held.x_offset = x;
  board->held.y_offset = y;
}

void load_piece_queue(tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  board->queue.block_width = w / 4.0f;
  board->queue.block_height = w / 4.0f;

  board->queue.h = h;
  board->queue.w = w;
 
  board->queue.x_offset = x;
  board->queue.y_offset = y;
}

// TODO: make this better
void update_current_drawable(cetris_ui *ui, drawable_t *current, drawable_t* ghost) {
  refresh_current(ui);
  int cur_piece = -1;
  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
      if ((ui->board.game.current.m[s]>>(3 - j))&1) {
        cur_piece++;

        GLfloat block_x = ui->board.x_offset + 
          (j + ui->board.game.current.pos.x) * ui->board.block_width;

        if (ghost) {
          GLfloat ghost_y = ui->board.y_offset + 
            (s + ui->board.game.current.ghost_y - ui->board.block_offset) 
            * ui->board.block_height;

          ghost[cur_piece].visable = true;
          set_block_texture(&ghost[cur_piece], ui->board.game.current.t + 7);
          update_rect(&ghost[cur_piece], block_x, ghost_y, 
              ui->board.block_width, ui->board.block_height,
              ui->window_width, ui->window_height);
        }

        if (s + ui->board.game.current.pos.y < ui->board.block_offset - 1) continue;

        GLfloat block_y = ui->board.y_offset + 
          (s + ui->board.game.current.pos.y - ui->board.block_offset)
          * ui->board.block_height;

        current[cur_piece].visable = true;
        set_block_texture(&current[cur_piece], ui->board.game.current.t);
        update_rect(&current[cur_piece], block_x, block_y, 
            ui->board.block_width, ui->board.block_height,
            ui->window_width, ui->window_height);

      }
    }
  }
}

void lock_current_drawable(cetris_ui *ui) {
  int current = 0;
  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
      if ((ui->board.game.lock_event.m[s]>>(3 - j))&1) {
        new_rectangle(&ui->board.board[ui->board.board_count + current]);

        GLfloat block_x = ui->board.x_offset + 
          (j + ui->board.game.lock_event.pos.x) * ui->board.block_width;

        GLfloat block_y = ui->board.y_offset + 
          (s + ui->board.game.lock_event.pos.y - ui->board.block_offset)
          * ui->board.block_height;

        set_block_texture(&ui->board.board[ui->board.board_count + current], ui->board.game.lock_event.t);
        update_rect(&ui->board.board[ui->board.board_count + current], block_x, block_y, 
            ui->board.block_width, ui->board.block_height,
            ui->window_width, ui->window_height);

        ui->board.board_count++;
      }
    }
  }
}

void refresh_board(cetris_ui *ui) {
  ui->board.board_count = 0;
  for (int s = 0; s < ui->board.game.config.board_x; s++) {
    for (int j = 0; j < ui->board.game.config.board_y; j++) {
      if (ui->board.game.board[s][j] & SLOT_OCCUPIED) {
        set_block_texture(&ui->board.board[ui->board.board_count], ui->board.game.board[s][j] >> 5);
        update_rect(&ui->board.board[ui->board.board_count++], 
            ui->board.x_offset + (s * ui->board.block_width),
            ui->board.y_offset + ((j - ui->board.block_offset) * ui->board.block_height),
            ui->board.block_width, ui->board.block_height,
            ui->window_width, ui->window_height);
      }
    }
  }
}

void draw_tetris_board(cetris_ui *ui) {
  if (ui->board.update_current) {
    update_current_drawable(ui, &ui->board.current[0], &ui->board.ghost[0]);
    ui->board.update_current = false;
  }

  if (ui->board.lock_current) {
    lock_current_drawable(ui);
    ui->board.lock_current = false;
  }

  if (ui->board.refresh_board) {
    refresh_board(ui);
    ui->board.refresh_board = false;
  }

  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, ui->board.skin.background.texture);
  glBindVertexArray(ui->board.skin.background.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindTexture(GL_TEXTURE_2D, 0); 

  glBindTexture(GL_TEXTURE_2D, ui->board.skin.playboard.texture);
  glBindVertexArray(ui->board.skin.playboard.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, ui->board.skin.overlay.texture);
  glBindVertexArray(ui->board.skin.overlay.vao);

  for (int y = 0; y < ui->board.game.config.board_visible + 1; y++) {
    GLfloat height = ui->board.block_height;
    GLfloat y_pos = (ui->board.y_offset - ui->board.block_height) + (y * ui->board.block_height);
    if (y == 0) {
      ui->board.skin.overlay.vertices[4] = ui->board.skin.overlay.vertices[9] = 0.6f;
      height = 10 * ui->res_scale;
      y_pos += 15 * ui->res_scale;
    }
    set_shine(&ui->board.skin.overlay, ui->board.skin.overlay_shine);
    update_rect(&ui->board.skin.overlay, ui->board.x_offset, 
        y_pos, ui->board.block_width * ui->board.game.config.board_x,
        height, ui->window_width,
        ui->window_height);
    if (ui->board.skin.overlay_shine > 0)
      ui->board.skin.overlay_shine -= 0.00025f;
    if (y == 0)
      ui->board.skin.overlay.vertices[4] = ui->board.skin.overlay.vertices[9] = 0.0f;
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, ui->board.skin.block.texture);
  for (int i = 0; i < ui->board.board_count; i++) {
    glBindVertexArray(ui->board.board[i].vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  for (int i = 0; i < 4; i++) {
    if (ui->board.current[i].visable) {
      glBindVertexArray(ui->board.current[i].vao);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    if (ui->board.ghost[i].visable) {
      glBindVertexArray(ui->board.ghost[i].vao);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, ui->board.skin.border.texture);
  glBindVertexArray(ui->board.skin.border.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_held_piece(cetris_ui *ui) {
  if (!ui->board.game.piece_held) return;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, ui->board.skin.block.texture);
  glBindVertexArray(ui->board.skin.block.vao);

  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
      if ((ui->board.game.held.m[s]>>(3 - j))&1) {
        GLfloat block_x = 
          ui->board.held.x_offset + (j * ui->board.held.block_width);
        GLfloat block_y = 
          ui->board.held.y_offset + (s * ui->board.held.block_height);

        if (ui->board.game.held.t == MINO_O) {
          block_x -= ui->board.held.block_width / 1.5f;
        }
        if (ui->board.game.held.t == MINO_I) {
          block_x -= ui->board.held.block_width / 1.5f;
        }

        set_block_texture(&ui->board.skin.block, ui->board.game.held.t);
        update_rect(&ui->board.skin.block, block_x, block_y, 
            ui->board.held.block_width, ui->board.held.block_height,
            ui->window_width, ui->window_height);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}


void draw_piece_queue(cetris_ui *ui) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, ui->board.skin.block.texture);
  glBindVertexArray(ui->board.skin.block.vao);
  
  for (int i = 0; i < 5; i++) {
  int index = (ui->board.game.current_index + i);
  int mino;

  if (index <= 6) {
    mino = ui->board.game.piece_queue[index];
  } else {
    index = index % 7;
    mino = ui->board.game.next_queue[index];
  }

  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
        
      if ((default_matrices[mino][s]>>(3 - j))&1) {
        GLfloat block_x = 
          ui->board.queue.x_offset + (j * ui->board.queue.block_width);
        GLfloat block_y = 
          ui->board.queue.y_offset + (ui->board.queue.h * (i / 5.0)) + (s * ui->board.queue.block_height);

        if (ui->board.game.held.t == MINO_O) {
          block_x -= ui->board.queue.block_width / 1.5f;
        }
        if (ui->board.game.held.t == MINO_I) {
          block_x -= ui->board.queue.block_width / 1.5f;
        }

        set_block_texture(&ui->board.skin.block, mino);
        update_rect(&ui->board.skin.block, block_x, block_y, 
            ui->board.queue.block_width, ui->board.queue.block_height,
            ui->window_width, ui->window_height);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
      }
    }
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}
