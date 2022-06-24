/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
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
#include "nghttp3_idtr.h"

#include <assert.h>

int nghttp3_idtr_init(nghttp3_idtr *idtr, int server, const nghttp3_mem *mem) {
  int rv;

  rv = nghttp3_gaptr_init(&idtr->gap, mem);
  if (rv != 0) {
    return rv;
  }

  idtr->server = server;
  idtr->mem = mem;

  return 0;
}

void nghttp3_idtr_free(nghttp3_idtr *idtr) {
  if (idtr == NULL) {
    return;
  }

  nghttp3_gaptr_free(&idtr->gap);
}

/*
 * id_from_stream_id translates |stream_id| to id space used by
 * nghttp3_idtr.
 */
static uint64_t id_from_stream_id(int64_t stream_id) {
  return (uint64_t)(stream_id >> 2);
}

int nghttp3_idtr_open(nghttp3_idtr *idtr, int64_t stream_id) {
  uint64_t q;

  assert((idtr->server && (stream_id % 2)) ||
         (!idtr->server && (stream_id % 2)) == 0);

  q = id_from_stream_id(stream_id);

  if (nghttp3_gaptr_is_pushed(&idtr->gap, q, 1)) {
    return NGHTTP3_ERR_STREAM_IN_USE;
  }

  return nghttp3_gaptr_push(&idtr->gap, q, 1);
}

int nghttp3_idtr_is_open(nghttp3_idtr *idtr, int64_t stream_id) {
  uint64_t q;

  assert((idtr->server && (stream_id % 2)) ||
         (!idtr->server && (stream_id % 2)) == 0);

  q = id_from_stream_id(stream_id);

  return nghttp3_gaptr_is_pushed(&idtr->gap, q, 1);
}

uint64_t nghttp3_idtr_first_gap(nghttp3_idtr *idtr) {
  return nghttp3_gaptr_first_gap_offset(&idtr->gap);
}
