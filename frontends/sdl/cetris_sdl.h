typedef struct {
  SDL_Color main;
  SDL_Color off;
  SDL_Color text;
} color_scheme_t;

color_scheme_t dark_mode = {
  .main = (SDL_Color){100, 100, 100, 255},
  .off =  (SDL_Color){50, 50, 50, 255},
  .text = (SDL_Color){240, 240, 240, 255}
};

color_scheme_t light_mode = {
  .main = (SDL_Color){255, 255, 255, 255},
  .off =  (SDL_Color){235, 235, 235, 255},
  .text = (SDL_Color){10, 10, 10, 255}
};

SDL_Color mino_colors[7] = {
  (SDL_Color){253,253,150,255},  // Yellow
  (SDL_Color){174,198,207,255},  // Aqua
  (SDL_Color){255,105,97,255},   // Red
  (SDL_Color){170,221,119,255},  // Olive
  (SDL_Color){255,179,71,255},   // Orange
  (SDL_Color){119,158,203,255},  // Navy
  (SDL_Color){177,156,217,255}   // Purple   
};

typedef struct {
  SDL_AudioSpec wav_spec;
  uint32_t wav_length;
  uint8_t *wav_buffer;
} audio_clip_t;

typedef struct {
  TTF_Font *font;
  int size;
} font_t;

typedef struct {
  cetris_game* game;
  color_scheme_t scheme;
  float count_down;
} game_board_t;
