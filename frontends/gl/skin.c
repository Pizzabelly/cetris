#include <glad/glad.h>

#include "drawable.h"
#include "audio.h"
#include "skin.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <SDL_mixer.h>

#ifdef _WIN32
#define format_str sprintf_s
#else
#define format_str snprintf
#endif

void load_element(drawable_t* drawable, char* file_name, char* dir_name) {
  char file[125];
  format_str(file, 125, "%s/%s", dir_name, file_name);
  glGenTextures(1, &drawable->texture);
  glBindTexture(GL_TEXTURE_2D, drawable->texture);
  new_rectangle(drawable);
  load_image(file, drawable);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void load_skin(char* name, cetris_skin_t* skin) {
  char *dir_name = malloc(100);
  format_str(dir_name, 100, "skins/%s", name);
  
  struct stat sb;

#ifdef _WIN32
  if (!(stat(dir_name, &sb) == 0 && (sb.st_mode & S_IFDIR)))
    dir_name = "skins/default";
#else
  if (!(stat(dir_name, &sb) == 0 && S_ISDIR(sb.st_mode)))
    dir_name = "skins/default";
#endif

  glActiveTexture(GL_TEXTURE0);

  load_element(&skin->block, "blocks.png", dir_name);
  load_element(&skin->overlay, "overlay.png", dir_name);
  load_element(&skin->background, "background.png", dir_name);
  load_element(&skin->playboard, "playboard.png", dir_name);
  load_element(&skin->border, "border.png", dir_name);

  char file[125];

  load_multiple_audio("clear", dir_name, 
      &skin->clear_sound_count, &skin->clear_sound);

  load_multiple_audio("four_clear", dir_name, 
      &skin->tetris_sound_count, &skin->tetris_sound);

  load_audio("lock", dir_name, &skin->lock_sound);
  load_audio("move_das", dir_name, &skin->move_sound);

}
