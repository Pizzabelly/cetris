typedef struct {
  SDL_Color main;
  SDL_Color off;
  SDL_Color text;
} color_scheme_t;

color_scheme_t dark_mode = {
  .main = {90, 90, 90, 255},
  .off =  {70, 70, 70, 255},
  .text = {240, 240, 240, 255}
};

color_scheme_t light_mode = {
  .main = {255, 255, 255, 255},
  .off =  {235, 235, 235, 255},
  .text = {10, 10, 10, 255}
};

SDL_Color mino_colors[7] = {
  {253,253,150,255},  // Yellow
  {174,198,207,255},  // Aqua
  {255,105,97,255},   // Red
  {170,221,119,255},  // Olive
  {255,179,71,255},   // Orange
  {119,158,203,255},  // Navy
  {177,156,217,255}   // Purple   
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

typedef struct {
  SDL_Texture *main;
  SDL_Texture *queue;
  SDL_Texture *hold;
} solo_game;

