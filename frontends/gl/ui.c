#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "drawable.h"

void load_tetris_board(tetris_board_t *board, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
  board->block_width = w / (GLfloat)board->config.board_x;
  board->block_height = h / (GLfloat)board->config.board_y;
  printf("%i, %f, %i, %f\n", board->config.board_x, board->block_width, 
                             board->config.board_y, board->block_height);

  board->x_offset = x;
  board->y_offset = y;
  
  board->blocks = (drawable_t **)malloc(sizeof(drawable_t *) * board->config.board_x);
  for (int i = 0; i < board->config.board_x; i++) {
    board->blocks[i] = (drawable_t *)malloc(sizeof(drawable_t) * board->config.board_y);
  }

  for (int s = 0; s < board->config.board_x; s++) {
    for (int j = 0; j < board->config.board_y; j++) {
      new_rectangle(&board->blocks[s][j], x + (s * board->block_width),
          y + (j * board->block_height), board->block_width, board->block_height);
    }
  }

  //glGenTextures(1, &board->blocks[0][0].texture);
  //load_image("blocks.jpg", board->block[0][0].texture);
}

void draw_tetris_board(cetris_ui *ui) {
  //glBindVertexArray(ui->board.block.vao);
  glBindTexture(GL_TEXTURE_2D, ui->skin.block_texture);
  for (int x = 0; x < ui->board.game.config.board_x; x++) {
    for (int y = ui->board.game.highest_line; y < ui->board.game.config.board_y; y++) {
      if (ui->board.game.board[x][y] & SLOT_OCCUPIED) {
        glBindVertexArray(ui->board.blocks[x][y].vao);
        /*
        update_rect(&ui->board.block, 
            ui->board.x_offset + (x * ui->board.block_width),
            ui->board.y_offset + (y * ui->board.block_height),
            ui->board.block_width,
            ui->board.block_height,
            (ui->board.game.board[x][y] >> 5));
        */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      }
    }
  }
}

void draw_current(cetris_ui *ui) {
  //glBindTexture(GL_TEXTURE_2D, ui->skin.block_texture);
  //glBindVertexArray(ui->board.block.vao);
  for (int s = 0; s < 4; s++) {
    for (int j = 0; j < 4; j++) {
      if ((ui->board.game.current.m[s]>>(3 - j))&1) {
        /*
        GLfloat block_x = 
          ui->board.x_offset + 
          (j + ui->board.game.current.pos.x) * ui->board.block_width;

        GLfloat block_y = 
          ui->board.y_offset + 
          (s + ui->board.game.current.pos.y) * ui->board.block_height;
          */
        
        int x = j + ui->board.game.current.pos.x;
        int y = s + ui->board.game.current.pos.y;
        glBindVertexArray(ui->board.blocks[x][y].vao);
        /*
        update_rect(&ui->board.block, block_x, block_y, 
            ui->board.block_width, ui->board.block_height,
            ui->board.game.current.t);
          */

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /*
        GLfloat ghost_y = 
          ui->board.y_offset + 
          (s + ui->board.game.current.ghost_y) * ui->board.block_height;
          */
        
        y = s + ui->board.game.current.ghost_y;
        glBindVertexArray(ui->board.blocks[x][y].vao);
        /*
        update_rect(&ui->board.block, block_x, ghost_y, 
            ui->board.block_width, ui->board.block_height,
            ui->board.game.current.t);
            */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
    }
  }
}
