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
#ifndef NGHTTP3_CONV_H
#define NGHTTP3_CONV_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */

#ifdef HAVE_BYTESWAP_H
#  include <byteswap.h>
#endif /* HAVE_BYTESWAP_H */

#ifdef HAVE_ENDIAN_H
#  include <endian.h>
#endif /* HAVE_ENDIAN_H */

#ifdef HAVE_SYS_ENDIAN_H
#  include <sys/endian.h>
#endif /* HAVE_SYS_ENDIAN_H */

#include <nghttp3/nghttp3.h>

#if defined(HAVE_BSWAP_64) ||                                                  \
    (defined(HAVE_DECL_BSWAP_64) && HAVE_DECL_BSWAP_64 > 0)
#  define nghttp3_bswap64 bswap_64
#else /* !HAVE_BSWAP_64 */
#  define nghttp3_bswap64(N)                                                   \
    ((uint64_t)(ntohl((uint32_t)(N))) << 32 | ntohl((uint32_t)((N) >> 32)))
#endif /* !HAVE_BSWAP_64 */

#if defined(HAVE_BE64TOH) ||                                                   \
    (defined(HAVE_DECL_BE64TOH) && HAVE_DECL_BE64TOH > 0)
#  define nghttp3_ntohl64(N) be64toh(N)
#  define nghttp3_htonl64(N) htobe64(N)
#else /* !HAVE_BE64TOH */
#  if defined(WORDS_BIGENDIAN)
#    define nghttp3_ntohl64(N) (N)
#    define nghttp3_htonl64(N) (N)
#  else /* !WORDS_BIGENDIAN */
#    define nghttp3_ntohl64(N) nghttp3_bswap64(N)
#    define nghttp3_htonl64(N) nghttp3_bswap64(N)
#  endif /* !WORDS_BIGENDIAN */
#endif   /* !HAVE_BE64TOH */

#if defined(WIN32)
/* Windows requires ws2_32 library for ntonl family of functions.  We
   define inline functions for those functions so that we don't have
   dependency on that lib. */

#  ifdef _MSC_VER
#    define STIN static __inline
#  else
#    define STIN static inline
#  endif

STIN uint32_t htonl(uint32_t hostlong) {
  uint32_t res;
  unsigned char *p = (unsigned char *)&res;
  *p++ = hostlong >> 24;
  *p++ = (hostlong >> 16) & 0xffu;
  *p++ = (hostlong >> 8) & 0xffu;
  *p = hostlong & 0xffu;
  return res;
}

STIN uint16_t htons(uint16_t hostshort) {
  uint16_t res;
  unsigned char *p = (unsigned char *)&res;
  *p++ = hostshort >> 8;
  *p = hostshort & 0xffu;
  return res;
}

STIN uint32_t ntohl(uint32_t netlong) {
  uint32_t res;
  unsigned char *p = (unsigned char *)&netlong;
  res = *p++ << 24;
  res += *p++ << 16;
  res += *p++ << 8;
  res += *p;
  return res;
}

STIN uint16_t ntohs(uint16_t netshort) {
  uint16_t res;
  unsigned char *p = (unsigned char *)&netshort;
  res = *p++ << 8;
  res += *p;
  return res;
}

#endif /* WIN32 */

/*
 * nghttp3_get_varint reads variable-length integer from |p|, and
 * returns it in host byte order.  The number of bytes read is stored
 * in |*plen|.
 */
int64_t nghttp3_get_varint(size_t *plen, const uint8_t *p);

/*
 * nghttp3_get_varint_fb reads first byte of encoded variable-length
 * integer from |p|.
 */
int64_t nghttp3_get_varint_fb(const uint8_t *p);

/*
 * nghttp3_get_varint_len returns the required number of bytes to read
 * variable-length integer starting at |p|.
 */
size_t nghttp3_get_varint_len(const uint8_t *p);

/*
 * nghttp3_put_uint64be writes |n| in host byte order in |p| in
 * network byte order.  It returns the one beyond of the last written
 * position.
 */
uint8_t *nghttp3_put_uint64be(uint8_t *p, uint64_t n);

/*
 * nghttp3_put_uint48be writes |n| in host byte order in |p| in
 * network byte order.  It writes only least significant 48 bits.  It
 * returns the one beyond of the last written position.
 */
uint8_t *nghttp3_put_uint48be(uint8_t *p, uint64_t n);

/*
 * nghttp3_put_uint32be writes |n| in host byte order in |p| in
 * network byte order.  It returns the one beyond of the last written
 * position.
 */
uint8_t *nghttp3_put_uint32be(uint8_t *p, uint32_t n);

/*
 * nghttp3_put_uint24be writes |n| in host byte order in |p| in
 * network byte order.  It writes only least significant 24 bits.  It
 * returns the one beyond of the last written position.
 */
uint8_t *nghttp3_put_uint24be(uint8_t *p, uint32_t n);

/*
 * nghttp3_put_uint16be writes |n| in host byte order in |p| in
 * network byte order.  It returns the one beyond of the last written
 * position.
 */
uint8_t *nghttp3_put_uint16be(uint8_t *p, uint16_t n);

/*
 * nghttp3_put_varint writes |n| in |p| using variable-length integer
 * encoding.  It returns the one beyond of the last written position.
 */
uint8_t *nghttp3_put_varint(uint8_t *p, int64_t n);

/*
 * nghttp3_put_varint_len returns the required number of bytes to
 * encode |n|.
 */
size_t nghttp3_put_varint_len(int64_t n);

/*
 * nghttp3_ord_stream_id returns the ordinal number of |stream_id|.
 */
uint64_t nghttp3_ord_stream_id(int64_t stream_id);

/*
 * NGHTTP3_PRI_INC_MASK is a bit mask to retrieve incremental bit from
 * a value produced by nghttp3_pri_to_uint8.
 */
#define NGHTTP3_PRI_INC_MASK (1 << 7)

/*
 * nghttp3_pri_to_uint8 encodes |pri| into uint8_t variable.
 */
uint8_t nghttp3_pri_to_uint8(const nghttp3_pri *pri);

/*
 * nghttp3_pri_uint8_urgency extracts urgency from |PRI| which is
 * supposed to be constructed by nghttp3_pri_to_uint8.
 */
#define nghttp3_pri_uint8_urgency(PRI)                                         \
  ((uint32_t)((PRI) & ~NGHTTP3_PRI_INC_MASK))

/*
 * nghttp3_pri_uint8_inc extracts inc from |PRI| which is supposed to
 * be constructed by nghttp3_pri_to_uint8.
 */
#define nghttp3_pri_uint8_inc(PRI) (((PRI)&NGHTTP3_PRI_INC_MASK) != 0)

#endif /* NGHTTP3_CONV_H */
