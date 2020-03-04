/**
 * Copyright (c) 2012-2016 Moodstocks SAS
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "jpgenc_buf.h"
#include "jpgenc_conf.h"

#define JPEC_BUFFER_INIT_SIZ 65536

#ifndef CONFIG_JPGENC_HEAP_MEM
static jpec_buffer_t buf_b;
static uint8_t buf_stream[JPEC_BUFFER_INIT_SIZ];
#endif

jpec_buffer_t *jpec_buffer_new(void) {
  return jpec_buffer_new2(-1);
}

jpec_buffer_t *jpec_buffer_new2(int siz) {
  if (siz < 0) siz = 0;
#ifdef CONFIG_JPGENC_HEAP_MEM
  jpec_buffer_t *b = (jpec_buffer_t *)malloc(sizeof(*b));
  b->stream = siz > 0 ? (uint8_t *)malloc(siz) : NULL;
#else
  jpec_buffer_t *b = &buf_b;
  b->stream = siz > 0 ? buf_stream : NULL;
#endif
  b->siz = siz;
  b->len = 0;
  return b;
}

void jpec_buffer_del(jpec_buffer_t *b) {
  assert(b);
#ifdef CONFIG_JPGENC_HEAP_MEM
  if (b->stream) free(b->stream);
  free(b);
#endif
}

void jpec_buffer_write_byte(jpec_buffer_t *b, int val) {
  assert(b);
  if (b->siz == b->len) {
    int nsiz = (b->siz > 0) ? 2 * b->siz : JPEC_BUFFER_INIT_SIZ;
#ifdef CONFIG_JPGENC_HEAP_MEM
    void* tmp = realloc(b->stream, nsiz);
#else
    void* tmp = (void *)buf_stream;
#endif
    b->stream = (uint8_t *) tmp;
    b->siz = nsiz;
  }
  b->stream[b->len++] = (uint8_t) val;
}

void jpec_buffer_write_2bytes(jpec_buffer_t *b, int val) {
  assert(b);
  jpec_buffer_write_byte(b, (val >> 8) & 0xFF);
  jpec_buffer_write_byte(b, val & 0xFF);  
}
