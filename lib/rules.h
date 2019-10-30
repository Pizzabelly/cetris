#ifndef RULES_H
#define RULES_H

#include <cetris.h>

cetris_config tetris_ds_config;
ctick tetris_worlds_levels[20];

bool twenty_line_sprint(cetris_game *g);
bool forty_line_sprint(cetris_game *g);

#endif /* RULES_H */
