#include <glad/glad.h>
#include <stdio.h>

#include "ui.h"
#include "drawable.h"

void load_tetris_board(tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  board->block_width = w / (GLfloat)board->config.board_x;
  board->block_height = h / board->config.board_visible;

  board->x_offset = x;
  board->y_offset = y;
  board->block_offset = (board->config.board_y - board->config.board_visible);
}

void draw_tetris_board(cetris_ui *ui) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ui->skin.overlay.texture);
  glBindVertexArray(ui->skin.overlay.vao);

  for (int y = 0; y < ui->board.game.config.board_visible; y++) {
    update_rect(&ui->skin.overlay, ui->board.x_offset, 
        ui->board.y_offset + (y * ui->board.block_height),
        ui->board.block_width * ui->board.game.config.board_x,
        ui->board.block_height);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, ui->skin.block.texture);

  glBindVertexArray(ui->skin.block.vao);
  for (int x = 0; x < ui->board.game.config.board_x; x++) {
    for (int y = ui->board.game.highest_line; y < ui->board.game.config.board_y; y++) {
      if (ui->board.game.board[x][y] & SLOT_OCCUPIED) {
        set_block_texture(&ui->skin.block, (ui->board.game.board[x][y] >> 5));
  
        update_rect(&ui->skin.block, 
            ui->board.x_offset + (x * ui->board.block_width),
            ui->board.y_offset + ((y - ui->board.block_offset) * ui->board.block_height),
            ui->board.block_width,
            ui->board.block_height);


        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }

  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
      if ((ui->board.game.current.m[s]>>(3 - j))&1) {
        GLfloat block_x = 
          ui->board.x_offset + 
          (j + ui->board.game.current.pos.x) * ui->board.block_width;

        GLfloat block_y = 
          ui->board.y_offset + 
          (s + ui->board.game.current.pos.y - ui->board.block_offset)
          * ui->board.block_height;

        set_block_texture(&ui->skin.block, ui->board.game.current.t);
        update_rect(&ui->skin.block, block_x, block_y, 
            ui->board.block_width, ui->board.block_height);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        GLfloat ghost_y = 
          ui->board.y_offset + 
          (s + ui->board.game.current.ghost_y - ui->board.block_offset) 
          * ui->board.block_height;

        set_block_texture(&ui->skin.block, ui->board.game.current.t + 7);
        update_rect(&ui->skin.block, block_x, ghost_y, 
            ui->board.block_width, ui->board.block_height);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }
  glBindTexture(GL_TEXTURE_2D, 0);

}
