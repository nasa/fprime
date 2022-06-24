/*
 * ngtcp2
 *
 * Copyright (c) 2017 ngtcp2 contributors
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
#include "ngtcp2_ringbuf.h"

#include <assert.h>
#ifdef WIN32
#  include <intrin.h>
#endif

#include "ngtcp2_macro.h"

#if defined(_MSC_VER) && defined(_M_ARM64)
unsigned int __popcnt(unsigned int x) {
  unsigned int c = 0;
  for (; x; ++c) {
    x &= x - 1;
  }
  return c;
}
#endif

int ngtcp2_ringbuf_init(ngtcp2_ringbuf *rb, size_t nmemb, size_t size,
                        const ngtcp2_mem *mem) {
#ifdef WIN32
  assert(1 == __popcnt((unsigned int)nmemb));
#else
  assert(1 == __builtin_popcount((unsigned int)nmemb));
#endif

  rb->buf = ngtcp2_mem_malloc(mem, nmemb * size);
  if (rb->buf == NULL) {
    return NGTCP2_ERR_NOMEM;
  }

  rb->mem = mem;
  rb->nmemb = nmemb;
  rb->size = size;
  rb->first = 0;
  rb->len = 0;

  return 0;
}

void ngtcp2_ringbuf_free(ngtcp2_ringbuf *rb) {
  if (rb == NULL) {
    return;
  }

  ngtcp2_mem_free(rb->mem, rb->buf);
}

void *ngtcp2_ringbuf_push_front(ngtcp2_ringbuf *rb) {
  rb->first = (rb->first - 1) & (rb->nmemb - 1);
  rb->len = ngtcp2_min(rb->nmemb, rb->len + 1);

  return (void *)&rb->buf[rb->first * rb->size];
}

void *ngtcp2_ringbuf_push_back(ngtcp2_ringbuf *rb) {
  size_t offset = (rb->first + rb->len) & (rb->nmemb - 1);

  if (rb->len == rb->nmemb) {
    rb->first = (rb->first + 1) & (rb->nmemb - 1);
  } else {
    ++rb->len;
  }

  return (void *)&rb->buf[offset * rb->size];
}

void ngtcp2_ringbuf_pop_front(ngtcp2_ringbuf *rb) {
  rb->first = (rb->first + 1) & (rb->nmemb - 1);
  --rb->len;
}

void ngtcp2_ringbuf_pop_back(ngtcp2_ringbuf *rb) {
  assert(rb->len);
  --rb->len;
}

void ngtcp2_ringbuf_resize(ngtcp2_ringbuf *rb, size_t len) {
  assert(len <= rb->nmemb);
  rb->len = len;
}

void *ngtcp2_ringbuf_get(ngtcp2_ringbuf *rb, size_t offset) {
  assert(offset < rb->len);
  offset = (rb->first + offset) & (rb->nmemb - 1);
  return &rb->buf[offset * rb->size];
}

int ngtcp2_ringbuf_full(ngtcp2_ringbuf *rb) { return rb->len == rb->nmemb; }
