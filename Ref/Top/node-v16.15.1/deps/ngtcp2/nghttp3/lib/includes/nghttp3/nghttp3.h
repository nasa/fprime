/*
 * nghttp3
 *
 * Copyright (c) 2018 nghttp3 contributors
 * Copyright (c) 2017 ngtcp2 contributors
 * Copyright (c) 2017 nghttp2 contributors
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
#ifndef NGHTTP3_H
#define NGHTTP3_H

/* Define WIN32 when build target is Win32 API (borrowed from
   libcurl) */
#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#  define WIN32
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#if defined(_MSC_VER) && (_MSC_VER < 1800)
/* MSVC < 2013 does not have inttypes.h because it is not C99
   compliant.  See compiler macros and version number in
   https://sourceforge.net/p/predef/wiki/Compilers/ */
#  include <stdint.h>
#else /* !defined(_MSC_VER) || (_MSC_VER >= 1800) */
#  include <inttypes.h>
#endif /* !defined(_MSC_VER) || (_MSC_VER >= 1800) */
#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>

#include <nghttp3/version.h>

#ifdef NGHTTP3_STATICLIB
#  define NGHTTP3_EXTERN
#elif defined(WIN32)
#  ifdef BUILDING_NGHTTP3
#    define NGHTTP3_EXTERN __declspec(dllexport)
#  else /* !BUILDING_NGHTTP3 */
#    define NGHTTP3_EXTERN __declspec(dllimport)
#  endif /* !BUILDING_NGHTTP3 */
#else    /* !defined(WIN32) */
#  ifdef BUILDING_NGHTTP3
#    define NGHTTP3_EXTERN __attribute__((visibility("default")))
#  else /* !BUILDING_NGHTTP3 */
#    define NGHTTP3_EXTERN
#  endif /* !BUILDING_NGHTTP3 */
#endif   /* !defined(WIN32) */

/**
 * @typedef
 *
 * :type:`nghttp3_ssize` is signed counterpart of size_t.
 */
typedef ptrdiff_t nghttp3_ssize;

/**
 * @macro
 *
 * :macro:`NGHTTP3_ALPN_H3` is a serialized form of HTTP/3 ALPN
 * protocol identifier this library supports.  Notice that the first
 * byte is the length of the following protocol identifier.
 */
#define NGHTTP3_ALPN_H3 "\x2h3"

/**
 * @macrosection
 *
 * nghttp3 library error codes
 */

/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_INVALID_ARGUMENT` indicates that a passed
 * argument is invalid.
 */
#define NGHTTP3_ERR_INVALID_ARGUMENT -101
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_NOBUF` indicates that a provided buffer does
 * not have enough space to store data.
 */
#define NGHTTP3_ERR_NOBUF -102
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_INVALID_STATE` indicates that a requested
 * operation is not allowed at the current connection state.
 */
#define NGHTTP3_ERR_INVALID_STATE -103
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_WOULDBLOCK` indicates that an operation might
 * block.
 */
#define NGHTTP3_ERR_WOULDBLOCK -104
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_STREAM_IN_USE` indicates that a stream ID is
 * already in use.
 */
#define NGHTTP3_ERR_STREAM_IN_USE -105
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_PUSH_ID_BLOCKED` indicates that there are no
 * spare push ID available.
 */
#define NGHTTP3_ERR_PUSH_ID_BLOCKED -106
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_MALFORMED_HTTP_HEADER` indicates that an HTTP
 * header field is malformed.
 */
#define NGHTTP3_ERR_MALFORMED_HTTP_HEADER -107
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_REMOVE_HTTP_HEADER` indicates that an HTTP
 * header field is discarded.
 */
#define NGHTTP3_ERR_REMOVE_HTTP_HEADER -108
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_MALFORMED_HTTP_MESSAGING` indicates that HTTP
 * messaging is malformed.
 */
#define NGHTTP3_ERR_MALFORMED_HTTP_MESSAGING -109
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_QPACK_FATAL` indicates that a fatal error is
 * occurred during QPACK processing and it cannot be recoverable.
 */
#define NGHTTP3_ERR_QPACK_FATAL -111
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_QPACK_HEADER_TOO_LARGE` indicates that a header
 * field is too large to process.
 */
#define NGHTTP3_ERR_QPACK_HEADER_TOO_LARGE -112
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_IGNORE_STREAM` indicates that a stream should
 * be ignored.
 */
#define NGHTTP3_ERR_IGNORE_STREAM -113
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_STREAM_NOT_FOUND` indicates that a stream is
 * not found.
 */
#define NGHTTP3_ERR_STREAM_NOT_FOUND -114
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_IGNORE_PUSH_PROMISE` indicates that a push
 * promise should be ignored.
 */
#define NGHTTP3_ERR_IGNORE_PUSH_PROMISE -115
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_CONN_CLOSING` indicates that a connection is
 * closing state.
 */
#define NGHTTP3_ERR_CONN_CLOSING -116
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_QPACK_DECOMPRESSION_FAILED` indicates that a
 * QPACK decompression failed.
 */
#define NGHTTP3_ERR_QPACK_DECOMPRESSION_FAILED -402
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_QPACK_ENCODER_STREAM_ERROR` indicates that an
 * error occurred while reading QPACK encoder stream.
 */
#define NGHTTP3_ERR_QPACK_ENCODER_STREAM_ERROR -403
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_QPACK_DECODER_STREAM_ERROR` indicates that an
 * error occurred while reading QPACK decoder stream.
 */
#define NGHTTP3_ERR_QPACK_DECODER_STREAM_ERROR -404
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_FRAME_UNEXPECTED` indicates that an
 * unexpected HTTP/3 frame is received.
 */
#define NGHTTP3_ERR_H3_FRAME_UNEXPECTED -408
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_FRAME_ERROR` indicates that an HTTP/3 frame
 * is malformed.
 */
#define NGHTTP3_ERR_H3_FRAME_ERROR -409
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_MISSING_SETTINGS` indicates that an HTTP/3
 * SETTINGS frame is missing.
 */
#define NGHTTP3_ERR_H3_MISSING_SETTINGS -665
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_INTERNAL_ERROR` indicates an internal error.
 */
#define NGHTTP3_ERR_H3_INTERNAL_ERROR -667
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM` indicates that a
 * critical stream is closed.
 */
#define NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM -668
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_GENERAL_PROTOCOL_ERROR` indicates a general
 * protocol error.  This is typically a catch-all error.
 */
#define NGHTTP3_ERR_H3_GENERAL_PROTOCOL_ERROR -669
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_ID_ERROR` indicates that an ID related error
 * occurred.
 */
#define NGHTTP3_ERR_H3_ID_ERROR -670
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_SETTINGS_ERROR` indicates that an HTTP/3
 * SETTINGS frame is malformed.
 */
#define NGHTTP3_ERR_H3_SETTINGS_ERROR -671
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_H3_STREAM_CREATION_ERROR` indicates that a
 * remote endpoint attempts to create a new stream which is not
 * allowed.
 */
#define NGHTTP3_ERR_H3_STREAM_CREATION_ERROR -672
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_FATAL` indicates that error codes less than
 * this value is fatal error.  When this error is returned, an
 * endpoint should drop connection immediately.
 */
#define NGHTTP3_ERR_FATAL -900
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_NOMEM` indicates out of memory.
 */
#define NGHTTP3_ERR_NOMEM -901
/**
 * @macro
 *
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` indicates that user defined
 * callback function failed.
 */
#define NGHTTP3_ERR_CALLBACK_FAILURE -902

/**
 * @macrosection
 *
 * HTTP/3 application error code
 */

/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_NO_ERROR` is HTTP/3 application error code
 * ``H3_NO_ERROR``.
 */
#define NGHTTP3_H3_NO_ERROR 0x0100
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_GENERAL_PROTOCOL_ERROR` is HTTP/3 application
 * error code ``H3_GENERAL_PROTOCOL_ERROR``.
 */
#define NGHTTP3_H3_GENERAL_PROTOCOL_ERROR 0x0101
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_INTERNAL_ERROR` is HTTP/3 application error code
 * ``H3_INTERNAL_ERROR``.
 */
#define NGHTTP3_H3_INTERNAL_ERROR 0x0102
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_STREAM_CREATION_ERROR` is HTTP/3 application
 * error code ``H3_STREAM_CREATION_ERROR``.
 */
#define NGHTTP3_H3_STREAM_CREATION_ERROR 0x0103
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_CLOSED_CRITICAL_STREAM` is HTTP/3 application
 * error code ``H3_CLOSED_CRITICAL_STREAM``.
 */
#define NGHTTP3_H3_CLOSED_CRITICAL_STREAM 0x0104
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_FRAME_UNEXPECTED` is HTTP/3 application error
 * code ``H3_FRAME_UNEXPECTED``.
 */
#define NGHTTP3_H3_FRAME_UNEXPECTED 0x0105
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_FRAME_ERROR` is HTTP/3 application error code
 * ``H3_FRAME_ERROR``.
 */
#define NGHTTP3_H3_FRAME_ERROR 0x0106
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_EXCESSIVE_LOAD` is HTTP/3 application error code
 * ``H3_EXCESSIVE_LOAD``.
 */
#define NGHTTP3_H3_EXCESSIVE_LOAD 0x0107
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_ID_ERROR` is HTTP/3 application error code
 * ``H3_ID_ERROR``.
 */
#define NGHTTP3_H3_ID_ERROR 0x0108
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_SETTINGS_ERROR` is HTTP/3 application error code
 * ``H3_SETTINGS_ERROR``.
 */
#define NGHTTP3_H3_SETTINGS_ERROR 0x0109
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_MISSING_SETTINGS` is HTTP/3 application error
 * code ``H3_MISSING_SETTINGS``.
 */
#define NGHTTP3_H3_MISSING_SETTINGS 0x010a
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_REQUEST_REJECTED` is HTTP/3 application error
 * code ``H3_REQUEST_REJECTED``.
 */
#define NGHTTP3_H3_REQUEST_REJECTED 0x010b
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_REQUEST_CANCELLED` is HTTP/3 application error
 * code ``H3_REQUEST_CANCELLED``.
 */
#define NGHTTP3_H3_REQUEST_CANCELLED 0x010c
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_REQUEST_INCOMPLETE` is HTTP/3 application error
 * code ``H3_REQUEST_INCOMPLETE``.
 */
#define NGHTTP3_H3_REQUEST_INCOMPLETE 0x010d
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_MESSAGE_ERROR` is HTTP/3 application error code
 * ``H3_MESSAGE_ERROR``.
 */
#define NGHTTP3_H3_MESSAGE_ERROR 0x010e
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_CONNECT_ERROR` is HTTP/3 application error code
 * ``H3_CONNECT_ERROR``.
 */
#define NGHTTP3_H3_CONNECT_ERROR 0x010f
/**
 * @macro
 *
 * :macro:`NGHTTP3_H3_VERSION_FALLBACK` is HTTP/3 application error
 * code ``H3_VERSION_FALLBACK``.
 */
#define NGHTTP3_H3_VERSION_FALLBACK 0x0110
/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECOMPRESSION_FAILED` is HTTP/3 application
 * error code ``QPACK_DECOMPRESSION_FAILED``.
 */
#define NGHTTP3_QPACK_DECOMPRESSION_FAILED 0x0200
/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_ENCODER_STREAM_ERROR` is HTTP/3 application
 * error code ``QPACK_ENCODER_STREAM_ERROR``.
 */
#define NGHTTP3_QPACK_ENCODER_STREAM_ERROR 0x0201
/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECODER_STREAM_ERROR` is HTTP/3 application
 * error code ``QPACK_DECODER_STREAM_ERROR``.
 */
#define NGHTTP3_QPACK_DECODER_STREAM_ERROR 0x0202

/**
 * @functypedef
 *
 * Custom memory allocator to replace malloc().  The |mem_user_data|
 * is the mem_user_data member of :type:`nghttp3_mem` structure.
 */
typedef void *(*nghttp3_malloc)(size_t size, void *mem_user_data);

/**
 * @functypedef
 *
 * Custom memory allocator to replace free().  The |mem_user_data| is
 * the mem_user_data member of :type:`nghttp3_mem` structure.
 */
typedef void (*nghttp3_free)(void *ptr, void *mem_user_data);

/**
 * @functypedef
 *
 * Custom memory allocator to replace calloc().  The |mem_user_data|
 * is the mem_user_data member of :type:`nghttp3_mem` structure.
 */
typedef void *(*nghttp3_calloc)(size_t nmemb, size_t size, void *mem_user_data);

/**
 * @functypedef
 *
 * Custom memory allocator to replace realloc().  The |mem_user_data|
 * is the mem_user_data member of :type:`nghttp3_mem` structure.
 */
typedef void *(*nghttp3_realloc)(void *ptr, size_t size, void *mem_user_data);

/**
 * @struct
 *
 * :type:`nghttp3_mem` is a custom memory allocator functions and user
 * defined pointer.  The |mem_user_data| member is passed to each
 * allocator function.  This can be used, for example, to achieve
 * per-session memory pool.
 *
 * In the following example code, ``my_malloc``, ``my_free``,
 * ``my_calloc`` and ``my_realloc`` are the replacement of the
 * standard allocators ``malloc``, ``free``, ``calloc`` and
 * ``realloc`` respectively::
 *
 *     void *my_malloc_cb(size_t size, void *mem_user_data) {
 *       return my_malloc(size);
 *     }
 *
 *     void my_free_cb(void *ptr, void *mem_user_data) { my_free(ptr); }
 *
 *     void *my_calloc_cb(size_t nmemb, size_t size, void *mem_user_data) {
 *       return my_calloc(nmemb, size);
 *     }
 *
 *     void *my_realloc_cb(void *ptr, size_t size, void *mem_user_data) {
 *       return my_realloc(ptr, size);
 *     }
 *
 *     void conn_new() {
 *       nghttp3_mem mem = {NULL, my_malloc_cb, my_free_cb, my_calloc_cb,
 *                          my_realloc_cb};
 *
 *       ...
 *     }
 */
typedef struct nghttp3_mem {
  /**
   * :member:`mem_user_data` is an arbitrary user supplied data.  This
   * is passed to each allocator function.
   */
  void *mem_user_data;
  /**
   * :member:`malloc` is a custom allocator function to replace
   * malloc().
   */
  nghttp3_malloc malloc;
  /**
   * :member:`free` is a custom allocator function to replace free().
   */
  nghttp3_free free;
  /**
   * :member:`calloc` is a custom allocator function to replace
   * calloc().
   */
  nghttp3_calloc calloc;
  /**
   * :member:`realloc` is a custom allocator function to replace
   * realloc().
   */
  nghttp3_realloc realloc;
} nghttp3_mem;

/**
 * @function
 *
 * `nghttp3_mem_default` returns the default memory allocator which
 * uses malloc/calloc/realloc/free.
 */
NGHTTP3_EXTERN const nghttp3_mem *nghttp3_mem_default(void);

/**
 * @struct
 *
 * :type:`nghttp3_vec` is ``struct iovec`` compatible structure to
 * reference arbitrary array of bytes.
 */
typedef struct nghttp3_vec {
  /**
   * :member:`base` points to the data.
   */
  uint8_t *base;
  /**
   * :member:`len` is the number of bytes which the buffer pointed by
   * base contains.
   */
  size_t len;
} nghttp3_vec;

/**
 * @struct
 *
 * :type:`nghttp3_rcbuf` is the object representing reference counted
 * buffer.  The details of this structure are intentionally hidden
 * from the public API.
 */
typedef struct nghttp3_rcbuf nghttp3_rcbuf;

/**
 * @function
 *
 * `nghttp3_rcbuf_incref` increments the reference count of |rcbuf| by
 * 1.
 */
NGHTTP3_EXTERN void nghttp3_rcbuf_incref(nghttp3_rcbuf *rcbuf);

/**
 * @function
 *
 * `nghttp3_rcbuf_decref` decrements the reference count of |rcbuf| by
 * 1.  If the reference count becomes zero, the object pointed by
 * |rcbuf| will be freed.  In this case, application must not use
 * |rcbuf| again.
 */
NGHTTP3_EXTERN void nghttp3_rcbuf_decref(nghttp3_rcbuf *rcbuf);

/**
 * @function
 *
 * `nghttp3_rcbuf_get_buf` returns the underlying buffer managed by
 * |rcbuf|.
 */
NGHTTP3_EXTERN nghttp3_vec nghttp3_rcbuf_get_buf(const nghttp3_rcbuf *rcbuf);

/**
 * @function
 *
 * `nghttp3_rcbuf_is_static` returns nonzero if the underlying buffer
 * is statically allocated, and 0 otherwise. This can be useful for
 * language bindings that wish to avoid creating duplicate strings for
 * these buffers.
 */
NGHTTP3_EXTERN int nghttp3_rcbuf_is_static(const nghttp3_rcbuf *rcbuf);

/**
 * @struct
 *
 * :type:`nghttp3_buf` is the variable size buffer.
 */
typedef struct nghttp3_buf {
  /**
   * :member:`begin` points to the beginning of the buffer.
   */
  uint8_t *begin;
  /**
   * :member:`end` points to the one beyond of the last byte of the
   * buffer
   */
  uint8_t *end;
  /**
   * :member:`pos` pointers to the start of data.  Typically, this
   * points to the point that next data should be read.  Initially, it
   * points to :member:`begin`.
   */
  uint8_t *pos;
  /**
   * :member:`last` points to the one beyond of the last data of the
   * buffer.  Typically, new data is written at this point.
   * Initially, it points to :member:`begin`.
   */
  uint8_t *last;
} nghttp3_buf;

/**
 * @function
 *
 * `nghttp3_buf_init` initializes empty |buf|.
 */
NGHTTP3_EXTERN void nghttp3_buf_init(nghttp3_buf *buf);

/**
 * @function
 *
 * `nghttp3_buf_free` frees resources allocated for |buf| using |mem|
 * as memory allocator.  buf->begin must be a heap buffer allocated by
 * |mem|.
 */
NGHTTP3_EXTERN void nghttp3_buf_free(nghttp3_buf *buf, const nghttp3_mem *mem);

/**
 * @function
 *
 * `nghttp3_buf_left` returns the number of additional bytes which can
 * be written to the underlying buffer.  In other words, it returns
 * buf->end - buf->last.
 */
NGHTTP3_EXTERN size_t nghttp3_buf_left(const nghttp3_buf *buf);

/**
 * @function
 *
 * `nghttp3_buf_len` returns the number of bytes left to read.  In
 * other words, it returns buf->last - buf->pos.
 */
NGHTTP3_EXTERN size_t nghttp3_buf_len(const nghttp3_buf *buf);

/**
 * @function
 *
 * `nghttp3_buf_reset` sets buf->pos and buf->last to buf->begin.
 */
NGHTTP3_EXTERN void nghttp3_buf_reset(nghttp3_buf *buf);

/**
 * @macrosection
 *
 * Flags for header field name/value pair
 */

/**
 * @macro
 *
 * :macro:`NGHTTP3_NV_FLAG_NONE` indicates no flag set.
 */
#define NGHTTP3_NV_FLAG_NONE 0

/**
 * @macro
 *
 * :macro:`NGHTTP3_NV_FLAG_NEVER_INDEX` indicates that this name/value
 * pair must not be indexed.  Other implementation calls this bit as
 * "sensitive".
 */
#define NGHTTP3_NV_FLAG_NEVER_INDEX 0x01

/**
 * @macro
 *
 * :macro:`NGHTTP3_NV_FLAG_NO_COPY_NAME` is set solely by application.
 * If this flag is set, the library does not make a copy of header
 * field name.  This could improve performance.
 */
#define NGHTTP3_NV_FLAG_NO_COPY_NAME 0x02

/**
 * @macro
 *
 * :macro:`NGHTTP3_NV_FLAG_NO_COPY_VALUE` is set solely by
 * application.  If this flag is set, the library does not make a copy
 * of header field value.  This could improve performance.
 */
#define NGHTTP3_NV_FLAG_NO_COPY_VALUE 0x04

/**
 * @struct
 *
 * :type:`nghttp3_nv` is the name/value pair, which mainly used to
 * represent header fields.
 */
typedef struct nghttp3_nv {
  /**
   * :member:`name` is the header field name.
   */
  uint8_t *name;
  /**
   * :member:`value` is the header field value.
   */
  uint8_t *value;
  /**
   * :member:`namelen` is the length of the |name|, excluding
   * terminating NULL.
   */
  size_t namelen;
  /**
   * :member:`valuelen` is the length of the |value|, excluding
   * terminating NULL.
   */
  size_t valuelen;
  /**
   * :member:`flags` is bitwise OR of one or more of
   * NGHTTP3_NV_FLAG_*.
   */
  uint8_t flags;
} nghttp3_nv;

/* Generated by mkstatichdtbl.py */
/**
 * @enum
 *
 * :type:`nghttp3_qpack_token` defines HTTP header field name tokens
 * to identify field name quickly.  It appears in
 * :member:`nghttp3_qpack_nv.token`.
 */
typedef enum nghttp3_qpack_token {
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__AUTHORITY` is a token for
   * ``:authority``.
   */
  NGHTTP3_QPACK_TOKEN__AUTHORITY = 0,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__PATH` is a token for ``:path``.
   */
  NGHTTP3_QPACK_TOKEN__PATH = 8,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_AGE` is a token for ``age``.
   */
  NGHTTP3_QPACK_TOKEN_AGE = 43,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONTENT_DISPOSITION` is a token for
   * ``content-disposition``.
   */
  NGHTTP3_QPACK_TOKEN_CONTENT_DISPOSITION = 52,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONTENT_LENGTH` is a token for
   * ``content-length``.
   */
  NGHTTP3_QPACK_TOKEN_CONTENT_LENGTH = 55,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_COOKIE` is a token for ``cookie``.
   */
  NGHTTP3_QPACK_TOKEN_COOKIE = 68,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_DATE` is a token for ``date``.
   */
  NGHTTP3_QPACK_TOKEN_DATE = 69,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ETAG` is a token for ``etag``.
   */
  NGHTTP3_QPACK_TOKEN_ETAG = 71,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_IF_MODIFIED_SINCE` is a token for
   * ``if-modified-since``.
   */
  NGHTTP3_QPACK_TOKEN_IF_MODIFIED_SINCE = 74,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_IF_NONE_MATCH` is a token for
   * ``if-none-match``.
   */
  NGHTTP3_QPACK_TOKEN_IF_NONE_MATCH = 75,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_LAST_MODIFIED` is a token for
   * ``last-modified``.
   */
  NGHTTP3_QPACK_TOKEN_LAST_MODIFIED = 77,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_LINK` is a token for ``link``.
   */
  NGHTTP3_QPACK_TOKEN_LINK = 78,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_LOCATION` is a token for ``location``.
   */
  NGHTTP3_QPACK_TOKEN_LOCATION = 79,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_REFERER` is a token for ``referer``.
   */
  NGHTTP3_QPACK_TOKEN_REFERER = 83,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_SET_COOKIE` is a token for
   * ``set-cookie``.
   */
  NGHTTP3_QPACK_TOKEN_SET_COOKIE = 85,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__METHOD` is a token for ``:method``.
   */
  NGHTTP3_QPACK_TOKEN__METHOD = 1,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__SCHEME` is a token for ``:scheme``.
   */
  NGHTTP3_QPACK_TOKEN__SCHEME = 9,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__STATUS` is a token for ``:status``.
   */
  NGHTTP3_QPACK_TOKEN__STATUS = 11,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCEPT` is a token for ``accept``.
   */
  NGHTTP3_QPACK_TOKEN_ACCEPT = 25,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCEPT_ENCODING` is a token for
   * ``accept-encoding``.
   */
  NGHTTP3_QPACK_TOKEN_ACCEPT_ENCODING = 27,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCEPT_RANGES` is a token for
   * ``accept-ranges``.
   */
  NGHTTP3_QPACK_TOKEN_ACCEPT_RANGES = 29,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_HEADERS` is a
   * token for ``access-control-allow-headers``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_HEADERS = 32,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_ORIGIN` is a
   * token for ``access-control-allow-origin``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_ORIGIN = 38,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CACHE_CONTROL` is a token for
   * ``cache-control``.
   */
  NGHTTP3_QPACK_TOKEN_CACHE_CONTROL = 46,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONTENT_ENCODING` is a token for
   * ``content-encoding``.
   */
  NGHTTP3_QPACK_TOKEN_CONTENT_ENCODING = 53,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONTENT_TYPE` is a token for
   * ``content-type``.
   */
  NGHTTP3_QPACK_TOKEN_CONTENT_TYPE = 57,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_RANGE` is a token for ``range``.
   */
  NGHTTP3_QPACK_TOKEN_RANGE = 82,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_STRICT_TRANSPORT_SECURITY` is a token
   * for ``strict-transport-security``.
   */
  NGHTTP3_QPACK_TOKEN_STRICT_TRANSPORT_SECURITY = 86,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_VARY` is a token for ``vary``.
   */
  NGHTTP3_QPACK_TOKEN_VARY = 92,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_X_CONTENT_TYPE_OPTIONS` is a token for
   * ``x-content-type-options``.
   */
  NGHTTP3_QPACK_TOKEN_X_CONTENT_TYPE_OPTIONS = 94,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_X_XSS_PROTECTION` is a token for
   * ``x-xss-protection``.
   */
  NGHTTP3_QPACK_TOKEN_X_XSS_PROTECTION = 98,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCEPT_LANGUAGE` is a token for
   * ``accept-language``.
   */
  NGHTTP3_QPACK_TOKEN_ACCEPT_LANGUAGE = 28,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_CREDENTIALS` is a
   * token for ``access-control-allow-credentials``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_CREDENTIALS = 30,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_METHODS` is a
   * token for ``access-control-allow-methods``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_ALLOW_METHODS = 35,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_EXPOSE_HEADERS` is a
   * token for ``access-control-expose-headers``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_EXPOSE_HEADERS = 39,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_REQUEST_HEADERS` is a
   * token for ``access-control-request-headers``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_REQUEST_HEADERS = 40,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_REQUEST_METHOD` is a
   * token for ``access-control-request-method``.
   */
  NGHTTP3_QPACK_TOKEN_ACCESS_CONTROL_REQUEST_METHOD = 41,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ALT_SVC` is a token for ``alt-svc``.
   */
  NGHTTP3_QPACK_TOKEN_ALT_SVC = 44,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_AUTHORIZATION` is a token for
   * ``authorization``.
   */
  NGHTTP3_QPACK_TOKEN_AUTHORIZATION = 45,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONTENT_SECURITY_POLICY` is a token
   * for ``content-security-policy``.
   */
  NGHTTP3_QPACK_TOKEN_CONTENT_SECURITY_POLICY = 56,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_EARLY_DATA` is a token for
   * ``early-data``.
   */
  NGHTTP3_QPACK_TOKEN_EARLY_DATA = 70,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_EXPECT_CT` is a token for
   * ``expect-ct``.
   */
  NGHTTP3_QPACK_TOKEN_EXPECT_CT = 72,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_FORWARDED` is a token for
   * ``forwarded``.
   */
  NGHTTP3_QPACK_TOKEN_FORWARDED = 73,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_IF_RANGE` is a token for ``if-range``.
   */
  NGHTTP3_QPACK_TOKEN_IF_RANGE = 76,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_ORIGIN` is a token for ``origin``.
   */
  NGHTTP3_QPACK_TOKEN_ORIGIN = 80,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_PURPOSE` is a token for ``purpose``.
   */
  NGHTTP3_QPACK_TOKEN_PURPOSE = 81,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_SERVER` is a token for ``server``.
   */
  NGHTTP3_QPACK_TOKEN_SERVER = 84,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_TIMING_ALLOW_ORIGIN` is a token for
   * ``timing-allow-origin``.
   */
  NGHTTP3_QPACK_TOKEN_TIMING_ALLOW_ORIGIN = 89,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_UPGRADE_INSECURE_REQUESTS` is a token
   * for ``upgrade-insecure-requests``.
   */
  NGHTTP3_QPACK_TOKEN_UPGRADE_INSECURE_REQUESTS = 90,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_USER_AGENT` is a token for
   * ``user-agent``.
   */
  NGHTTP3_QPACK_TOKEN_USER_AGENT = 91,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_X_FORWARDED_FOR` is a token for
   * ``x-forwarded-for``.
   */
  NGHTTP3_QPACK_TOKEN_X_FORWARDED_FOR = 95,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_X_FRAME_OPTIONS` is a token for
   * ``x-frame-options``.
   */
  NGHTTP3_QPACK_TOKEN_X_FRAME_OPTIONS = 96,

  /* Additional header fields for HTTP messaging validation */

  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_HOST` is a token for ``host``.
   */
  NGHTTP3_QPACK_TOKEN_HOST = 1000,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_CONNECTION` is a token for
   * ``connection``.
   */
  NGHTTP3_QPACK_TOKEN_CONNECTION,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_KEEP_ALIVE` is a token for
   * ``keep-alive``.
   */
  NGHTTP3_QPACK_TOKEN_KEEP_ALIVE,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_PROXY_CONNECTION` is a token for
   * ``proxy-connection``.
   */
  NGHTTP3_QPACK_TOKEN_PROXY_CONNECTION,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_TRANSFER_ENCODING` is a token for
   * ``transfer-encoding``.
   */
  NGHTTP3_QPACK_TOKEN_TRANSFER_ENCODING,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_UPGRADE` is a token for ``upgrade``.
   */
  NGHTTP3_QPACK_TOKEN_UPGRADE,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_TE` is a token for ``te``.
   */
  NGHTTP3_QPACK_TOKEN_TE,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN__PROTOCOL` is a token for
   * ``:protocol``.
   */
  NGHTTP3_QPACK_TOKEN__PROTOCOL,
  /**
   * :enum:`NGHTTP3_QPACK_TOKEN_PRIORITY` is a token for ``priority``.
   */
  NGHTTP3_QPACK_TOKEN_PRIORITY
} nghttp3_qpack_token;

/**
 * @struct
 *
 * :type:`nghttp3_qpack_nv` represents header field name/value pair
 * just like :type:`nghttp3_nv`.  It is an extended version of
 * :type:`nghttp3_nv` and has reference counted buffers and tokens
 * which might be useful for applications.
 */
typedef struct {
  /**
   * :member:`name` is the buffer containing header field name.
   * NULL-termination is guaranteed.
   */
  nghttp3_rcbuf *name;
  /**
   * :member:`value` is the buffer containing header field value.
   * NULL-termination is guaranteed.
   */
  nghttp3_rcbuf *value;
  /**
   * :member:`token` is :type:`nghttp3_qpack_token` value of
   *  :member:`name`.  It could be -1 if we have no token for that
   *  header field name.
   */
  int32_t token;
  /**
   * :member:`flags` is a bitwise OR of one or more of
   * NGHTTP3_NV_FLAG_*.  See :macro:`NGHTTP3_NV_FLAG_NONE`.
   */
  uint8_t flags;
} nghttp3_qpack_nv;

/**
 * @struct
 *
 * :type:`nghttp3_qpack_encoder` is QPACK encoder.
 */
typedef struct nghttp3_qpack_encoder nghttp3_qpack_encoder;

/**
 * @function
 *
 * `nghttp3_qpack_encoder_new` initializes QPACK encoder.  |pencoder|
 * must be non-NULL pointer.  |max_dtable_size| is the maximum dynamic
 * table size.  |max_blocked| is the maximum number of streams which
 * can be blocked.  |mem| is a memory allocator.  This function
 * allocates memory for :type:`nghttp3_qpack_encoder` itself and
 * assigns its pointer to |*pencoder| if it succeeds.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 */
NGHTTP3_EXTERN int nghttp3_qpack_encoder_new(nghttp3_qpack_encoder **pencoder,
                                             size_t max_dtable_size,
                                             size_t max_blocked,
                                             const nghttp3_mem *mem);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_del` frees memory allocated for |encoder|.
 * This function frees memory pointed by |encoder| itself.
 */
NGHTTP3_EXTERN void nghttp3_qpack_encoder_del(nghttp3_qpack_encoder *encoder);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_encode` encodes the list of header fields
 * |nva|.  |nvlen| is the length of |nva|.  |stream_id| is the
 * identifier of the stream which this header fields belong to.  This
 * function writes header block prefix, encoded header fields, and
 * encoder stream to |pbuf|, |rbuf|, and |ebuf| respectively.  The
 * :member:`nghttp3_buf.last` will be adjusted when data is written.
 * An application should write |pbuf| and |rbuf| to the request stream
 * in this order.
 *
 * The buffer pointed by |pbuf|, |rbuf|, and |ebuf| can be empty
 * buffer.  It is fine to pass a buffer initialized by
 * `nghttp3_buf_init(buf) <nghttp3_buf_init>`.  This function
 * allocates memory for these buffers as necessary.  In particular, it
 * frees and expands buffer if the current capacity of buffer is not
 * enough.  If :member:`nghttp3_buf.begin` of any buffer is not NULL,
 * it must be allocated by the same memory allocator passed to
 * `nghttp3_qpack_encoder_new()`.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory
 * :macro:`NGHTTP3_ERR_QPACK_FATAL`
 *      |encoder| is in unrecoverable error state and cannot be used
 *      anymore.
 */
NGHTTP3_EXTERN int nghttp3_qpack_encoder_encode(
    nghttp3_qpack_encoder *encoder, nghttp3_buf *pbuf, nghttp3_buf *rbuf,
    nghttp3_buf *ebuf, int64_t stream_id, const nghttp3_nv *nva, size_t nvlen);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_read_decoder` reads decoder stream.  The
 * buffer pointed by |src| of length |srclen| contains decoder stream.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory
 * :macro:`NGHTTP3_ERR_QPACK_FATAL`
 *     |encoder| is in unrecoverable error state and cannot be used
 *     anymore.
 * :macro:`NGHTTP3_ERR_QPACK_DECODER_STREAM`
 *     |encoder| is unable to process input because it is malformed.
 */
NGHTTP3_EXTERN nghttp3_ssize nghttp3_qpack_encoder_read_decoder(
    nghttp3_qpack_encoder *encoder, const uint8_t *src, size_t srclen);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_set_max_dtable_size` sets max dynamic table
 * size to |max_dtable_size|.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * :macro:`NGHTTP3_ERR_INVALID_ARGUMENT`
 *     |max_dtable_size| exceeds the hard limit that decoder specifies.
 */
NGHTTP3_EXTERN int
nghttp3_qpack_encoder_set_max_dtable_size(nghttp3_qpack_encoder *encoder,
                                          size_t max_dtable_size);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_set_hard_max_dtable_size` sets hard maximum
 * dynamic table size to |hard_max_dtable_size|.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * TBD
 */
NGHTTP3_EXTERN int
nghttp3_qpack_encoder_set_hard_max_dtable_size(nghttp3_qpack_encoder *encoder,
                                               size_t hard_max_dtable_size);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_set_max_blocked` sets the number of streams
 * which can be blocked to |max_blocked|.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * TBD
 */
NGHTTP3_EXTERN int
nghttp3_qpack_encoder_set_max_blocked(nghttp3_qpack_encoder *encoder,
                                      size_t max_blocked);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_ack_header` tells |encoder| that header
 * block for a stream denoted by |stream_id| was acknowledged by
 * decoder.  This function is provided for debugging purpose only.  In
 * HTTP/3, |encoder| knows acknowledgement of header block by reading
 * decoder stream with `nghttp3_qpack_encoder_read_decoder()`.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_encoder_ack_header(nghttp3_qpack_encoder *encoder,
                                 int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_add_insert_count` increments known received
 * count of |encoder| by |n|.  This function is provided for debugging
 * purpose only.  In HTTP/3, |encoder| increments known received count
 * by reading decoder stream with
 * `nghttp3_qpack_encoder_read_decoder()`.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 * :macro:`NGHTTP3_ERR_QPACK_DECODER_STREAM_ERROR`
 *     |n| is too large.
 */
NGHTTP3_EXTERN int
nghttp3_qpack_encoder_add_insert_count(nghttp3_qpack_encoder *encoder,
                                       uint64_t n);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_ack_everything` tells |encoder| that all
 * encoded header blocks are acknowledged.  This function is provided
 * for debugging purpose only.  In HTTP/3, |encoder| knows this by
 * reading decoder stream with `nghttp3_qpack_encoder_read_decoder()`.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_encoder_ack_everything(nghttp3_qpack_encoder *encoder);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_cancel_stream` tells |encoder| that stream
 * denoted by |stream_id| is cancelled.  This function is provided for
 * debugging purpose only.  In HTTP/3, |encoder| knows this by reading
 * decoder stream with `nghttp3_qpack_encoder_read_decoder()`.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_encoder_cancel_stream(nghttp3_qpack_encoder *encoder,
                                    int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_qpack_encoder_get_num_blocked` returns the number of
 * streams which are potentially blocked at decoder side.
 */
NGHTTP3_EXTERN size_t
nghttp3_qpack_encoder_get_num_blocked(nghttp3_qpack_encoder *encoder);

/**
 * @struct
 *
 * :type:`nghttp3_qpack_stream_context` is a decoder context for an
 * individual stream.  Its state is per header block.  In order to
 * reuse this object for another header block, call
 * `nghttp3_qpack_stream_context_reset`.
 */
typedef struct nghttp3_qpack_stream_context nghttp3_qpack_stream_context;

/**
 * @function
 *
 * `nghttp3_qpack_stream_context_new` initializes stream context.
 * |psctx| must be non-NULL pointer.  |stream_id| is stream ID.  |mem|
 * is a memory allocator.  This function allocates memory for
 * :type:`nghttp3_qpack_stream_context` itself and assigns its pointer
 * to |*psctx| if it succeeds.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 */
NGHTTP3_EXTERN int
nghttp3_qpack_stream_context_new(nghttp3_qpack_stream_context **psctx,
                                 int64_t stream_id, const nghttp3_mem *mem);

/**
 * @function
 *
 * `nghttp3_qpack_stream_context_del` frees memory allocated for
 * |sctx|.  This function frees memory pointed by |sctx| itself.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_stream_context_del(nghttp3_qpack_stream_context *sctx);

/**
 * @function
 *
 * `nghttp3_qpack_stream_context_get_ricnt` returns required insert
 * count.
 */
NGHTTP3_EXTERN uint64_t
nghttp3_qpack_stream_context_get_ricnt(nghttp3_qpack_stream_context *sctx);

/**
 * @function
 *
 * `nghttp3_qpack_stream_context_reset` resets the state of |sctx|.
 * Then it can be reused for an another header block in the same
 * stream.
 */
NGHTTP3_EXTERN
void nghttp3_qpack_stream_context_reset(nghttp3_qpack_stream_context *sctx);

/**
 * @struct
 *
 * :type:`nghttp3_qpack_decoder` is QPACK decoder.
 */
typedef struct nghttp3_qpack_decoder nghttp3_qpack_decoder;

/**
 * @function
 *
 * `nghttp3_qpack_decoder_new` initializes QPACK decoder.  |pdecoder|
 * must be non-NULL pointer.  |max_dtable_size| is the maximum dynamic
 * table size.  |max_blocked| is the maximum number of streams which
 * can be blocked.  |mem| is a memory allocator.  This function
 * allocates memory for :type:`nghttp3_qpack_decoder` itself and
 * assigns its pointer to |*pdecoder| if it succeeds.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 */
NGHTTP3_EXTERN int nghttp3_qpack_decoder_new(nghttp3_qpack_decoder **pdecoder,
                                             size_t max_dtable_size,
                                             size_t max_blocked,
                                             const nghttp3_mem *mem);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_del` frees memory allocated for |decoder|.
 * This function frees memory pointed by |decoder| itself.
 */
NGHTTP3_EXTERN void nghttp3_qpack_decoder_del(nghttp3_qpack_decoder *decoder);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_read_encoder` reads encoder stream.  The
 * buffer pointed by |src| of length |srclen| contains encoder stream.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 * :macro:`NGHTTP3_ERR_QPACK_FATAL`
 *     |decoder| is in unrecoverable error state and cannot be used
 *     anymore.
 * :macro:`NGHTTP3_ERR_QPACK_ENCODER_STREAM`
 *     Could not interpret encoder stream instruction.
 */
NGHTTP3_EXTERN nghttp3_ssize nghttp3_qpack_decoder_read_encoder(
    nghttp3_qpack_decoder *decoder, const uint8_t *src, size_t srclen);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_get_icnt` returns insert count.
 */
NGHTTP3_EXTERN uint64_t
nghttp3_qpack_decoder_get_icnt(const nghttp3_qpack_decoder *decoder);

/**
 * @macrosection
 *
 * Flags for QPACK decoder
 */

/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_NONE` indicates that no flag set.
 */
#define NGHTTP3_QPACK_DECODE_FLAG_NONE 0

/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_EMIT` indicates that a header
 * field is successfully decoded.
 */
#define NGHTTP3_QPACK_DECODE_FLAG_EMIT 0x01

/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_FINAL` indicates that all header
 * fields have been decoded.
 */
#define NGHTTP3_QPACK_DECODE_FLAG_FINAL 0x02

/**
 * @macro
 *
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_BLOCKED` indicates that decoding
 * has been blocked.
 */
#define NGHTTP3_QPACK_DECODE_FLAG_BLOCKED 0x04

/**
 * @function
 *
 * `nghttp3_qpack_decoder_read_request` reads request stream.  The
 * request stream is given as the buffer pointed by |src| of length
 * |srclen|.  |sctx| is the stream context and it must be created by
 * `nghttp3_qpack_stream_context_new()`.  |*pflags| must be non-NULL
 * pointer.  |nv| must be non-NULL pointer.
 *
 * If this function succeeds, it assigns flags to |*pflags|.  If
 * |*pflags| has :macro:`NGHTTP3_QPACK_DECODE_FLAG_EMIT` set, a
 * decoded header field is assigned to |nv|.  If |*pflags| has
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_FINAL` set, all header fields
 * have been successfully decoded.  If |*pflags| has
 * :macro:`NGHTTP3_QPACK_DECODE_FLAG_BLOCKED` set, decoding is blocked
 * due to required insert count.
 *
 * When a header field is decoded, an application receives it in |nv|.
 * nv->name and nv->value are reference counted buffer, and their
 * reference counts are already incremented for application use.
 * Therefore, when application finishes processing the header field,
 * it must call `nghttp3_rcbuf_decref(nv->name)
 * <nghttp3_rcbuf_decref>` and `nghttp3_rcbuf_decref(nv->value)
 * <nghttp3_rcbuf_decref>` or memory leak might occur.
 *
 * This function returns the number of bytes read, or one of the
 * following negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 * :macro:`NGHTTP3_ERR_QPACK_FATAL`
 *     |decoder| is in unrecoverable error state and cannot be used
 *     anymore.
 * :macro:`NGHTTP3_ERR_QPACK_DECOMPRESSION_FAILED`
 *     Could not interpret header block instruction.
 * :macro:`NGHTTP3_ERR_QPACK_HEADER_TOO_LARGE`
 *     Header field is too large.
 */
NGHTTP3_EXTERN nghttp3_ssize nghttp3_qpack_decoder_read_request(
    nghttp3_qpack_decoder *decoder, nghttp3_qpack_stream_context *sctx,
    nghttp3_qpack_nv *nv, uint8_t *pflags, const uint8_t *src, size_t srclen,
    int fin);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_write_decoder` writes decoder stream into
 * |dbuf|.
 *
 * The caller must ensure that `nghttp3_buf_left(dbuf)
 * <nghttp3_buf_left>` >=
 * `nghttp3_qpack_decoder_get_decoder_streamlen(decoder)
 * <nghttp3_qpack_decoder_get_decoder_streamlen>`.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_decoder_write_decoder(nghttp3_qpack_decoder *decoder,
                                    nghttp3_buf *dbuf);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_get_decoder_streamlen` returns the length of
 * decoder stream.
 */
NGHTTP3_EXTERN size_t
nghttp3_qpack_decoder_get_decoder_streamlen(nghttp3_qpack_decoder *decoder);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_cancel_stream` cancels header decoding for
 * stream denoted by |stream_id|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 * :macro:`NGHTTP3_ERR_QPACK_FATAL`
 *     Decoder stream overflow.
 */
NGHTTP3_EXTERN int
nghttp3_qpack_decoder_cancel_stream(nghttp3_qpack_decoder *decoder,
                                    int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_set_dtable_cap` sets |cap| as maximum
 * dynamic table size.  Normally, the maximum capacity is communicated
 * in encoder stream.  This function is provided for debugging and
 * testing purpose.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_decoder_set_dtable_cap(nghttp3_qpack_decoder *decoder,
                                     size_t cap);

/**
 * @function
 *
 * `nghttp3_qpack_decoder_set_max_concurrent_streams` tells |decoder|
 * the maximum number of concurrent streams that a remote endpoint can
 * open, including both bidirectional and unidirectional streams which
 * potentially receive QPACK encoded HEADERS frame.  This value is
 * used as a hint to limit the length of decoder stream.
 */
NGHTTP3_EXTERN void
nghttp3_qpack_decoder_set_max_concurrent_streams(nghttp3_qpack_decoder *decoder,
                                                 size_t max_concurrent_streams);

/**
 * @function
 *
 * `nghttp3_strerror` returns textual representation of |liberr|.
 */
NGHTTP3_EXTERN const char *nghttp3_strerror(int liberr);

/**
 * @function
 *
 * `nghttp3_err_infer_quic_app_error_code` returns a QUIC application
 * error code which corresponds to |liberr|.
 */
NGHTTP3_EXTERN uint64_t nghttp3_err_infer_quic_app_error_code(int liberr);

/**
 * @functypedef
 *
 * :type:`nghttp3_debug_vprintf_callback` is a callback function
 * invoked when the library outputs debug logging.  The function is
 * called with arguments suitable for ``vfprintf(3)``
 *
 * The debug output is only enabled if the library is built with
 * ``DEBUGBUILD`` macro defined.
 */
typedef void (*nghttp3_debug_vprintf_callback)(const char *format,
                                               va_list args);

/**
 * @function
 *
 * `nghttp3_set_debug_vprintf_callback` sets a debug output callback
 * called by the library when built with ``DEBUGBUILD`` macro defined.
 * If this option is not used, debug log is written into standard
 * error output.
 *
 * For builds without ``DEBUGBUILD`` macro defined, this function is
 * noop.
 *
 * Note that building with ``DEBUGBUILD`` may cause significant
 * performance penalty to libnghttp3 because of extra processing.  It
 * should be used for debugging purpose only.
 *
 * .. Warning::
 *
 *   Building with ``DEBUGBUILD`` may cause significant performance
 *   penalty to libnghttp3 because of extra processing.  It should be
 *   used for debugging purpose only.  We write this two times because
 *   this is important.
 */
NGHTTP3_EXTERN void nghttp3_set_debug_vprintf_callback(
    nghttp3_debug_vprintf_callback debug_vprintf_callback);

/**
 * @struct
 *
 * :type:`nghttp3_conn` represents a single HTTP/3 connection.
 */
typedef struct nghttp3_conn nghttp3_conn;

/**
 * @functypedef
 *
 * :type:`nghttp3_acked_stream_data` is a callback function which is
 * invoked when data sent on stream denoted by |stream_id| supplied
 * from application is acknowledged by remote endpoint.  The number of
 * bytes acknowledged is given in |datalen|.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_acked_stream_data)(nghttp3_conn *conn, int64_t stream_id,
                                         size_t datalen, void *conn_user_data,
                                         void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_conn_stream_close` is a callback function which is
 * invoked when a stream identified by |stream_id| is closed.
 * |app_error_code| indicates the reason of this closure.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_stream_close)(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code,
                                    void *conn_user_data,
                                    void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_recv_data` is a callback function which is invoked
 * when a part of request or response body on stream identified by
 * |stream_id| is received.  |data| points to the received data and
 * its length is |datalen|.
 *
 * The application is responsible for increasing flow control credit
 * by |datalen| bytes.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_recv_data)(nghttp3_conn *conn, int64_t stream_id,
                                 const uint8_t *data, size_t datalen,
                                 void *conn_user_data, void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_deferred_consume` is a callback function which is
 * invoked when the library consumed |consumed| bytes for a stream
 * identified by |stream_id|.  This callback is used to notify the
 * consumed bytes for stream blocked by QPACK decoder.  The
 * application is responsible for increasing flow control credit by
 * |consumed| bytes.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_deferred_consume)(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, void *conn_user_data,
                                        void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_begin_headers` is a callback function which is
 * invoked when an incoming header block section is started on a
 * stream denoted by |stream_id|.  Each header field is passed to
 * application by :type:`nghttp3_recv_header` callback.  And then
 * :type:`nghttp3_end_headers` is called when a whole header block is
 * processed.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_begin_headers)(nghttp3_conn *conn, int64_t stream_id,
                                     void *conn_user_data,
                                     void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_recv_header` is a callback function which is invoked
 * when a header field is received on a stream denoted by |stream_id|.
 * |name| contains a field name and |value| contains a field value.
 * |token| is one of token defined in :type:`nghttp3_qpack_token` or
 * -1 if no token is defined for |name|.  |flags| is bitwise OR of
 * zero or more of NGHTTP3_NV_FLAG_*.
 *
 * The buffers for |name| and |value| are reference counted. If
 * application needs to keep them, increment the reference count with
 * `nghttp3_rcbuf_incref`.  When they are no longer used, call
 * `nghttp3_rcbuf_decref`.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_recv_header)(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *value, uint8_t flags,
                                   void *conn_user_data,
                                   void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_end_headers` is a callback function which is invoked
 * when an incoming header block has ended.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_end_headers)(nghttp3_conn *conn, int64_t stream_id,
                                   void *conn_user_data,
                                   void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_begin_push_promise` is a callback function which is
 * invoked when an incoming header block section in PUSH_PROMISE is
 * started on a stream denoted by |stream_id|.  |push_id| identifies a
 * push promise.  Each header field is passed to application by
 * :type:`nghttp3_recv_push_promise` callback.  And then
 * :type:`nghttp3_end_push_promise` is called when a whole header
 * block is processed.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_begin_push_promise)(nghttp3_conn *conn, int64_t stream_id,
                                          int64_t push_id, void *conn_user_data,
                                          void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_recv_push_promise` is a callback function which is
 * invoked when a header field in PUSH_PROMISE is received on a stream
 * denoted by |stream_id|.  |push_id| identifies a push promise.
 * |name| contains a field name and |value| contains a field value.
 * |token| is one of token defined in :type:`nghttp3_qpack_token` or
 * -1 if no token is defined for |name|.  |flags| is bitwise OR of
 * zero or more of NGHTTP3_NV_FLAG_*.
 *
 * The buffers for |name| and |value| are reference counted. If
 * application needs to keep them, increment the reference count with
 * `nghttp3_rcbuf_incref`.  When they are no longer used, call
 * `nghttp3_rcbuf_decref`.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_recv_push_promise)(nghttp3_conn *conn, int64_t stream_id,
                                         int64_t push_id, int32_t token,
                                         nghttp3_rcbuf *name,
                                         nghttp3_rcbuf *value, uint8_t flags,
                                         void *conn_user_data,
                                         void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_end_push_promise` is a callback function which is
 * invoked when an incoming header block in PUSH_PROMISE has ended.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_end_push_promise)(nghttp3_conn *conn, int64_t stream_id,
                                        int64_t push_id, void *conn_user_data,
                                        void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_end_stream` is a callback function which is invoked
 * when the receiving side of stream is closed.  For server, this
 * callback function is invoked when HTTP request is received
 * completely.  For client, this callback function is invoked when
 * HTTP response is received completely.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_end_stream)(nghttp3_conn *conn, int64_t stream_id,
                                  void *conn_user_data, void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_cancel_push` is a callback function which is invoked
 * when the push identified by |push_id| is cancelled by remote
 * endpoint.  If a stream has been bound to the push ID, |stream_id|
 * contains the stream ID and |stream_user_data| points to the stream
 * user data.  Otherwise, |stream_id| is -1 and |stream_user_data| is
 * NULL.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_cancel_push)(nghttp3_conn *conn, int64_t push_id,
                                   int64_t stream_id, void *conn_user_data,
                                   void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_send_stop_sending` is a callback function which is
 * invoked when the library asks application to send STOP_SENDING to
 * the stream identified by |stream_id|.  |app_error_code| indicates
 * the reason for this action.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_send_stop_sending)(nghttp3_conn *conn, int64_t stream_id,
                                         uint64_t app_error_code,
                                         void *conn_user_data,
                                         void *stream_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_push_stream` is a callback function which is invoked
 * when a push stream identified by |stream_id| is opened with
 * |push_id|.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_push_stream)(nghttp3_conn *conn, int64_t push_id,
                                   int64_t stream_id, void *conn_user_data);

/**
 * @functypedef
 *
 * :type:`nghttp3_reset_stream` is a callback function which is
 * invoked when the library asks application to reset stream
 * identified by |stream_id|.  |app_error_code| indicates the reason
 * for this action.
 *
 * The implementation of this callback must return 0 if it succeeds.
 * Returning :macro:`NGHTTP3_ERR_CALLBACK_FAILURE` will return to the
 * caller immediately.  Any values other than 0 is treated as
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 */
typedef int (*nghttp3_reset_stream)(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code,
                                    void *conn_user_data,
                                    void *stream_user_data);

/**
 * @struct
 *
 * :type:`nghttp3_callbacks` holds a set of callback functions.
 */
typedef struct nghttp3_callbacks {
  /**
   * :member:`acked_stream_data` is a callback function which is
   * invoked when data sent on a particular stream have been
   * acknowledged by a remote endpoint.
   */
  nghttp3_acked_stream_data acked_stream_data;
  /**
   * :member:`stream_close` is a callback function which is invoked
   * when a particular stream has closed.
   */
  nghttp3_stream_close stream_close;
  /**
   * :member:`recv_data` is a callback function which is invoked when
   * stream data is received.
   */
  nghttp3_recv_data recv_data;
  /**
   * :member:`deferred_consume` is a callback function which is
   * invoked when the library consumed data for a particular stream
   * which had been blocked for synchronization between streams.
   */
  nghttp3_deferred_consume deferred_consume;
  /**
   * :member:`begin_headers` is a callback function which is invoked
   * when a header block has started on a particular stream.
   */
  nghttp3_begin_headers begin_headers;
  /**
   * :member:`recv_header` is a callback function which is invoked
   * when a single header field is received on a particular stream.
   */
  nghttp3_recv_header recv_header;
  /**
   * :member:`end_headers` is a callback function which is invoked
   * when a header block has ended on a particular stream.
   */
  nghttp3_end_headers end_headers;
  /**
   * :member:`begin_trailers` is a callback function which is invoked
   * when a trailer block has started on a particular stream.
   */
  nghttp3_begin_headers begin_trailers;
  /**
   * :member:`recv_trailer` is a callback function which is invoked
   * when a single trailer field is received on a particular stream.
   */
  nghttp3_recv_header recv_trailer;
  /**
   * :member:`end_trailers` is a callback function which is invoked
   * when a trailer block has ended on a particular stream.
   */
  nghttp3_end_headers end_trailers;
  /**
   * :member:`begin_push_promise` is a callback function which is
   * invoked when a push promise has started on a particular stream.
   */
  nghttp3_begin_push_promise begin_push_promise;
  /**
   * :member:`recv_push_promise` is a callback function which is
   * invoked when a single header field in a push promise is received
   * on a particular stream.
   */
  nghttp3_recv_push_promise recv_push_promise;
  /**
   * :member:`end_push_promise` is a callback function which is
   * invoked when a push promise has ended on a particular stream.
   */
  nghttp3_end_push_promise end_push_promise;
  /**
   * :member:`cancel_push` is a callback function which is invoked
   * when a push promise has been cancelled by a remote endpoint.
   */
  nghttp3_cancel_push cancel_push;
  /**
   * :member:`send_stop_sending` is a callback function which is
   * invoked when the library asks application to send STOP_SENDING to
   * a particular stream.
   */
  nghttp3_send_stop_sending send_stop_sending;
  /**
   * :member:`push_stream` is a callback function which is invoked
   * when a push stream has opened.
   */
  nghttp3_push_stream push_stream;
  /**
   * :member:`end_stream` is a callback function which is invoked when
   * a receiving side of stream has been closed.
   */
  nghttp3_end_stream end_stream;
  /**
   * :member:`reset_stream` is a callback function which is invoked
   * when the library asks application to reset stream (by sending
   * RESET_STREAM).
   */
  nghttp3_reset_stream reset_stream;
} nghttp3_callbacks;

/**
 * @struct
 *
 * :type:`nghttp3_settings` defines HTTP/3 settings.
 */
typedef struct {
  /**
   * :member:`max_field_section_size` specifies the maximum header
   * section (block) size.
   */
  uint64_t max_field_section_size;
  /**
   * :member:`max_pushes` specifies the maximum number of concurrent
   * pushes it accepts from a remote endpoint.
   */
  uint64_t max_pushes;
  /**
   * :member:`qpack_max_table_capacity` is the maximum size of QPACK
   * dynamic table.
   */
  size_t qpack_max_table_capacity;
  /**
   * :member:`qpack_blocked_streams` is the maximum number of streams
   * which can be blocked while they are being decoded.
   */
  size_t qpack_blocked_streams;
} nghttp3_settings;

/**
 * @function
 *
 * `nghttp3_settings_default` fills |settings| with the default
 * values.
 */
NGHTTP3_EXTERN void nghttp3_settings_default(nghttp3_settings *settings);

/**
 * @function
 *
 * `nghttp3_conn_client_new` creates :type:`nghttp3_conn` and
 * initializes it for client use.  The pointer to the object is stored
 * in |*pconn|.
 */
NGHTTP3_EXTERN int nghttp3_conn_client_new(nghttp3_conn **pconn,
                                           const nghttp3_callbacks *callbacks,
                                           const nghttp3_settings *settings,
                                           const nghttp3_mem *mem,
                                           void *conn_user_data);

/**
 * @function
 *
 * `nghttp3_conn_server_new` creates :type:`nghttp3_conn` and
 * initializes it for server use.  The pointer to the object is stored
 * in |*pconn|.
 */
NGHTTP3_EXTERN int nghttp3_conn_server_new(nghttp3_conn **pconn,
                                           const nghttp3_callbacks *callbacks,
                                           const nghttp3_settings *settings,
                                           const nghttp3_mem *mem,
                                           void *conn_user_data);

/**
 * @function
 *
 * `nghttp3_conn_del` frees resources allocated for |conn|.
 */
NGHTTP3_EXTERN void nghttp3_conn_del(nghttp3_conn *conn);

/**
 * @function
 *
 * `nghttp3_conn_bind_control_stream` binds stream denoted by
 * |stream_id| to outgoing unidirectional control stream.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_INVALID_STATE`
 *     Control stream has already corresponding stream ID.
 *
 * TBD
 */
NGHTTP3_EXTERN int nghttp3_conn_bind_control_stream(nghttp3_conn *conn,
                                                    int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_bind_qpack_streams` binds stream denoted by
 * |qenc_stream_id| to outgoing QPACK encoder stream and stream
 * denoted by |qdec_stream_id| to outgoing QPACK encoder stream.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_INVALID_STATE`
 *     QPACK encoder/decoder stream have already corresponding stream
 *     IDs.
 *
 * TBD
 */
NGHTTP3_EXTERN int nghttp3_conn_bind_qpack_streams(nghttp3_conn *conn,
                                                   int64_t qenc_stream_id,
                                                   int64_t qdec_stream_id);

/**
 * @function
 *
 * `nghttp3_conn_read_stream` reads data |src| of length |srclen| on
 * stream identified by |stream_id|.  It returns the number of bytes
 * consumed.  The "consumed" means that application can increase flow
 * control credit (both stream and connection) of underlying QUIC
 * connection by that amount.  It does not include the amount of data
 * carried by DATA frame which contains application data (excluding
 * any control or QPACK unidirectional streams) .  See
 * :type:`nghttp3_recv_data` to handle those bytes.  If |fin| is
 * nonzero, this is the last data from remote endpoint in this stream.
 */
NGHTTP3_EXTERN nghttp3_ssize nghttp3_conn_read_stream(nghttp3_conn *conn,
                                                      int64_t stream_id,
                                                      const uint8_t *src,
                                                      size_t srclen, int fin);

/**
 * @function
 *
 * `nghttp3_conn_writev_stream` stores stream data to send to |vec| of
 * length |veccnt| and returns the number of nghttp3_vec object in
 * which it stored data.  It stores stream ID to |*pstream_id|.  An
 * application has to call `nghttp3_conn_add_write_offset` to inform
 * |conn| of the actual number of bytes that underlying QUIC stack
 * accepted.  |*pfin| will be nonzero if this is the last data to
 * send.  If there is no stream to write data or send fin, this
 * function returns 0, and -1 is assigned to |*pstream_id|.  This
 * function may return 0 and |*pstream_id| is not -1 and |*pfin| is
 * nonzero.  It means 0 length data to |*pstream_id| and it is the
 * last data to the stream.  They must be passed to QUIC stack, and
 * they are accepted, the application has to call
 * `nghttp3_conn_add_write_offset`.
 */
NGHTTP3_EXTERN nghttp3_ssize nghttp3_conn_writev_stream(nghttp3_conn *conn,
                                                        int64_t *pstream_id,
                                                        int *pfin,
                                                        nghttp3_vec *vec,
                                                        size_t veccnt);

/**
 * @function
 *
 * `nghttp3_conn_add_write_offset` tells |conn| the number of bytes
 * |n| for stream denoted by |stream_id| QUIC stack accepted.
 *
 * If stream has no data to send but just sends fin (closing the write
 * side of a stream), the number of bytes sent is 0.  It is important
 * to call this function even if |n| is 0 in this case.  It is safe to
 * call this function if |n| is 0.
 *
 * `nghttp3_conn_writev_stream` must be called before calling this
 * function to get data to send, and those data must be fed into QUIC
 * stack.
 */
NGHTTP3_EXTERN int nghttp3_conn_add_write_offset(nghttp3_conn *conn,
                                                 int64_t stream_id, size_t n);

/**
 * @function
 *
 * `nghttp3_conn_add_ack_offset` tells |conn| the number of bytes |n|
 * for stream denoted by |stream_id| QUIC stack has acknowledged.
 */
NGHTTP3_EXTERN int nghttp3_conn_add_ack_offset(nghttp3_conn *conn,
                                               int64_t stream_id, uint64_t n);

/**
 * @function
 *
 * `nghttp3_conn_block_stream` tells the library that stream
 * identified by |stream_id| is blocked due to QUIC flow control.
 */
NGHTTP3_EXTERN int nghttp3_conn_block_stream(nghttp3_conn *conn,
                                             int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_unblock_stream` tells the library that stream
 * identified by |stream_id| which was blocked by QUIC flow control is
 * unblocked.
 */
NGHTTP3_EXTERN int nghttp3_conn_unblock_stream(nghttp3_conn *conn,
                                               int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_shutdown_stream_write` tells the library that any
 * further write operation to stream identified by |stream_id| is
 * prohibited.  This works like `nghttp3_conn_block_stream`, but it
 * cannot be unblocked by `nghttp3_conn_unblock_stream`.
 */
NGHTTP3_EXTERN int nghttp3_conn_shutdown_stream_write(nghttp3_conn *conn,
                                                      int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_resume_stream` resumes stream identified by
 * |stream_id| which was previously unable to provide data.
 */
NGHTTP3_EXTERN int nghttp3_conn_resume_stream(nghttp3_conn *conn,
                                              int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_close_stream` closes stream identified by
 * |stream_id|.  |app_error_code| is the reason of the closure.
 */
NGHTTP3_EXTERN int nghttp3_conn_close_stream(nghttp3_conn *conn,
                                             int64_t stream_id,
                                             uint64_t app_error_code);

/**
 * @function
 *
 * `nghttp3_conn_reset_stream` must be called if stream identified by
 * |stream_id| is reset by a remote endpoint.  This is required in
 * order to cancel QPACK stream.
 */
NGHTTP3_EXTERN int nghttp3_conn_reset_stream(nghttp3_conn *conn,
                                             int64_t stream_id);

/**
 * @macrosection
 *
 * Data flags
 */

/**
 * :macro:`NGHTTP3_DATA_FLAG_NONE` indicates no flag set.
 */
#define NGHTTP3_DATA_FLAG_NONE 0x00

/**
 * :macro:`NGHTTP3_DATA_FLAG_EOF` indicates that all request or
 * response body has been provided to the library.  It also indicates
 * that sending side of stream is closed unless
 * :macro:`NGHTTP3_DATA_FLAG_NO_END_STREAM` is given at the same time.
 */
#define NGHTTP3_DATA_FLAG_EOF 0x01

/**
 * :macro:`NGHTTP3_DATA_FLAG_NO_END_STREAM` indicates that sending
 * side of stream is not closed even if :macro:`NGHTTP3_DATA_FLAG_EOF`
 * is set.  Usually this flag is used to send trailer fields with
 * `nghttp3_conn_submit_trailers()`.  If
 * `nghttp3_conn_submit_trailers()` has been called, regardless of
 * this flag, the submitted trailer fields are sent.
 */
#define NGHTTP3_DATA_FLAG_NO_END_STREAM 0x02

/**
 * @function
 *
 * `nghttp3_conn_set_max_client_streams_bidi` tells |conn| the
 * cumulative number of bidirectional streams that client can open.
 */
NGHTTP3_EXTERN void
nghttp3_conn_set_max_client_streams_bidi(nghttp3_conn *conn,
                                         uint64_t max_streams);

/**
 * @function
 *
 * `nghttp3_conn_set_max_concurrent_streams` tells |conn| the maximum
 * number of concurrent streams that a remote endpoint can open,
 * including both bidirectional and unidirectional streams which
 * potentially receive QPACK encoded HEADERS frame.  This value is
 * used as a hint to limit the internal resource consumption.
 */
NGHTTP3_EXTERN void
nghttp3_conn_set_max_concurrent_streams(nghttp3_conn *conn,
                                        size_t max_concurrent_streams);

/**
 * @functypedef
 *
 * :type:`nghttp3_read_data_callback` is a callback function invoked
 * when the library asks an application to provide stream data for a
 * stream denoted by |stream_id|.
 *
 * The library provides |vec| of length |veccnt| to the application.
 * The application should fill data and its length to |vec|.  It has
 * to return the number of the filled objects.  The application must
 * retain data until they are safe to free.  It is notified by
 * :type:`nghttp3_acked_stream_data` callback.
 *
 * If this is the last data to send (or there is no data to send
 * because all data have been sent already), set
 * :macro:`NGHTTP3_DATA_FLAG_EOF` to |*pflags|.
 *
 * If the application is unable to provide data temporarily, return
 * :macro:`NGHTTP3_ERR_WOULDBLOCK`.  When it is ready to provide
 * data, call `nghttp3_conn_resume_stream()`.
 *
 * The callback should return the number of objects in |vec| that the
 * application filled if it succeeds, or
 * :macro:`NGHTTP3_ERR_CALLBACK_FAILURE`.
 *
 * TODO Add NGHTTP3_ERR_TEMPORAL_CALLBACK_FAILURE to reset just this
 * stream.
 */
typedef nghttp3_ssize (*nghttp3_read_data_callback)(
    nghttp3_conn *conn, int64_t stream_id, nghttp3_vec *vec, size_t veccnt,
    uint32_t *pflags, void *conn_user_data, void *stream_user_data);

/**
 * @struct
 *
 * :type:`nghttp3_data_reader` specifies the way how to generate
 * request or response body.
 */
typedef struct {
  /**
   * :member:`read_data` is a callback function to generate body.
   */
  nghttp3_read_data_callback read_data;
} nghttp3_data_reader;

/**
 * @function
 *
 * `nghttp3_conn_submit_request` submits HTTP request header fields
 * and body on the stream identified by |stream_id|.  |stream_id| must
 * be a client initiated bidirectional stream.  Only client can submit
 * HTTP request.  |nva| of length |nvlen| specifies HTTP request
 * header fields.  |dr| specifies a request body.  If there is no
 * request body, specify NULL.  If |dr| is NULL, it implies the end of
 * stream.  |stream_user_data| is an opaque pointer attached to the
 * stream.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_request(
    nghttp3_conn *conn, int64_t stream_id, const nghttp3_nv *nva, size_t nvlen,
    const nghttp3_data_reader *dr, void *stream_user_data);

/**
 * @function
 *
 * `nghttp3_conn_submit_push_promise` submits push promise on the
 * stream identified by |stream_id|.  |stream_id| must be a client
 * initiated bidirectional stream.  Only server can submit push
 * promise.  On success, a push ID is assigned to |*ppush_id|.  |nva|
 * of length |nvlen| specifies HTTP request header fields.  In order
 * to submit HTTP response, first call
 * `nghttp3_conn_bind_push_stream()` and then
 * `nghttp3_conn_submit_response()`.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_push_promise(nghttp3_conn *conn,
                                                    int64_t *ppush_id,
                                                    int64_t stream_id,
                                                    const nghttp3_nv *nva,
                                                    size_t nvlen);

/**
 * @function
 *
 * `nghttp3_conn_submit_info` submits HTTP non-final response header
 * fields on the stream identified by |stream_id|.  |nva| of length
 * |nvlen| specifies HTTP response header fields.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_info(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            const nghttp3_nv *nva,
                                            size_t nvlen);

/**
 * @function
 *
 * `nghttp3_conn_submit_response` submits HTTP response header fields
 * and body on the stream identified by |stream_id|.  |nva| of length
 * |nvlen| specifies HTTP response header fields.  |dr| specifies a
 * response body.  If there is no response body, specify NULL.  If
 * |dr| is NULL, it implies the end of stream.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_response(nghttp3_conn *conn,
                                                int64_t stream_id,
                                                const nghttp3_nv *nva,
                                                size_t nvlen,
                                                const nghttp3_data_reader *dr);

/**
 * @function
 *
 * `nghttp3_conn_submit_trailers` submits HTTP trailer fields on the
 * stream identified by |stream_id|.  |nva| of length |nvlen|
 * specifies HTTP trailer fields.  Calling this function implies the
 * end of stream.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_trailers(nghttp3_conn *conn,
                                                int64_t stream_id,
                                                const nghttp3_nv *nva,
                                                size_t nvlen);

/**
 * @function
 *
 * `nghttp3_conn_bind_push_stream` binds the stream identified by
 * |stream_id| to the push identified by |push_id|.  |stream_id| must
 * be a server initiated unidirectional stream.  |push_id| must be
 * obtained from `nghttp3_conn_submit_push_promise()`.  To send
 * response to this push, call `nghttp3_conn_submit_response()`.
 */
NGHTTP3_EXTERN int nghttp3_conn_bind_push_stream(nghttp3_conn *conn,
                                                 int64_t push_id,
                                                 int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_cancel_push` cancels the push identified by
 * |push_id|.
 */
NGHTTP3_EXTERN int nghttp3_conn_cancel_push(nghttp3_conn *conn,
                                            int64_t push_id);

/**
 * @function
 *
 * `nghttp3_conn_submit_shutdown_notice` notifies the other endpoint
 * to stop creating new stream (for server) or push (for client).
 * After a couple of RTTs later, call `nghttp3_conn_shutdown` to start
 * graceful shutdown.
 */
NGHTTP3_EXTERN int nghttp3_conn_submit_shutdown_notice(nghttp3_conn *conn);

/**
 * @function
 *
 * `nghttp3_conn_shutdown` starts graceful shutdown.  It should be
 * called after `nghttp3_conn_submit_shutdown_notice` and a couple of
 * RTT.  After calling this function, the local endpoint starts
 * rejecting new incoming streams (for server) or pushes (for client).
 * The existing streams or pushes are processed normally.
 */
NGHTTP3_EXTERN int nghttp3_conn_shutdown(nghttp3_conn *conn);

/**
 * @function
 *
 * `nghttp3_conn_set_stream_user_data` sets |stream_user_data| to the
 * stream identified by |stream_id|.
 */
NGHTTP3_EXTERN int nghttp3_conn_set_stream_user_data(nghttp3_conn *conn,
                                                     int64_t stream_id,
                                                     void *stream_user_data);

/**
 * @function
 *
 * `nghttp3_conn_get_frame_payload_left` returns the number of bytes
 * left to read current frame payload for a stream denoted by
 * |stream_id|.  If no such stream is found, it returns
 * :macro:`NGHTTP3_ERR_STREAM_NOT_FOUND`.
 */
NGHTTP3_EXTERN int64_t nghttp3_conn_get_frame_payload_left(nghttp3_conn *conn,
                                                           int64_t stream_id);

/**
 * @macro
 *
 * :macro:`NGHTTP3_DEFAULT_URGENCY` is the default urgency level.
 */
#define NGHTTP3_DEFAULT_URGENCY 1

/**
 * @macro
 *
 * :macro:`NGHTTP3_URGENCY_HIGH` is the highest urgency level.
 */
#define NGHTTP3_URGENCY_HIGH 0

/**
 * @macro
 *
 * :macro:`NGHTTP3_URGENCY_LOW` is the lowest urgency level.
 */
#define NGHTTP3_URGENCY_LOW 7

/**
 * @macro
 *
 * :macro:`NGHTTP3_URGENCY_LEVELS` is the number of urgency levels.
 */
#define NGHTTP3_URGENCY_LEVELS (NGHTTP3_URGENCY_LOW + 1)

/**
 * @struct
 *
 * :type:`nghttp3_pri` represents HTTP priority.
 */
typedef struct nghttp3_pri {
  /**
   * :member:`urgency` is the urgency of a stream, it must be in
   * [:macro:`NGHTTP3_URGENCY_HIGH`, :macro:`NGHTTP3_URGENCY_LOW`],
   * inclusive, and 0 is the highest urgency.
   */
  uint32_t urgency;
  /**
   * :member:`inc` indicates that a content can be processed
   * incrementally or not.  If inc is 0, it cannot be processed
   * incrementally.  If inc is 1, it can be processed incrementally.
   * Other value is not permitted.
   */
  int inc;
} nghttp3_pri;

/**
 * @function
 *
 * `nghttp3_conn_get_stream_priority` stores stream priority of a
 * stream denoted by |stream_id| into |*dest|.  Only server can use
 * this function.
 *
 * This function must not be called if |conn| is initialized as
 * client.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_STREAM_NOT_FOUND`
 *     Stream not found.
 */
NGHTTP3_EXTERN int nghttp3_conn_get_stream_priority(nghttp3_conn *conn,
                                                    nghttp3_pri *dest,
                                                    int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_conn_set_stream_priority` updates stream priority of a
 * stream denoted by |stream_id| by the value pointed by |pri|.
 *
 * This function must not be called if |conn| is initialized as
 * client.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_STREAM_NOT_FOUND`
 *     Stream not found.
 * :macro:`NGHTTP3_ERR_NOMEM`
 *     Out of memory.
 */
NGHTTP3_EXTERN int nghttp3_conn_set_stream_priority(nghttp3_conn *conn,
                                                    int64_t stream_id,
                                                    const nghttp3_pri *pri);

/**
 * @function
 *
 * `nghttp3_conn_is_remote_qpack_encoder_stream` returns nonzero if a
 * stream denoted by |stream_id| is QPACK encoder stream of a remote
 * endpoint.
 */
NGHTTP3_EXTERN int
nghttp3_conn_is_remote_qpack_encoder_stream(nghttp3_conn *conn,
                                            int64_t stream_id);

/**
 * @function
 *
 * `nghttp3_vec_len` returns the sum of length in |vec| of |cnt|
 * elements.
 */
NGHTTP3_EXTERN size_t nghttp3_vec_len(const nghttp3_vec *vec, size_t cnt);

/**
 * @function
 *
 * `nghttp3_check_header_name` returns nonzero if HTTP header field
 * name |name| of length |len| is valid according to
 * http://tools.ietf.org/html/rfc7230#section-3.2
 *
 * Because this is a header field name in HTTP/3, the upper cased
 * alphabet is treated as error.
 */
NGHTTP3_EXTERN int nghttp3_check_header_name(const uint8_t *name, size_t len);

/**
 * @function
 *
 * `nghttp3_check_header_value` returns nonzero if HTTP header field
 * value |value| of length |len| is valid according to
 * http://tools.ietf.org/html/rfc7230#section-3.2
 */
NGHTTP3_EXTERN int nghttp3_check_header_value(const uint8_t *value, size_t len);

/**
 * @function
 *
 * `nghttp3_http_parse_priority` parses priority HTTP header field
 * stored in the buffer pointed by |value| of length |len|.  If it
 * successfully processed header field value, it stores the result
 * into |*dest|.  This function just overwrites what it sees in the
 * header field value and does not initialize any field in |*dest|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :macro:`NGHTTP3_ERR_INVALID_ARGUMENT`
 *     The function could not parse the provided value.
 */
NGHTTP3_EXTERN int nghttp3_http_parse_priority(nghttp3_pri *dest,
                                               const uint8_t *value,
                                               size_t len);

/**
 * @macro
 *
 * :macro:`NGHTTP3_VERSION_AGE` is the age of :type:`nghttp3_info`.
 */
#define NGHTTP3_VERSION_AGE 1

/**
 * @struct
 *
 * :type:`nghttp3_info` is what `nghttp3_version()` returns.  It holds
 * information about the particular nghttp3 version.
 */
typedef struct {
  /**
   * :member:`age` is the age of this struct.  This instance of
   * nghttp3 sets it to :macro:`NGHTTP3_VERSION_AGE` but a future
   * version may bump it and add more struct fields at the bottom
   */
  int age;
  /**
   * :member:`version_num` is the :macro:`NGHTTP3_VERSION_NUM` number
   * (since age ==1)
   */
  int version_num;
  /**
   * :member:`version_str` points to the :macro:`NGHTTP3_VERSION`
   * string (since age ==1)
   */
  const char *version_str;
  /* -------- the above fields all exist when age == 1 */
} nghttp3_info;

/**
 * @function
 *
 * `nghttp3_version` returns a pointer to a :type:`nghttp3_info`
 * struct with version information about the run-time library in use.
 * The |least_version| argument can be set to a 24 bit numerical value
 * for the least accepted version number and if the condition is not
 * met, this function will return a ``NULL``.  Pass in 0 to skip the
 * version checking.
 */
NGHTTP3_EXTERN nghttp3_info *nghttp3_version(int least_version);

#ifdef __cplusplus
}
#endif

#endif /* NGHTTP3_H */
