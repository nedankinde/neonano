#ifndef BUFFER_H_
#define BUFFER_H_

#include <common.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} Buffer;

static void buffer_init(Buffer *buf, size_t initial_capacity) {
  buf->buffer = (char *)malloc(initial_capacity * sizeof(char));
  if (buf->buffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  buf->length = 0;
  buf->capacity = initial_capacity;
}

static void buffer_append(Buffer *buf, const char *data) {
  size_t data_len = strlen(data);
  size_t new_length = buf->length + data_len;

  if (new_length >= buf->capacity) {
    buf->capacity = new_length + 1;
    buf->buffer = (char *)realloc(buf->buffer, buf->capacity * sizeof(char));
    if (buf->buffer == NULL) {
      fprintf(stderr, "Memory reallocation failed\n");
      exit(1);
    }
  }
  strcpy(buf->buffer + buf->length, data);
  buf->length = new_length;
}

static void buffer_destroy(Buffer *buf) {
  free(buf->buffer);
  buf->buffer = NULL;
  buf->length = 0;
  buf->capacity = 0;
}

#endif // BUFFER_H_
