/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
 * Copyright (c) 2013 nghttp2 contributors
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
#ifndef NGHTTP3_FRAME_H
#define NGHTTP3_FRAME_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <nghttp3/nghttp3.h>

#include "nghttp3_buf.h"

typedef enum nghttp3_frame_type {
  NGHTTP3_FRAME_DATA = 0x00,
  NGHTTP3_FRAME_HEADERS = 0x01,
  NGHTTP3_FRAME_CANCEL_PUSH = 0x03,
  NGHTTP3_FRAME_SETTINGS = 0x04,
  NGHTTP3_FRAME_PUSH_PROMISE = 0x05,
  NGHTTP3_FRAME_GOAWAY = 0x07,
  NGHTTP3_FRAME_MAX_PUSH_ID = 0x0d,
} nghttp3_frame_type;

typedef enum nghttp3_h2_reserved_type {
  NGHTTP3_H2_FRAME_PRIORITY = 0x02,
  NGHTTP3_H2_FRAME_PING = 0x06,
  NGHTTP3_H2_FRAME_WINDOW_UPDATE = 0x08,
  NGHTTP3_H2_FRAME_CONTINUATION = 0x9,
} nghttp3_h2_reserved_type;

typedef struct nghttp3_frame_hd {
  int64_t type;
  int64_t length;
} nghttp3_frame_hd;

typedef struct nghttp3_frame_data {
  nghttp3_frame_hd hd;
} nghttp3_frame_data;

typedef struct nghttp3_frame_headers {
  nghttp3_frame_hd hd;
  nghttp3_nv *nva;
  size_t nvlen;
} nghttp3_frame_headers;

typedef struct nghttp3_frame_cancel_push {
  nghttp3_frame_hd hd;
  int64_t push_id;
} nghttp3_frame_cancel_push;

#define NGHTTP3_SETTINGS_ID_MAX_FIELD_SECTION_SIZE 0x06
#define NGHTTP3_SETTINGS_ID_QPACK_MAX_TABLE_CAPACITY 0x01
#define NGHTTP3_SETTINGS_ID_QPACK_BLOCKED_STREAMS 0x07

#define NGHTTP3_H2_SETTINGS_ID_ENABLE_PUSH 0x2
#define NGHTTP3_H2_SETTINGS_ID_MAX_CONCURRENT_STREAMS 0x3
#define NGHTTP3_H2_SETTINGS_ID_INITIAL_WINDOW_SIZE 0x4
#define NGHTTP3_H2_SETTINGS_ID_MAX_FRAME_SIZE 0x5

typedef struct nghttp3_settings_entry {
  uint64_t id;
  uint64_t value;
} nghttp3_settings_entry;

typedef struct nghttp3_frame_settings {
  nghttp3_frame_hd hd;
  size_t niv;
  nghttp3_settings_entry iv[1];
} nghttp3_frame_settings;

typedef struct nghttp3_frame_push_promise {
  nghttp3_frame_hd hd;
  nghttp3_nv *nva;
  size_t nvlen;
  int64_t push_id;
} nghttp3_frame_push_promise;

typedef struct nghttp3_frame_goaway {
  nghttp3_frame_hd hd;
  int64_t id;
} nghttp3_frame_goaway;

typedef struct nghttp3_frame_max_push_id {
  nghttp3_frame_hd hd;
  int64_t push_id;
} nghttp3_frame_max_push_id;

typedef union nghttp3_frame {
  nghttp3_frame_hd hd;
  nghttp3_frame_data data;
  nghttp3_frame_headers headers;
  nghttp3_frame_cancel_push cancel_push;
  nghttp3_frame_settings settings;
  nghttp3_frame_push_promise push_promise;
  nghttp3_frame_goaway goaway;
  nghttp3_frame_max_push_id max_push_id;
} nghttp3_frame;

/*
 * nghttp3_frame_write_hd writes frame header |hd| to |dest|.  This
 * function assumes that |dest| has enough space to write |hd|.
 *
 * This function returns |dest| plus the number of bytes written.
 */
uint8_t *nghttp3_frame_write_hd(uint8_t *dest, const nghttp3_frame_hd *hd);

/*
 * nghttp3_frame_write_hd_len returns the number of bytes required to
 * write |hd|.  hd->length must be set.
 */
size_t nghttp3_frame_write_hd_len(const nghttp3_frame_hd *hd);

/*
 * nghttp3_frame_write_settings writes SETTINGS frame |fr| to |dest|.
 * This function assumes that |dest| has enough space to write |fr|.
 *
 * This function returns |dest| plus the number of bytes written.
 */
uint8_t *nghttp3_frame_write_settings(uint8_t *dest,
                                      const nghttp3_frame_settings *fr);

/*
 * nghttp3_frame_write_settings_len returns the number of bytes
 * required to write |fr|.  fr->hd.length is ignored.  This function
 * stores payload length in |*ppayloadlen|.
 */
size_t nghttp3_frame_write_settings_len(int64_t *pppayloadlen,
                                        const nghttp3_frame_settings *fr);

/*
 * nghttp3_frame_write_cancel_push writes CANCEL_PUSH frame |fr| to
 * |dest|.  This function assumes that |dest| has enough space to
 * write |fr|.
 *
 * This function returns |dest| plus the number of bytes written.
 */
uint8_t *nghttp3_frame_write_cancel_push(uint8_t *dest,
                                         const nghttp3_frame_cancel_push *fr);

/*
 * nghttp3_frame_write_cancel_push_len returns the number of bytes
 * required to write |fr|.  fr->hd.length is ignored.  This function
 * stores payload length in |*ppayloadlen|.
 */
size_t nghttp3_frame_write_cancel_push_len(int64_t *ppayloadlen,
                                           const nghttp3_frame_cancel_push *fr);

/*
 * nghttp3_frame_write_max_push_id writes MAX_PUSH_ID frame |fr| to
 * |dest|.  This function assumes that |dest| has enough space to
 * write |fr|.
 *
 * This function returns |dest| plus the number of bytes written.
 */
uint8_t *nghttp3_frame_write_max_push_id(uint8_t *dest,
                                         const nghttp3_frame_max_push_id *fr);

/*
 * nghttp3_frame_write_max_push_id_len returns the number of bytes
 * required to write |fr|.  fr->hd.length is ignored.  This function
 * stores payload length in |*ppayloadlen|.
 */
size_t nghttp3_frame_write_max_push_id_len(int64_t *ppayloadlen,
                                           const nghttp3_frame_max_push_id *fr);

/*
 * nghttp3_frame_write_goaway writes GOAWAY frame |fr| to |dest|.
 * This function assumes that |dest| has enough space to write |fr|.
 *
 * This function returns |dest| plus the number of bytes written.
 */
uint8_t *nghttp3_frame_write_goaway(uint8_t *dest,
                                    const nghttp3_frame_goaway *fr);

/*
 * nghttp3_frame_write_goaway_len returns the number of bytes required
 * to write |fr|.  fr->hd.length is ignored.  This function stores
 * payload length in |*ppayloadlen|.
 */
size_t nghttp3_frame_write_goaway_len(int64_t *ppayloadlen,
                                      const nghttp3_frame_goaway *fr);

/*
 * nghttp3_nva_copy copies name/value pairs from |nva|, which contains
 * |nvlen| pairs, to |*nva_ptr|, which is dynamically allocated so
 * that all items can be stored.  The resultant name and value in
 * nghttp2_nv are guaranteed to be NULL-terminated even if the input
 * is not null-terminated.
 *
 * The |*pnva| must be freed using nghttp3_nva_del().
 *
 * This function returns 0 if it succeeds or one of the following
 * negative error codes:
 *
 * NGHTTP3_ERR_NOMEM
 *     Out of memory.
 */
int nghttp3_nva_copy(nghttp3_nv **pnva, const nghttp3_nv *nva, size_t nvlen,
                     const nghttp3_mem *mem);

/*
 * nghttp3_nva_del frees |nva|.
 */
void nghttp3_nva_del(nghttp3_nv *nva, const nghttp3_mem *mem);

/*
 * nghttp3_frame_headers_free frees memory allocated for |fr|.  It
 * assumes that fr->nva is created by nghttp3_nva_copy() or NULL.
 */
void nghttp3_frame_headers_free(nghttp3_frame_headers *fr,
                                const nghttp3_mem *mem);

/*
 * nghttp3_frame_push_promise_free frees memory allocated for |fr|.
 * It assumes that fr->nva is created by nghttp3_nva_copy() or NULL.
 */
void nghttp3_frame_push_promise_free(nghttp3_frame_push_promise *fr,
                                     const nghttp3_mem *mem);

#endif /* NGHTTP3_FRAME_H */
