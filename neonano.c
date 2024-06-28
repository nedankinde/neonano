
#include <terminal.h>

#define PROJECT_NAME "neonano"

int main(void) {
  term_init();
  term_raw_mode();

  const char* menu_list[3];
  menu_list[0] = "Start Program";
  menu_list[1] = "Options";
  menu_list[2] = "Leave";

  size_t selected = 0;

  size_t x = 0;
  while (1) {
    term_clear();
    term_hide_cursor();

    size_t width = 0;
    size_t height = 0;

    term_size(&width, &height);

    term_background_color(0, 100, 255);
    term_foreground_color(255, 255, 255);

    for (size_t i = 1; i < (x % width + 2); ++i) {
      term_draw_pixel(i + 5, 1, " ");
    }
   
    term_draw_pixel(1, 1, "[ Loading ]");

    x += 3;

    term_reset_colors();

    term_foreground_color(0, 255, 0);

    term_draw_box(1, 2, width, height, " NeoNano ");

    for (size_t i = 0; i < 3; ++i) {
      size_t x = (width / 2) - (strlen(menu_list[i]) / 2) + 1;
      size_t y = (10 + (i * 2));

      if (i == selected) {
        term_foreground_color(255, 255, 255);
        term_draw_pixel(x - 2, y, "▶");
      } else {
        term_foreground_color(0, 255, 0);
      }

      term_draw_pixel((width / 2) - (strlen(menu_list[i]) / 2) + 1, 10 + (i * 2), menu_list[i]);
    }
   
    term_foreground_color(255, 255, 255);
    term_draw_box_thin(((width / 2) - 15) + 1, ((height / 2) - 6), 30, 12, " Settings ");

    term_flush();

    char c = '\0';
    read(STDIN_FILENO, &c, 1);

    if (c == ',') {
      selected -= 1;
    }
    if (c == 'o') {
      selected += 1;
    }
    if (c == 'q') {
      break;
    }
  }

  term_destroy();
  return 0;
}
