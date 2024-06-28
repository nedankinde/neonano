#ifndef TUI_H_
#define TUI_H_

#include "terminal.h"
#include <common.h>
#include <buffer.h>

struct UIElement {
  size_t x;
  size_t y;
  size_t width;
  size_t height;

  struct UIElement **children;
  size_t children_size;
  size_t children_capacity;

  Buffer buffer;

  void (*draw)(void*);
  void (*update)(void*);
};

typedef struct UIElement (UIElement);

typedef struct {
  UIElement element;
  const char* text;
} TextElement;

typedef struct {
  UIElement element;
  char* text;
  size_t current_line;
} TextEditor;

extern UIElement window;

void tui_init(void);
void tui_element_add(UIElement *parent, UIElement* child);
void tui_element_init(UIElement *element);
void tui_element_destroy(UIElement *element);
void tui_element_draw(UIElement *element);
void tui_element_update(UIElement *element);
void tui_handle_input(KeyPressed key);

TextElement tui_text_new(const char *text);
TextEditor tui_texteditor_new(const char *text);

#endif // TUI_H_
