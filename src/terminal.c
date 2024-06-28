#include "terminal.h"
#include <stdint.h>

Terminal terminal;

static struct termios termios_backup;

void term_init(void) {
  if (tcgetattr(STDIN_FILENO, &termios_backup) == -1) {
    term_error("tcgetattr");
    exit(-1);
  }
 
  buffer_init(&terminal.buffer, 256);
}

void term_destroy(void) {
  term_clear();
  term_show_cursor();
  term_flush();

  term_dump();
  buffer_destroy(&terminal.buffer);

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_backup) == -1) {
    term_error("tcsetattr");
    exit(-1);
  }
}

void term_size(size_t *width, size_t *height) {
  struct winsize size;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
    exit(-1);
  }

  *width = (size_t) size.ws_col;
  *height = (size_t) size.ws_row;
}

void term_raw_mode(void) {
  struct termios raw = termios_backup;
  atexit(term_destroy);

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    term_error("tcsetattr");
    exit(-1);
  }

  term_hide_cursor();
}

static size_t error_count = 0;
void term_error(const char *message) {
  if (error_count < 256) {
    terminal.error_log[error_count++] = message;
  } else {
    printf("Too many errors... quitting!\n");
    exit(-1);
  }
}

void term_dump(void) {
  size_t i = 0;
  while(terminal.error_log[i++] != NULL && (i < 256)) {
    printf("%s\n", terminal.error_log[i - 1]);
  }
}

void term_write(const char *text) {
  buffer_append(&terminal.buffer, text);
}

void term_flush(void) {
  term_write("\r");
  write(STDOUT_FILENO, terminal.buffer.buffer, terminal.buffer.length);
  buffer_destroy(&terminal.buffer);
  buffer_init(&terminal.buffer, 256);
}

void term_clear(void) {
  term_write("\x1b[H");
  term_write("\033c");
  term_write("\x1b[2J");
}

void term_background_color(uint8_t r, uint8_t g, uint8_t b) {
  char escape_sequence[32];
  snprintf(escape_sequence, sizeof(escape_sequence), "\x1b[48;2;%i;%i;%im", r, g, b);
  term_write(escape_sequence);
}

void term_foreground_color(uint8_t r, uint8_t g, uint8_t b) {
  char escape_sequence[32];
  snprintf(escape_sequence, sizeof(escape_sequence), "\x1b[38;2;%i;%i;%im", r, g, b);
  term_write(escape_sequence);
}

void term_reset_colors(void) {
  term_write("\x1b[0m");
}

void term_hide_cursor(void) {
  term_write("\x1b[?25l");
}

void term_show_cursor(void) {
  term_write("\x1b[?25H");
}

void term_move_cursor(size_t x, size_t y) {
  char escape_sequence[32];
  snprintf(escape_sequence, sizeof(escape_sequence), "\x1b[%zu;%zuH", y, x);
  term_write(escape_sequence);
}

void term_save_cursor_position(void) {
  term_write("\x1b[s");
}

void term_restore_cursor_position(void) {
  term_write("\x1b[u");
}

void term_draw_pixel(size_t x, size_t y, const char *character) {
  term_save_cursor_position();
  term_move_cursor(x, y);
  term_write(character);
  term_restore_cursor_position();
}

void term_draw_box(size_t x, size_t y, size_t width, size_t height, const char *title) {
  for (size_t i = x; i < x + width; ++i) {
    term_draw_pixel(i, y, "═");                // Top side
    term_draw_pixel(i, y + height - 1, "═");   // Bottom side
  }

  for (size_t j = y; j < y + height; ++j) {
    term_draw_pixel(x, j, "║");                // Left side
    term_draw_pixel(x + width - 1, j, "║");    // Right side
  }

  // Corners
  term_draw_pixel(x, y, "╔");                         // Top left corner
  term_draw_pixel(x + width - 1, y, "╗");             // Top right corner
  term_draw_pixel(x, y + height - 1, "╚");            // Bottom left corner
  term_draw_pixel(x + width - 1, y + height - 1, "╝"); // Bottom right corner

  term_draw_pixel(x + 3, y, title);
}


void term_draw_box_thin(size_t x, size_t y, size_t width, size_t height, const char *title) {
  for (size_t i = x; i < x + width; ++i) {
    term_draw_pixel(i, y, "─");                // Top side
    term_draw_pixel(i, y + height - 1, "─");   // Bottom side
  }

  for (size_t j = y; j < y + height; ++j) {
    term_draw_pixel(x, j, "│");                // Left side
    term_draw_pixel(x + width - 1, j, "│");    // Right side
  }

  // Corners
  term_draw_pixel(x, y, "╭");                         // Top left corner
  term_draw_pixel(x + width - 1, y, "╮");             // Top right corner
  term_draw_pixel(x, y + height - 1, "╰");            // Bottom left corner
  term_draw_pixel(x + width - 1, y + height - 1, "╯"); // Bottom right corner

  term_draw_pixel(x + 3, y, title);
}


KeyPressed term_read_input(void) {
  KeyPressed result;
  result.tag = -1;
  result.ch = '\0';
 
  char key;

  read(STDOUT_FILENO, &key, 1);

  if (key == '\r') {
    result.tag = ENTER_KEY;
  }

  if (key == '\x1b') {
    char seq[3];
   
    while (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
    while (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

    if (seq[0] == '[') {
      switch (seq[1]) {
        case 'A':
          result.tag = UP_ARROW_KEY;
          break;
        case 'B':
          result.tag = DOWN_ARROW_KEY;
          break;
        case 'C':
          result.tag = LEFT_ARROW_KEY;
          break;
        case 'D':
          result.tag = RIGHT_ARROW_KEY;
          break;
        default:
          break;
      }
    }
  } else if (key == 3) { // Ctrl+C (ASCII value 3)
    result.tag = CTRL_C_KEY;
  }
 
  return result;
}
