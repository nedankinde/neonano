#include "tui.h"
#include "terminal.h"
#include <stdio.h>

UIElement window;

#define FunPtr void (*)(void*)

void _tui_window_draw(UIElement *self) {
  for (size_t i = 0; i < self->children_size; ++i) {
    tui_element_draw(self->children[i]);
  }
}

void _tui_window_update(UIElement *self) {
  for (size_t i = 0; i < self->children_size; ++i) {
    tui_element_update(self->children[i]);
  }
}

void tui_init(void) {
  tui_element_init(&window);
  window.draw = (FunPtr) _tui_window_draw;
  window.update = (FunPtr) _tui_window_update;
}

void tui_destroy(void) {
 
}

KeyPressed last_keypressed;

void tui_handle_input(KeyPressed key) {
  last_keypressed.tag = key.tag;
  last_keypressed.ch = key.ch;
}

void tui_element_add(UIElement *parent, UIElement *child) {
  if (parent->children == NULL) {
    parent->children = malloc(sizeof(UIElement) * parent->children_capacity);
    parent->children[0] = child;
  } else {
    if (parent->children_capacity * 0.7 < parent->children_size) {
      parent->children = realloc(parent->children, sizeof(UIElement) * (parent->children_capacity * 2));
    }
    parent->children[parent->children_size] = child;
  }
  parent->children_size++;
}

void tui_element_init(UIElement *element) {
  element->x = 1;
  element->y = 1;
  element->width = 0;
  element->height = 0;
  element->children = NULL;
  element->children_capacity = 4;
  element->children_size = 0;

  element->draw = NULL;
  element->update = NULL;
  buffer_init(&element->buffer, 80);
}

void tui_element_destroy(UIElement *element) {
  buffer_destroy(&element->buffer);
}

void tui_element_draw(UIElement *element) {
  if (element->draw != NULL) {
    element->draw(element);
  }
}

void tui_element_update(UIElement *element) {
  if (element->update != NULL) {
    element->update(element);
  }
}

void _tui_text_draw(TextElement *self) {
  term_draw_pixel(self->element.x, self->element.y, self->text);
}

void _tui_text_update(TextElement *self) {

}

TextElement tui_text_new(const char *text) {
  TextElement text_element;
  text_element.text = text;

  tui_element_init(&text_element.element);
  UIElement* result = (UIElement*) &text_element;

  result->draw = (FunPtr) _tui_text_draw;
  result->update = (FunPtr) _tui_text_update;

  return text_element;
}

void _tui_texteditor_draw(TextEditor *self) {
  term_draw_pixel(self->element.x, self->element.y, self->text);
}

void _tui_texteditor_update(TextEditor *self) {
  if (last_keypressed.tag == ENTER_KEY) {
    strcat(self->text, "!");
  }
}

TextEditor tui_texteditor_new(const char *text) {
  TextEditor text_element;
  text_element.text = malloc(sizeof(char) * strlen(text));
  strcat(text_element.text, text);

  tui_element_init(&text_element.element);
  UIElement* result = (UIElement*) &text_element;

  result->draw = (FunPtr) _tui_texteditor_draw;
  result->update = (FunPtr) _tui_texteditor_update;

  return text_element;
}
