typedef struct {
  SDL_Color main;
  SDL_Color off;
  SDL_Color text;
} color_scheme_t;

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
