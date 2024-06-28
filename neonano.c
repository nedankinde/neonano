
#include "tui.h"
#include <terminal.h>

#define PROJECT_NAME "neonano"

int main(void) {
  term_init();
  term_raw_mode();
  tui_init();

  TextEditor text = tui_texteditor_new("Hello world!");
  text.element.x = 3;
  text.element.y = 3;

  tui_element_add(&window, (UIElement*) &text);

  for(;;) {
    term_clear();
    term_hide_cursor();

    size_t width = 0;
    size_t height = 0;

    term_size(&width, &height);

    term_draw_box_thin(1, 1, width, height, " Window ");

    tui_element_draw(&window);
    tui_element_update(&window);

    term_flush();

    KeyPressed result = term_read_input();

    if (result.tag == CTRL_C_KEY) {
      break;
    }

    tui_handle_input(result);
  }

  term_destroy();
  return 0;
}
