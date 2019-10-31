typedef struct {
  SDL_Color main;
  SDL_Color off;
  SDL_Color text;
} color_scheme_t;

color_scheme_t dark_mode = {
  .main = {40, 40, 40, 255},
  .off =  {90, 90, 90, 255},
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
  bool random_audio;

  int clear_sound_count;
  audio_clip_t *clear_sound;

  int tetris_sound_count;
  audio_clip_t *tetris_sound;
  audio_clip_t lock_sound;
 
  bool custom_block;
  SDL_Texture *blocks;

  bool image_background;
  SDL_Texture *game_background;

  bool custom_border;
  SDL_Texture *border;
  
  bool custom_background;
  SDL_Texture *background;

} cetris_skin_t;

typedef struct {
  cetris_game game;
  color_scheme_t scheme;
  cetris_config conf;
  float count_down;
} game_board_t;

typedef struct {
  SDL_Texture *main;
  SDL_Texture *queue;
  SDL_Texture *hold;

  game_board_t game_board;
} solo_game_t;

typedef struct {
  int key_down;
  int key_right;
  int key_left;
  int key_rotate_cw;
  int key_rotate_ccw;
  int key_drop;
  int key_hold;
  int key_restart;
} key_bindings_t;

typedef struct {
  key_bindings_t keys;
  char *skin_name;
} config_t;

enum {
  SOLO
};

typedef struct {
  int current_game;
  solo_game_t solo;
  color_scheme_t colors;

  SDL_Renderer* render;
  SDL_Window *window;
  SDL_Surface *screen;

  int font_count;
  font_t fonts[10];

  SDL_AudioDeviceID audio_device;
  
  config_t config;

  cetris_skin_t skin;
} cetris_ui;
