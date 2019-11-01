#include <glad/glad.h>

#include "drawable.h"
#include "skin.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
#define format_str sprintf_s
#else
#define format_str snprintf
#endif

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
  
  char file[125];
  format_str(file, 125, "%s/blocks.png", dir_name);
  if (load_image(file, skin->block_texture)) {
    skin->has_block_texture = true;
  } else skin->has_block_texture = false;
}
