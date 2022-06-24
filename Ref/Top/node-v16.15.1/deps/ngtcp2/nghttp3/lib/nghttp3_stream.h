/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
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
#ifndef NGHTTP3_STREAM_H
#define NGHTTP3_STREAM_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <nghttp3/nghttp3.h>

#include "nghttp3_map.h"
#include "nghttp3_tnode.h"
#include "nghttp3_ringbuf.h"
#include "nghttp3_buf.h"
#include "nghttp3_frame.h"
#include "nghttp3_qpack.h"

#define NGHTTP3_STREAM_MIN_CHUNK_SIZE 256

/* NGHTTP3_MIN_UNSENT_BYTES is the minimum unsent bytes which is large
   enough to fill outgoing single QUIC packet. */
#define NGHTTP3_MIN_UNSENT_BYTES 4096

/* NGHTTP3_STREAM_MIN_WRITELEN is the minimum length of write to cause
   the stream to reschedule. */
#define NGHTTP3_STREAM_MIN_WRITELEN 800

/* nghttp3_stream_type is unidirectional stream type. */
typedef enum nghttp3_stream_type {
  NGHTTP3_STREAM_TYPE_CONTROL = 0x00,
  NGHTTP3_STREAM_TYPE_PUSH = 0x01,
  NGHTTP3_STREAM_TYPE_QPACK_ENCODER = 0x02,
  NGHTTP3_STREAM_TYPE_QPACK_DECODER = 0x03,
  NGHTTP3_STREAM_TYPE_UNKNOWN = UINT64_MAX,
} nghttp3_stream_type;

typedef enum nghttp3_ctrl_stream_state {
  NGHTTP3_CTRL_STREAM_STATE_FRAME_TYPE,
  NGHTTP3_CTRL_STREAM_STATE_FRAME_LENGTH,
  NGHTTP3_CTRL_STREAM_STATE_CANCEL_PUSH,
  NGHTTP3_CTRL_STREAM_STATE_SETTINGS,
  NGHTTP3_CTRL_STREAM_STATE_GOAWAY,
  NGHTTP3_CTRL_STREAM_STATE_MAX_PUSH_ID,
  NGHTTP3_CTRL_STREAM_STATE_IGN_FRAME,
  NGHTTP3_CTRL_STREAM_STATE_SETTINGS_ID,
  NGHTTP3_CTRL_STREAM_STATE_SETTINGS_VALUE,
} nghttp3_ctrl_stream_state;

typedef enum nghttp3_req_stream_state {
  NGHTTP3_REQ_STREAM_STATE_FRAME_TYPE,
  NGHTTP3_REQ_STREAM_STATE_FRAME_LENGTH,
  NGHTTP3_REQ_STREAM_STATE_DATA,
  NGHTTP3_REQ_STREAM_STATE_HEADERS,
  NGHTTP3_REQ_STREAM_STATE_PUSH_PROMISE_PUSH_ID,
  NGHTTP3_REQ_STREAM_STATE_PUSH_PROMISE,
  NGHTTP3_REQ_STREAM_STATE_IGN_PUSH_PROMISE,
  NGHTTP3_REQ_STREAM_STATE_IGN_FRAME,
  NGHTTP3_REQ_STREAM_STATE_IGN_REST,
} nghttp3_req_stream_state;

typedef enum nghttp3_push_stream_state {
  NGHTTP3_PUSH_STREAM_STATE_FRAME_TYPE,
  NGHTTP3_PUSH_STREAM_STATE_FRAME_LENGTH,
  NGHTTP3_PUSH_STREAM_STATE_DATA,
  NGHTTP3_PUSH_STREAM_STATE_HEADERS,
  NGHTTP3_PUSH_STREAM_STATE_IGN_FRAME,
  NGHTTP3_PUSH_STREAM_STATE_PUSH_ID,
  NGHTTP3_PUSH_STREAM_STATE_IGN_REST,
} nghttp3_push_stream_state;

typedef struct nghttp3_varint_read_state {
  int64_t acc;
  size_t left;
} nghttp3_varint_read_state;

typedef struct nghttp3_stream_read_state {
  nghttp3_varint_read_state rvint;
  nghttp3_frame fr;
  int state;
  int64_t left;
} nghttp3_stream_read_state;

/* NGHTTP3_STREAM_FLAG_NONE indicates that no flag is set. */
#define NGHTTP3_STREAM_FLAG_NONE 0x0000
/* NGHTTP3_STREAM_FLAG_TYPE_IDENTIFIED is set when a unidirectional
   stream type is identified. */
#define NGHTTP3_STREAM_FLAG_TYPE_IDENTIFIED 0x0001
/* NGHTTP3_STREAM_FLAG_FC_BLOCKED indicates that stream is blocked by
   QUIC flow control. */
#define NGHTTP3_STREAM_FLAG_FC_BLOCKED 0x0002
/* NGHTTP3_STREAM_FLAG_READ_DATA_BLOCKED indicates that application is
   temporarily unable to provide data. */
#define NGHTTP3_STREAM_FLAG_READ_DATA_BLOCKED 0x0004
/* NGHTTP3_STREAM_FLAG_WRITE_END_STREAM indicates that application
   finished to feed outgoing data. */
#define NGHTTP3_STREAM_FLAG_WRITE_END_STREAM 0x0008
/* NGHTTP3_STREAM_FLAG_QPACK_DECODE_BLOCKED indicates that stream is
   blocked due to QPACK decoding. */
#define NGHTTP3_STREAM_FLAG_QPACK_DECODE_BLOCKED 0x0010
/* NGHTTP3_STREAM_FLAG_READ_EOF indicates that remote endpoint sent
   fin. */
#define NGHTTP3_STREAM_FLAG_READ_EOF 0x0020
/* NGHTTP3_STREAM_FLAG_CLOSED indicates that QUIC stream was closed.
   nghttp3_stream object can still alive because it might be blocked
   by QPACK decoder. */
#define NGHTTP3_STREAM_FLAG_CLOSED 0x0040
/* NGHTTP3_STREAM_FLAG_PUSH_PROMISE_BLOCKED indicates that stream is
   blocked because the corresponding PUSH_PROMISE has not been
   received yet. */
#define NGHTTP3_STREAM_FLAG_PUSH_PROMISE_BLOCKED 0x0080
/* NGHTTP3_STREAM_FLAG_SHUT_WR indicates that any further write
   operation to a stream is prohibited. */
#define NGHTTP3_STREAM_FLAG_SHUT_WR 0x0100
/* NGHTTP3_STREAM_FLAG_RESET indicates that stream is reset. */
#define NGHTTP3_STREAM_FLAG_RESET 0x0200

typedef enum nghttp3_stream_http_state {
  NGHTTP3_HTTP_STATE_NONE,
  NGHTTP3_HTTP_STATE_REQ_INITIAL,
  NGHTTP3_HTTP_STATE_REQ_BEGIN,
  NGHTTP3_HTTP_STATE_REQ_HEADERS_BEGIN,
  NGHTTP3_HTTP_STATE_REQ_HEADERS_END,
  NGHTTP3_HTTP_STATE_REQ_DATA_BEGIN,
  NGHTTP3_HTTP_STATE_REQ_DATA_END,
  NGHTTP3_HTTP_STATE_REQ_TRAILERS_BEGIN,
  NGHTTP3_HTTP_STATE_REQ_TRAILERS_END,
  NGHTTP3_HTTP_STATE_REQ_END,
  NGHTTP3_HTTP_STATE_RESP_INITIAL,
  NGHTTP3_HTTP_STATE_RESP_BEGIN,
  NGHTTP3_HTTP_STATE_RESP_HEADERS_BEGIN,
  NGHTTP3_HTTP_STATE_RESP_HEADERS_END,
  NGHTTP3_HTTP_STATE_RESP_DATA_BEGIN,
  NGHTTP3_HTTP_STATE_RESP_DATA_END,
  NGHTTP3_HTTP_STATE_RESP_TRAILERS_BEGIN,
  NGHTTP3_HTTP_STATE_RESP_TRAILERS_END,
  NGHTTP3_HTTP_STATE_RESP_END,
} nghttp3_stream_http_state;

typedef enum nghttp3_stream_http_event {
  NGHTTP3_HTTP_EVENT_DATA_BEGIN,
  NGHTTP3_HTTP_EVENT_DATA_END,
  NGHTTP3_HTTP_EVENT_HEADERS_BEGIN,
  NGHTTP3_HTTP_EVENT_HEADERS_END,
  NGHTTP3_HTTP_EVENT_PUSH_PROMISE_BEGIN,
  NGHTTP3_HTTP_EVENT_PUSH_PROMISE_END,
  NGHTTP3_HTTP_EVENT_MSG_END,
} nghttp3_stream_http_event;

typedef struct nghttp3_stream nghttp3_stream;

typedef struct nghttp3_push_promise nghttp3_push_promise;

/*
 * nghttp3_stream_acked_data is a callback function which is invoked
 * when data sent on stream denoted by |stream_id| supplied from
 * application is acknowledged by remote endpoint.  The number of
 * bytes acknowledged is given in |datalen|.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning NGHTTP3_ERR_CALLBACK_FAILURE will return to the caller
 * immediately.  Any values other than 0 is treated as
 * NGHTTP3_ERR_CALLBACK_FAILURE.
 */
typedef int (*nghttp3_stream_acked_data)(nghttp3_stream *stream,
                                         int64_t stream_id, size_t datalen,
                                         void *user_data);

typedef struct nghttp3_stream_callbacks {
  nghttp3_stream_acked_data acked_data;
} nghttp3_stream_callbacks;

typedef struct nghttp3_http_state {
  /* status_code is HTTP status code received.  This field is used
     if connection is initialized as client. */
  int32_t status_code;
  /* content_length is the value of received content-length header
     field. */
  int64_t content_length;
  /* recv_content_length is the number of body bytes received so
     far. */
  int64_t recv_content_length;
  uint16_t flags;
  /* pri is a stream priority produced by nghttp3_pri_to_uint8. */
  uint8_t pri;
} nghttp3_http_state;

struct nghttp3_stream {
  const nghttp3_mem *mem;
  nghttp3_map_entry me;
  /* node is a node in dependency tree.  For server initiated
     unidirectional stream (push), scheduling is done via
     corresponding nghttp3_push_promise object pointed by pp. */
  nghttp3_tnode node;
  nghttp3_pq_entry qpack_blocked_pe;
  nghttp3_stream_callbacks callbacks;
  nghttp3_ringbuf frq;
  nghttp3_ringbuf chunks;
  nghttp3_ringbuf outq;
  /* inq stores the stream raw data which cannot be read because
     stream is blocked by QPACK decoder. */
  nghttp3_ringbuf inq;
  nghttp3_qpack_stream_context qpack_sctx;
  /* conn is a reference to underlying connection.  It could be NULL
     if stream is not a request/push stream. */
  nghttp3_conn *conn;
  void *user_data;
  /* unsent_bytes is the number of bytes in outq not written yet */
  size_t unsent_bytes;
  /* outq_idx is an index into outq where next write is made. */
  size_t outq_idx;
  /* outq_offset is write offset relative to the element at outq_idx
     in outq. */
  size_t outq_offset;
  /* ack_offset is offset acknowledged by peer relative to the first
     element in outq. */
  uint64_t ack_offset;
  /* ack_done is the number of bytes notified to an application that
     they are acknowledged inside the first outq element if it is of
     type NGHTTP3_BUF_TYPE_ALIEN. */
  size_t ack_done;
  size_t unscheduled_nwrite;
  nghttp3_stream_type type;
  nghttp3_stream_read_state rstate;
  /* pp is nghttp3_push_promise that this stream fulfills. */
  nghttp3_push_promise *pp;
  /* error_code indicates the reason of closure of this stream. */
  uint64_t error_code;

  struct {
    nghttp3_stream_http_state hstate;
  } tx;

  struct {
    nghttp3_stream_http_state hstate;
    nghttp3_http_state http;
  } rx;

  uint16_t flags;
};

typedef struct nghttp3_frame_entry {
  nghttp3_frame fr;
  union {
    struct {
      nghttp3_settings *local_settings;
    } settings;
    struct {
      nghttp3_data_reader dr;
    } data;
  } aux;
} nghttp3_frame_entry;

int nghttp3_stream_new(nghttp3_stream **pstream, int64_t stream_id,
                       uint64_t seq, const nghttp3_stream_callbacks *callbacks,
                       const nghttp3_mem *mem);

void nghttp3_stream_del(nghttp3_stream *stream);

void nghttp3_varint_read_state_reset(nghttp3_varint_read_state *rvint);

void nghttp3_stream_read_state_reset(nghttp3_stream_read_state *rstate);

nghttp3_ssize nghttp3_read_varint(nghttp3_varint_read_state *rvint,
                                  const uint8_t *src, size_t srclen, int fin);

int nghttp3_stream_frq_add(nghttp3_stream *stream,
                           const nghttp3_frame_entry *frent);

int nghttp3_stream_fill_outq(nghttp3_stream *stream);

int nghttp3_stream_write_stream_type(nghttp3_stream *stream);

int nghttp3_stream_write_stream_type_push_id(nghttp3_stream *stream);

nghttp3_ssize nghttp3_stream_writev(nghttp3_stream *stream, int *pfin,
                                    nghttp3_vec *vec, size_t veccnt);

int nghttp3_stream_write_qpack_decoder_stream(nghttp3_stream *stream);

int nghttp3_stream_outq_is_full(nghttp3_stream *stream);

int nghttp3_stream_outq_add(nghttp3_stream *stream,
                            const nghttp3_typed_buf *tbuf);

int nghttp3_stream_write_headers(nghttp3_stream *stream,
                                 nghttp3_frame_entry *frent);

int nghttp3_stream_write_push_promise(nghttp3_stream *stream,
                                      nghttp3_frame_entry *frent);

int nghttp3_stream_write_header_block(nghttp3_stream *stream,
                                      nghttp3_qpack_encoder *qenc,
                                      nghttp3_stream *qenc_stream,
                                      nghttp3_buf *rbuf, nghttp3_buf *ebuf,
                                      int64_t frame_type, int64_t push_id,
                                      const nghttp3_nv *nva, size_t nvlen);

int nghttp3_stream_write_data(nghttp3_stream *stream, int *peof,
                              nghttp3_frame_entry *frent);

int nghttp3_stream_write_settings(nghttp3_stream *stream,
                                  nghttp3_frame_entry *frent);

int nghttp3_stream_write_cancel_push(nghttp3_stream *stream,
                                     nghttp3_frame_entry *frent);

int nghttp3_stream_write_max_push_id(nghttp3_stream *stream,
                                     nghttp3_frame_entry *frent);

int nghttp3_stream_ensure_chunk(nghttp3_stream *stream, size_t need);

nghttp3_buf *nghttp3_stream_get_chunk(nghttp3_stream *stream);

int nghttp3_stream_is_blocked(nghttp3_stream *stream);

int nghttp3_stream_add_outq_offset(nghttp3_stream *stream, size_t n);

/*
 * nghttp3_stream_outq_write_done returns nonzero if all contents in
 * outq have been written.
 */
int nghttp3_stream_outq_write_done(nghttp3_stream *stream);

int nghttp3_stream_add_ack_offset(nghttp3_stream *stream, uint64_t n);

/*
 * nghttp3_stream_is_active returns nonzero if |stream| is active.  In
 * other words, it has something to send.  This function does not take
 * into account its descendants.
 */
int nghttp3_stream_is_active(nghttp3_stream *stream);

/*
 * nghttp3_stream_require_schedule returns nonzero if |stream| should
 * be scheduled.  In other words, |stream| or its descendants have
 * something to send.
 */
int nghttp3_stream_require_schedule(nghttp3_stream *stream);

int nghttp3_stream_buffer_data(nghttp3_stream *stream, const uint8_t *src,
                               size_t srclen);

size_t nghttp3_stream_get_buffered_datalen(nghttp3_stream *stream);

int nghttp3_stream_ensure_qpack_stream_context(nghttp3_stream *stream);

void nghttp3_stream_delete_qpack_stream_context(nghttp3_stream *stream);

int nghttp3_stream_transit_rx_http_state(nghttp3_stream *stream,
                                         nghttp3_stream_http_event event);

int nghttp3_stream_empty_headers_allowed(nghttp3_stream *stream);

/*
 * nghttp3_stream_bidi_or_push returns nonzero if |stream| is
 * bidirectional or push stream.
 */
int nghttp3_stream_bidi_or_push(nghttp3_stream *stream);

/*
 * nghttp3_stream_uni returns nonzero if stream identified by
 * |stream_id| is unidirectional.
 */
int nghttp3_stream_uni(int64_t stream_id);

/*
 * nghttp3_client_stream_bidi returns nonzero if stream identified by
 * |stream_id| is client initiated bidirectional stream.
 */
int nghttp3_client_stream_bidi(int64_t stream_id);

/*
 * nghttp3_client_stream_uni returns nonzero if stream identified by
 * |stream_id| is client initiated unidirectional stream.
 */
int nghttp3_client_stream_uni(int64_t stream_id);

/*
 * nghttp3_server_stream_uni returns nonzero if stream identified by
 * |stream_id| is server initiated unidirectional stream.
 */
int nghttp3_server_stream_uni(int64_t stream_id);

#endif /* NGHTTP3_STREAM_H */
