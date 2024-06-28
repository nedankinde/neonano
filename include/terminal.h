#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "buffer.h"
#include <common.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

enum KEY {
  LEFT_ARROW_KEY,
  RIGHT_ARROW_KEY,
  UP_ARROW_KEY,
  DOWN_ARROW_KEY,
  CHARACTER_KEY,
  CTRL_C_KEY,
  ENTER_KEY,
};

typedef struct {
  enum KEY tag;
  char ch;
} KeyPressed;

typedef struct {
  // Probably going to be the only time I would want a dynamic array so I will not implement one :^)
  const char *error_log[256];

  size_t width;
  size_t height;

  Buffer buffer;
} Terminal;

extern Terminal terminal;

void term_init(void);
void term_destroy(void);
void term_error(const char*);
void term_dump(void);
void term_write(const char*);
void term_flush(void);

// Helpers

void term_clear(void);
void term_background_color(uint8_t r, uint8_t g, uint8_t b);
void term_foreground_color(uint8_t r, uint8_t g, uint8_t b);
void term_draw_pixel(size_t x, size_t y, const char* character);
void term_draw_box(size_t x, size_t y, size_t width, size_t height, const char* title);
void term_draw_box_thin(size_t x, size_t y, size_t width, size_t height, const char* title);
void term_reset_colors(void);
void term_save_cursor_position(void);
void term_restore_cursor_position(void);
void term_hide_cursor(void);
void term_show_cursor(void);
void term_move_cursor(size_t x, size_t y);
void term_raw_mode(void);
void term_size(size_t *width, size_t *height);
KeyPressed term_read_input(void);

#endif // TERMINAL_H_
