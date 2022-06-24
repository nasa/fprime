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
#include "ngtcp2_pkt.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ngtcp2_conv.h"
#include "ngtcp2_str.h"
#include "ngtcp2_macro.h"
#include "ngtcp2_cid.h"
#include "ngtcp2_mem.h"
#include "ngtcp2_vec.h"

int ngtcp2_pkt_chain_new(ngtcp2_pkt_chain **ppc, const ngtcp2_path *path,
                         const ngtcp2_pkt_info *pi, const uint8_t *pkt,
                         size_t pktlen, size_t dgramlen, ngtcp2_tstamp ts,
                         const ngtcp2_mem *mem) {
  *ppc = ngtcp2_mem_malloc(mem, sizeof(ngtcp2_pkt_chain) + pktlen);
  if (*ppc == NULL) {
    return NGTCP2_ERR_NOMEM;
  }

  ngtcp2_path_storage_init2(&(*ppc)->path, path);
  (*ppc)->pi = *pi;
  (*ppc)->next = NULL;
  (*ppc)->pkt = (uint8_t *)(*ppc) + sizeof(ngtcp2_pkt_chain);
  (*ppc)->pktlen = pktlen;
  (*ppc)->dgramlen = dgramlen;
  (*ppc)->ts = ts;

  memcpy((*ppc)->pkt, pkt, pktlen);

  return 0;
}

void ngtcp2_pkt_chain_del(ngtcp2_pkt_chain *pc, const ngtcp2_mem *mem) {
  ngtcp2_mem_free(mem, pc);
}

int ngtcp2_pkt_decode_version_cid(uint32_t *pversion, const uint8_t **pdcid,
                                  size_t *pdcidlen, const uint8_t **pscid,
                                  size_t *pscidlen, const uint8_t *data,
                                  size_t datalen, size_t short_dcidlen) {
  size_t len;
  uint32_t version;
  size_t dcidlen, scidlen;

  assert(datalen);

  if (data[0] & NGTCP2_HEADER_FORM_BIT) {
    /* 1 byte (Header Form, Fixed Bit, Long Packet Type, Type-Specific bits)
     * 4 bytes Version
     * 1 byte DCID Length
     * 1 byte SCID Length
     */
    len = 1 + 4 + 1 + 1;
    if (datalen < len) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }

    dcidlen = data[5];
    len += dcidlen;
    if (datalen < len) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }
    scidlen = data[5 + 1 + dcidlen];
    len += scidlen;
    if (datalen < len) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }

    version = ngtcp2_get_uint32(&data[1]);

    if ((version == 0 || version == NGTCP2_PROTO_VER_V1 ||
         (NGTCP2_PROTO_VER_DRAFT_MIN <= version &&
          version <= NGTCP2_PROTO_VER_DRAFT_MAX)) &&
        (dcidlen > NGTCP2_MAX_CIDLEN || scidlen > NGTCP2_MAX_CIDLEN)) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }

    *pversion = version;
    *pdcid = &data[6];
    *pdcidlen = dcidlen;
    *pscid = &data[6 + dcidlen + 1];
    *pscidlen = scidlen;

    if (version && version != NGTCP2_PROTO_VER_V1 &&
        (version < NGTCP2_PROTO_VER_DRAFT_MIN ||
         NGTCP2_PROTO_VER_DRAFT_MAX < version)) {
      return 1;
    }
    return 0;
  }

  assert(short_dcidlen <= NGTCP2_MAX_CIDLEN);

  len = 1 + short_dcidlen;
  if (datalen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  *pversion = 0;
  *pdcid = &data[1];
  *pdcidlen = short_dcidlen;
  *pscid = NULL;
  *pscidlen = 0;

  return 0;
}

void ngtcp2_pkt_hd_init(ngtcp2_pkt_hd *hd, uint8_t flags, uint8_t type,
                        const ngtcp2_cid *dcid, const ngtcp2_cid *scid,
                        int64_t pkt_num, size_t pkt_numlen, uint32_t version,
                        size_t len) {
  hd->flags = flags;
  hd->type = type;
  if (dcid) {
    hd->dcid = *dcid;
  } else {
    ngtcp2_cid_zero(&hd->dcid);
  }
  if (scid) {
    hd->scid = *scid;
  } else {
    ngtcp2_cid_zero(&hd->scid);
  }
  hd->pkt_num = pkt_num;
  hd->token.base = NULL;
  hd->token.len = 0;
  hd->pkt_numlen = pkt_numlen;
  hd->version = version;
  hd->len = len;
}

static int has_mask(uint8_t b, uint8_t mask) { return (b & mask) == mask; }

ngtcp2_ssize ngtcp2_pkt_decode_hd_long(ngtcp2_pkt_hd *dest, const uint8_t *pkt,
                                       size_t pktlen) {
  uint8_t type;
  uint32_t version;
  size_t dcil, scil;
  const uint8_t *p;
  size_t len = 0;
  size_t n;
  size_t ntokenlen = 0;
  const uint8_t *token = NULL;
  size_t tokenlen = 0;
  uint64_t vi;

  if (pktlen < 5) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  if (!(pkt[0] & NGTCP2_HEADER_FORM_BIT)) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  version = ngtcp2_get_uint32(&pkt[1]);

  if (version == 0) {
    type = NGTCP2_PKT_VERSION_NEGOTIATION;
    /* This must be Version Negotiation packet which lacks packet
       number and payload length fields. */
    len = 5 + 2;
  } else {
    if (!(pkt[0] & NGTCP2_FIXED_BIT_MASK)) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }

    type = ngtcp2_pkt_get_type_long(pkt[0]);
    switch (type) {
    case NGTCP2_PKT_INITIAL:
      len = 1 /* Token Length */ + NGTCP2_MIN_LONG_HEADERLEN -
            1; /* Cut packet number field */
      break;
    case NGTCP2_PKT_RETRY:
      /* Retry packet does not have packet number and length fields */
      len = 5 + 2;
      break;
    case NGTCP2_PKT_HANDSHAKE:
    case NGTCP2_PKT_0RTT:
      len = NGTCP2_MIN_LONG_HEADERLEN - 1; /* Cut packet number field */
      break;
    default:
      /* Unreachable */
      assert(0);
    }
  }

  if (pktlen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  p = &pkt[5];
  dcil = *p;
  if (dcil > NGTCP2_MAX_CIDLEN) {
    /* QUIC v1 implementation never expect to receive CID length more
       than NGTCP2_MAX_CIDLEN. */
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }
  len += dcil;

  if (pktlen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  p += 1 + dcil;
  scil = *p;
  if (scil > NGTCP2_MAX_CIDLEN) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }
  len += scil;

  if (pktlen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  p += 1 + scil;

  if (type == NGTCP2_PKT_INITIAL) {
    /* Token Length */
    ntokenlen = ngtcp2_get_varint_len(p);
    len += ntokenlen - 1;

    if (pktlen < len) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }

    vi = ngtcp2_get_varint(&ntokenlen, p);
    if (pktlen - len < vi) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }
    tokenlen = (size_t)vi;
    len += tokenlen;

    p += ntokenlen;

    if (tokenlen) {
      token = p;
    }

    p += tokenlen;
  }

  switch (type) {
  case NGTCP2_PKT_VERSION_NEGOTIATION:
  case NGTCP2_PKT_RETRY:
    break;
  default:
    /* Length */
    n = ngtcp2_get_varint_len(p);
    len += n - 1;

    if (pktlen < len) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }
  }

  dest->flags = NGTCP2_PKT_FLAG_LONG_FORM;
  dest->type = type;
  dest->version = version;
  dest->pkt_num = 0;
  dest->pkt_numlen = 0;

  p = &pkt[6];
  ngtcp2_cid_init(&dest->dcid, p, dcil);
  p += dcil + 1;
  ngtcp2_cid_init(&dest->scid, p, scil);
  p += scil;

  dest->token.base = (uint8_t *)token;
  dest->token.len = tokenlen;
  p += ntokenlen + tokenlen;

  switch (type) {
  case NGTCP2_PKT_VERSION_NEGOTIATION:
  case NGTCP2_PKT_RETRY:
    dest->len = 0;
    break;
  default:
    vi = ngtcp2_get_varint(&n, p);
    if (vi > SIZE_MAX) {
      return NGTCP2_ERR_INVALID_ARGUMENT;
    }
    dest->len = (size_t)vi;
    p += n;
  }

  assert((size_t)(p - pkt) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_hd_short(ngtcp2_pkt_hd *dest, const uint8_t *pkt,
                                        size_t pktlen, size_t dcidlen) {
  size_t len = 1 + dcidlen;
  const uint8_t *p = pkt;

  assert(dcidlen <= NGTCP2_MAX_CIDLEN);

  if (pktlen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  if ((pkt[0] & NGTCP2_HEADER_FORM_BIT) ||
      (pkt[0] & NGTCP2_FIXED_BIT_MASK) == 0) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  p = &pkt[1];

  dest->type = NGTCP2_PKT_SHORT;

  ngtcp2_cid_init(&dest->dcid, p, dcidlen);
  p += dcidlen;

  /* Set 0 to SCID so that we don't accidentally reference it and gets
     garbage. */
  ngtcp2_cid_zero(&dest->scid);

  dest->flags = NGTCP2_PKT_FLAG_NONE;
  dest->version = 0;
  dest->len = 0;
  dest->pkt_num = 0;
  dest->pkt_numlen = 0;

  assert((size_t)(p - pkt) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_hd_long(uint8_t *out, size_t outlen,
                                       const ngtcp2_pkt_hd *hd) {
  uint8_t *p;
  size_t len = NGTCP2_MIN_LONG_HEADERLEN + hd->dcid.datalen + hd->scid.datalen -
               2; /* NGTCP2_MIN_LONG_HEADERLEN includes 1 byte for
                     len and 1 byte for packet number. */

  if (hd->type != NGTCP2_PKT_RETRY) {
    len += 2 /* Length */ + hd->pkt_numlen;
  }

  if (hd->type == NGTCP2_PKT_INITIAL) {
    len += ngtcp2_put_varint_len(hd->token.len) + hd->token.len;
  }

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = (uint8_t)(NGTCP2_HEADER_FORM_BIT | NGTCP2_FIXED_BIT_MASK |
                   (hd->type << 4) | (uint8_t)(hd->pkt_numlen - 1));
  p = ngtcp2_put_uint32be(p, hd->version);
  *p++ = (uint8_t)hd->dcid.datalen;
  if (hd->dcid.datalen) {
    p = ngtcp2_cpymem(p, hd->dcid.data, hd->dcid.datalen);
  }
  *p++ = (uint8_t)hd->scid.datalen;
  if (hd->scid.datalen) {
    p = ngtcp2_cpymem(p, hd->scid.data, hd->scid.datalen);
  }

  if (hd->type == NGTCP2_PKT_INITIAL) {
    p = ngtcp2_put_varint(p, hd->token.len);
    if (hd->token.len) {
      p = ngtcp2_cpymem(p, hd->token.base, hd->token.len);
    }
  }

  if (hd->type != NGTCP2_PKT_RETRY) {
    p = ngtcp2_put_varint14(p, (uint16_t)hd->len);
    p = ngtcp2_put_pkt_num(p, hd->pkt_num, hd->pkt_numlen);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_hd_short(uint8_t *out, size_t outlen,
                                        const ngtcp2_pkt_hd *hd) {
  uint8_t *p;
  size_t len = 1 + hd->dcid.datalen + hd->pkt_numlen;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p = NGTCP2_FIXED_BIT_MASK | (uint8_t)(hd->pkt_numlen - 1);
  if (hd->flags & NGTCP2_PKT_FLAG_KEY_PHASE) {
    *p |= NGTCP2_SHORT_KEY_PHASE_BIT;
  }

  ++p;

  if (hd->dcid.datalen) {
    p = ngtcp2_cpymem(p, hd->dcid.data, hd->dcid.datalen);
  }

  p = ngtcp2_put_pkt_num(p, hd->pkt_num, hd->pkt_numlen);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_frame(ngtcp2_frame *dest, const uint8_t *payload,
                                     size_t payloadlen) {
  uint8_t type;

  if (payloadlen == 0) {
    return 0;
  }

  type = payload[0];

  switch (type) {
  case NGTCP2_FRAME_PADDING:
    return (ngtcp2_ssize)ngtcp2_pkt_decode_padding_frame(&dest->padding,
                                                         payload, payloadlen);
  case NGTCP2_FRAME_RESET_STREAM:
    return ngtcp2_pkt_decode_reset_stream_frame(&dest->reset_stream, payload,
                                                payloadlen);
  case NGTCP2_FRAME_CONNECTION_CLOSE:
  case NGTCP2_FRAME_CONNECTION_CLOSE_APP:
    return ngtcp2_pkt_decode_connection_close_frame(&dest->connection_close,
                                                    payload, payloadlen);
  case NGTCP2_FRAME_MAX_DATA:
    return ngtcp2_pkt_decode_max_data_frame(&dest->max_data, payload,
                                            payloadlen);
  case NGTCP2_FRAME_MAX_STREAM_DATA:
    return ngtcp2_pkt_decode_max_stream_data_frame(&dest->max_stream_data,
                                                   payload, payloadlen);
  case NGTCP2_FRAME_MAX_STREAMS_BIDI:
  case NGTCP2_FRAME_MAX_STREAMS_UNI:
    return ngtcp2_pkt_decode_max_streams_frame(&dest->max_streams, payload,
                                               payloadlen);
  case NGTCP2_FRAME_PING:
    return ngtcp2_pkt_decode_ping_frame(&dest->ping, payload, payloadlen);
  case NGTCP2_FRAME_DATA_BLOCKED:
    return ngtcp2_pkt_decode_data_blocked_frame(&dest->data_blocked, payload,
                                                payloadlen);
  case NGTCP2_FRAME_STREAM_DATA_BLOCKED:
    return ngtcp2_pkt_decode_stream_data_blocked_frame(
        &dest->stream_data_blocked, payload, payloadlen);
  case NGTCP2_FRAME_STREAMS_BLOCKED_BIDI:
  case NGTCP2_FRAME_STREAMS_BLOCKED_UNI:
    return ngtcp2_pkt_decode_streams_blocked_frame(&dest->streams_blocked,
                                                   payload, payloadlen);
  case NGTCP2_FRAME_NEW_CONNECTION_ID:
    return ngtcp2_pkt_decode_new_connection_id_frame(&dest->new_connection_id,
                                                     payload, payloadlen);
  case NGTCP2_FRAME_STOP_SENDING:
    return ngtcp2_pkt_decode_stop_sending_frame(&dest->stop_sending, payload,
                                                payloadlen);
  case NGTCP2_FRAME_ACK:
  case NGTCP2_FRAME_ACK_ECN:
    return ngtcp2_pkt_decode_ack_frame(&dest->ack, payload, payloadlen);
  case NGTCP2_FRAME_PATH_CHALLENGE:
    return ngtcp2_pkt_decode_path_challenge_frame(&dest->path_challenge,
                                                  payload, payloadlen);
  case NGTCP2_FRAME_PATH_RESPONSE:
    return ngtcp2_pkt_decode_path_response_frame(&dest->path_response, payload,
                                                 payloadlen);
  case NGTCP2_FRAME_CRYPTO:
    return ngtcp2_pkt_decode_crypto_frame(&dest->crypto, payload, payloadlen);
  case NGTCP2_FRAME_NEW_TOKEN:
    return ngtcp2_pkt_decode_new_token_frame(&dest->new_token, payload,
                                             payloadlen);
  case NGTCP2_FRAME_RETIRE_CONNECTION_ID:
    return ngtcp2_pkt_decode_retire_connection_id_frame(
        &dest->retire_connection_id, payload, payloadlen);
  case NGTCP2_FRAME_HANDSHAKE_DONE:
    return ngtcp2_pkt_decode_handshake_done_frame(&dest->handshake_done,
                                                  payload, payloadlen);
  case NGTCP2_FRAME_DATAGRAM:
  case NGTCP2_FRAME_DATAGRAM_LEN:
    return ngtcp2_pkt_decode_datagram_frame(&dest->datagram, payload,
                                            payloadlen);
  default:
    if (has_mask(type, NGTCP2_FRAME_STREAM)) {
      return ngtcp2_pkt_decode_stream_frame(&dest->stream, payload, payloadlen);
    }
    return NGTCP2_ERR_FRAME_ENCODING;
  }
}

ngtcp2_ssize ngtcp2_pkt_decode_stream_frame(ngtcp2_stream *dest,
                                            const uint8_t *payload,
                                            size_t payloadlen) {
  uint8_t type;
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t datalen;
  size_t ndatalen = 0;
  size_t n;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  type = payload[0];

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  if (type & NGTCP2_STREAM_OFF_BIT) {
    ++len;
    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    n = ngtcp2_get_varint_len(p);
    len += n - 1;

    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    p += n;
  }

  if (type & NGTCP2_STREAM_LEN_BIT) {
    ++len;
    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    ndatalen = ngtcp2_get_varint_len(p);
    len += ndatalen - 1;

    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    vi = ngtcp2_get_varint(&ndatalen, p);
    if (payloadlen - len < vi) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }
    datalen = (size_t)vi;
    len += datalen;
  } else {
    len = payloadlen;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_STREAM;
  dest->flags = (uint8_t)(type & ~NGTCP2_FRAME_STREAM);
  dest->fin = (type & NGTCP2_STREAM_FIN_BIT) != 0;
  dest->stream_id = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;

  if (type & NGTCP2_STREAM_OFF_BIT) {
    dest->offset = ngtcp2_get_varint(&n, p);
    p += n;
  } else {
    dest->offset = 0;
  }

  if (type & NGTCP2_STREAM_LEN_BIT) {
    p += ndatalen;
  } else {
    datalen = payloadlen - (size_t)(p - payload);
  }

  if (datalen) {
    dest->data[0].len = datalen;
    dest->data[0].base = (uint8_t *)p;
    dest->datacnt = 1;
    p += datalen;
  } else {
    dest->datacnt = 0;
  }

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_ack_frame(ngtcp2_ack *dest,
                                         const uint8_t *payload,
                                         size_t payloadlen) {
  size_t num_blks, max_num_blks;
  size_t nnum_blks;
  size_t len = 1 + 1 + 1 + 1 + 1;
  const uint8_t *p;
  size_t i, j;
  ngtcp2_ack_blk *blk;
  size_t n;
  uint8_t type;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  type = payload[0];

  p = payload + 1;

  /* Largest Acknowledged */
  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  /* ACK Delay */
  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  /* ACK Block Count */
  nnum_blks = ngtcp2_get_varint_len(p);
  len += nnum_blks - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  vi = ngtcp2_get_varint(&nnum_blks, p);
  if (vi > SIZE_MAX / (1 + 1) || payloadlen - len < vi * (1 + 1)) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  num_blks = (size_t)vi;
  len += num_blks * (1 + 1);

  p += nnum_blks;

  /* First ACK Block */
  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  for (i = 0; i < num_blks; ++i) {
    /* Gap, and Additional ACK Block */
    for (j = 0; j < 2; ++j) {
      n = ngtcp2_get_varint_len(p);
      len += n - 1;

      if (payloadlen < len) {
        return NGTCP2_ERR_FRAME_ENCODING;
      }

      p += n;
    }
  }

  if (type == NGTCP2_FRAME_ACK_ECN) {
    len += 3;
    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    for (i = 0; i < 3; ++i) {
      n = ngtcp2_get_varint_len(p);
      len += n - 1;

      if (payloadlen < len) {
        return NGTCP2_ERR_FRAME_ENCODING;
      }

      p += n;
    }
  }

  /* TODO We might not decode all blocks.  It could be very large. */
  max_num_blks = ngtcp2_min(NGTCP2_MAX_ACK_BLKS, num_blks);

  p = payload + 1;

  dest->type = type;
  dest->largest_ack = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;
  dest->ack_delay = ngtcp2_get_varint(&n, p);
  /* This value will be assigned in the upper layer. */
  dest->ack_delay_unscaled = 0;
  p += n;
  dest->num_blks = max_num_blks;
  p += nnum_blks;
  dest->first_ack_blklen = ngtcp2_get_varint(&n, p);
  p += n;

  for (i = 0; i < max_num_blks; ++i) {
    blk = &dest->blks[i];
    blk->gap = ngtcp2_get_varint(&n, p);
    p += n;
    blk->blklen = ngtcp2_get_varint(&n, p);
    p += n;
  }
  for (i = max_num_blks; i < num_blks; ++i) {
    p += ngtcp2_get_varint_len(p);
    p += ngtcp2_get_varint_len(p);
  }

  if (type == NGTCP2_FRAME_ACK_ECN) {
    dest->ecn.ect0 = ngtcp2_get_varint(&n, p);
    p += n;

    dest->ecn.ect1 = ngtcp2_get_varint(&n, p);
    p += n;

    dest->ecn.ce = ngtcp2_get_varint(&n, p);
    p += n;
  }

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

size_t ngtcp2_pkt_decode_padding_frame(ngtcp2_padding *dest,
                                       const uint8_t *payload,
                                       size_t payloadlen) {
  const uint8_t *p, *ep;

  assert(payloadlen > 0);

  p = payload + 1;
  ep = payload + payloadlen;

  for (; p != ep && *p == NGTCP2_FRAME_PADDING; ++p)
    ;

  dest->type = NGTCP2_FRAME_PADDING;
  dest->len = (size_t)(p - payload);

  return dest->len;
}

ngtcp2_ssize ngtcp2_pkt_decode_reset_stream_frame(ngtcp2_reset_stream *dest,
                                                  const uint8_t *payload,
                                                  size_t payloadlen) {
  size_t len = 1 + 1 + 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }
  p += n;
  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }
  p += n;
  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_RESET_STREAM;
  dest->stream_id = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;
  dest->app_error_code = ngtcp2_get_varint(&n, p);
  p += n;
  dest->final_size = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_connection_close_frame(
    ngtcp2_connection_close *dest, const uint8_t *payload, size_t payloadlen) {
  size_t len = 1 + 1 + 1;
  const uint8_t *p;
  size_t reasonlen;
  size_t nreasonlen;
  size_t n;
  uint8_t type;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  type = payload[0];

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  if (type == NGTCP2_FRAME_CONNECTION_CLOSE) {
    ++len;

    n = ngtcp2_get_varint_len(p);
    len += n - 1;
    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    p += n;
  }

  nreasonlen = ngtcp2_get_varint_len(p);
  len += nreasonlen - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  vi = ngtcp2_get_varint(&nreasonlen, p);
  if (payloadlen - len < vi) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }
  reasonlen = (size_t)vi;
  len += reasonlen;

  p = payload + 1;

  dest->type = type;
  dest->error_code = ngtcp2_get_varint(&n, p);
  p += n;
  if (type == NGTCP2_FRAME_CONNECTION_CLOSE) {
    dest->frame_type = ngtcp2_get_varint(&n, p);
    p += n;
  } else {
    dest->frame_type = 0;
  }
  dest->reasonlen = reasonlen;
  p += nreasonlen;
  if (reasonlen == 0) {
    dest->reason = NULL;
  } else {
    dest->reason = (uint8_t *)p;
    p += reasonlen;
  }

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_max_data_frame(ngtcp2_max_data *dest,
                                              const uint8_t *payload,
                                              size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  dest->type = NGTCP2_FRAME_MAX_DATA;
  dest->max_data = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_max_stream_data_frame(
    ngtcp2_max_stream_data *dest, const uint8_t *payload, size_t payloadlen) {
  size_t len = 1 + 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_MAX_STREAM_DATA;
  dest->stream_id = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;
  dest->max_stream_data = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_max_streams_frame(ngtcp2_max_streams *dest,
                                                 const uint8_t *payload,
                                                 size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  dest->type = payload[0];
  dest->max_streams = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_ping_frame(ngtcp2_ping *dest,
                                          const uint8_t *payload,
                                          size_t payloadlen) {
  (void)payload;
  (void)payloadlen;

  dest->type = NGTCP2_FRAME_PING;
  return 1;
}

ngtcp2_ssize ngtcp2_pkt_decode_data_blocked_frame(ngtcp2_data_blocked *dest,
                                                  const uint8_t *payload,
                                                  size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  dest->type = NGTCP2_FRAME_DATA_BLOCKED;
  dest->offset = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_decode_stream_data_blocked_frame(ngtcp2_stream_data_blocked *dest,
                                            const uint8_t *payload,
                                            size_t payloadlen) {
  size_t len = 1 + 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_STREAM_DATA_BLOCKED;
  dest->stream_id = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;
  dest->offset = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_streams_blocked_frame(
    ngtcp2_streams_blocked *dest, const uint8_t *payload, size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  dest->type = payload[0];
  dest->max_streams = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_new_connection_id_frame(
    ngtcp2_new_connection_id *dest, const uint8_t *payload, size_t payloadlen) {
  size_t len = 1 + 1 + 1 + 1 + 16;
  const uint8_t *p;
  size_t n;
  size_t cil;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  cil = *p;
  if (cil < NGTCP2_MIN_CIDLEN || cil > NGTCP2_MAX_CIDLEN) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  len += cil;
  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_NEW_CONNECTION_ID;
  dest->seq = ngtcp2_get_varint(&n, p);
  p += n;
  dest->retire_prior_to = ngtcp2_get_varint(&n, p);
  p += n + 1;
  ngtcp2_cid_init(&dest->cid, p, cil);
  p += cil;
  memcpy(dest->stateless_reset_token, p, NGTCP2_STATELESS_RESET_TOKENLEN);
  p += NGTCP2_STATELESS_RESET_TOKENLEN;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_stop_sending_frame(ngtcp2_stop_sending *dest,
                                                  const uint8_t *payload,
                                                  size_t payloadlen) {
  size_t len = 1 + 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }
  p += n;
  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_STOP_SENDING;
  dest->stream_id = (int64_t)ngtcp2_get_varint(&n, p);
  p += n;
  dest->app_error_code = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_path_challenge_frame(ngtcp2_path_challenge *dest,
                                                    const uint8_t *payload,
                                                    size_t payloadlen) {
  size_t len = 1 + 8;
  const uint8_t *p;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_PATH_CHALLENGE;
  ngtcp2_cpymem(dest->data, p, sizeof(dest->data));
  p += sizeof(dest->data);

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_path_response_frame(ngtcp2_path_response *dest,
                                                   const uint8_t *payload,
                                                   size_t payloadlen) {
  size_t len = 1 + 8;
  const uint8_t *p;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  dest->type = NGTCP2_FRAME_PATH_RESPONSE;
  ngtcp2_cpymem(dest->data, p, sizeof(dest->data));
  p += sizeof(dest->data);

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_crypto_frame(ngtcp2_crypto *dest,
                                            const uint8_t *payload,
                                            size_t payloadlen) {
  size_t len = 1 + 1 + 1;
  const uint8_t *p;
  size_t datalen;
  size_t ndatalen;
  size_t n;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p += n;

  ndatalen = ngtcp2_get_varint_len(p);
  len += ndatalen - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  vi = ngtcp2_get_varint(&ndatalen, p);
  if (payloadlen - len < vi) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  datalen = (size_t)vi;
  len += datalen;

  p = payload + 1;

  dest->type = NGTCP2_FRAME_CRYPTO;
  dest->offset = ngtcp2_get_varint(&n, p);
  p += n;
  dest->data[0].len = datalen;
  p += ndatalen;
  if (dest->data[0].len) {
    dest->data[0].base = (uint8_t *)p;
    p += dest->data[0].len;
    dest->datacnt = 1;
  } else {
    dest->data[0].base = NULL;
    dest->datacnt = 0;
  }

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_new_token_frame(ngtcp2_new_token *dest,
                                               const uint8_t *payload,
                                               size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;
  size_t datalen;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  vi = ngtcp2_get_varint(&n, p);
  if (payloadlen - len < vi) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }
  datalen = (size_t)vi;
  len += datalen;

  dest->type = NGTCP2_FRAME_NEW_TOKEN;
  dest->token.len = datalen;
  p += n;
  dest->token.base = (uint8_t *)p;
  p += dest->token.len;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_decode_retire_connection_id_frame(ngtcp2_retire_connection_id *dest,
                                             const uint8_t *payload,
                                             size_t payloadlen) {
  size_t len = 1 + 1;
  const uint8_t *p;
  size_t n;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  p = payload + 1;

  n = ngtcp2_get_varint_len(p);
  len += n - 1;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  dest->type = NGTCP2_FRAME_RETIRE_CONNECTION_ID;
  dest->seq = ngtcp2_get_varint(&n, p);
  p += n;

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_decode_handshake_done_frame(ngtcp2_handshake_done *dest,
                                                    const uint8_t *payload,
                                                    size_t payloadlen) {
  (void)payload;
  (void)payloadlen;

  dest->type = NGTCP2_FRAME_HANDSHAKE_DONE;
  return 1;
}

ngtcp2_ssize ngtcp2_pkt_decode_datagram_frame(ngtcp2_datagram *dest,
                                              const uint8_t *payload,
                                              size_t payloadlen) {
  size_t len = 1;
  const uint8_t *p;
  uint8_t type;
  size_t datalen;
  size_t n;
  uint64_t vi;

  if (payloadlen < len) {
    return NGTCP2_ERR_FRAME_ENCODING;
  }

  type = payload[0];

  p = payload + 1;

  switch (type) {
  case NGTCP2_FRAME_DATAGRAM:
    datalen = payloadlen - 1;
    len = payloadlen;
    break;
  case NGTCP2_FRAME_DATAGRAM_LEN:
    ++len;
    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    n = ngtcp2_get_varint_len(p);
    len += n - 1;

    if (payloadlen < len) {
      return NGTCP2_ERR_FRAME_ENCODING;
    }

    vi = ngtcp2_get_varint(&n, p);
    if (payloadlen - len < vi) {
      return NGTCP2_FRAME_ENCODING_ERROR;
    }

    datalen = (size_t)vi;
    len += datalen;
    break;
  default:
    assert(0);
  }

  dest->type = type;

  if (datalen == 0) {
    dest->datacnt = 0;
    dest->data = NULL;

    if (type == NGTCP2_FRAME_DATAGRAM_LEN) {
      p += n;
    }
  } else {
    dest->datacnt = 1;
    dest->data = dest->rdata;
    dest->rdata[0].len = datalen;

    if (type == NGTCP2_FRAME_DATAGRAM_LEN) {
      p += n;
    }

    dest->rdata[0].base = (uint8_t *)p;
    p += datalen;
  }

  assert((size_t)(p - payload) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_frame(uint8_t *out, size_t outlen,
                                     ngtcp2_frame *fr) {
  switch (fr->type) {
  case NGTCP2_FRAME_STREAM:
    return ngtcp2_pkt_encode_stream_frame(out, outlen, &fr->stream);
  case NGTCP2_FRAME_ACK:
  case NGTCP2_FRAME_ACK_ECN:
    return ngtcp2_pkt_encode_ack_frame(out, outlen, &fr->ack);
  case NGTCP2_FRAME_PADDING:
    return ngtcp2_pkt_encode_padding_frame(out, outlen, &fr->padding);
  case NGTCP2_FRAME_RESET_STREAM:
    return ngtcp2_pkt_encode_reset_stream_frame(out, outlen, &fr->reset_stream);
  case NGTCP2_FRAME_CONNECTION_CLOSE:
  case NGTCP2_FRAME_CONNECTION_CLOSE_APP:
    return ngtcp2_pkt_encode_connection_close_frame(out, outlen,
                                                    &fr->connection_close);
  case NGTCP2_FRAME_MAX_DATA:
    return ngtcp2_pkt_encode_max_data_frame(out, outlen, &fr->max_data);
  case NGTCP2_FRAME_MAX_STREAM_DATA:
    return ngtcp2_pkt_encode_max_stream_data_frame(out, outlen,
                                                   &fr->max_stream_data);
  case NGTCP2_FRAME_MAX_STREAMS_BIDI:
  case NGTCP2_FRAME_MAX_STREAMS_UNI:
    return ngtcp2_pkt_encode_max_streams_frame(out, outlen, &fr->max_streams);
  case NGTCP2_FRAME_PING:
    return ngtcp2_pkt_encode_ping_frame(out, outlen, &fr->ping);
  case NGTCP2_FRAME_DATA_BLOCKED:
    return ngtcp2_pkt_encode_data_blocked_frame(out, outlen, &fr->data_blocked);
  case NGTCP2_FRAME_STREAM_DATA_BLOCKED:
    return ngtcp2_pkt_encode_stream_data_blocked_frame(
        out, outlen, &fr->stream_data_blocked);
  case NGTCP2_FRAME_STREAMS_BLOCKED_BIDI:
  case NGTCP2_FRAME_STREAMS_BLOCKED_UNI:
    return ngtcp2_pkt_encode_streams_blocked_frame(out, outlen,
                                                   &fr->streams_blocked);
  case NGTCP2_FRAME_NEW_CONNECTION_ID:
    return ngtcp2_pkt_encode_new_connection_id_frame(out, outlen,
                                                     &fr->new_connection_id);
  case NGTCP2_FRAME_STOP_SENDING:
    return ngtcp2_pkt_encode_stop_sending_frame(out, outlen, &fr->stop_sending);
  case NGTCP2_FRAME_PATH_CHALLENGE:
    return ngtcp2_pkt_encode_path_challenge_frame(out, outlen,
                                                  &fr->path_challenge);
  case NGTCP2_FRAME_PATH_RESPONSE:
    return ngtcp2_pkt_encode_path_response_frame(out, outlen,
                                                 &fr->path_response);
  case NGTCP2_FRAME_CRYPTO:
    return ngtcp2_pkt_encode_crypto_frame(out, outlen, &fr->crypto);
  case NGTCP2_FRAME_NEW_TOKEN:
    return ngtcp2_pkt_encode_new_token_frame(out, outlen, &fr->new_token);
  case NGTCP2_FRAME_RETIRE_CONNECTION_ID:
    return ngtcp2_pkt_encode_retire_connection_id_frame(
        out, outlen, &fr->retire_connection_id);
  case NGTCP2_FRAME_HANDSHAKE_DONE:
    return ngtcp2_pkt_encode_handshake_done_frame(out, outlen,
                                                  &fr->handshake_done);
  case NGTCP2_FRAME_DATAGRAM:
  case NGTCP2_FRAME_DATAGRAM_LEN:
    return ngtcp2_pkt_encode_datagram_frame(out, outlen, &fr->datagram);
  default:
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }
}

ngtcp2_ssize ngtcp2_pkt_encode_stream_frame(uint8_t *out, size_t outlen,
                                            ngtcp2_stream *fr) {
  size_t len = 1;
  uint8_t flags = NGTCP2_STREAM_LEN_BIT;
  uint8_t *p;
  size_t i;
  size_t datalen = 0;

  if (fr->fin) {
    flags |= NGTCP2_STREAM_FIN_BIT;
  }

  if (fr->offset) {
    flags |= NGTCP2_STREAM_OFF_BIT;
    len += ngtcp2_put_varint_len(fr->offset);
  }

  len += ngtcp2_put_varint_len((uint64_t)fr->stream_id);

  for (i = 0; i < fr->datacnt; ++i) {
    datalen += fr->data[i].len;
  }

  len += ngtcp2_put_varint_len(datalen);
  len += datalen;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = flags | NGTCP2_FRAME_STREAM;

  fr->flags = flags;

  p = ngtcp2_put_varint(p, (uint64_t)fr->stream_id);

  if (fr->offset) {
    p = ngtcp2_put_varint(p, fr->offset);
  }

  p = ngtcp2_put_varint(p, datalen);

  for (i = 0; i < fr->datacnt; ++i) {
    assert(fr->data[i].len);
    assert(fr->data[i].base);
    p = ngtcp2_cpymem(p, fr->data[i].base, fr->data[i].len);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_ack_frame(uint8_t *out, size_t outlen,
                                         ngtcp2_ack *fr) {
  size_t len = 1 + ngtcp2_put_varint_len((uint64_t)fr->largest_ack) +
               ngtcp2_put_varint_len(fr->ack_delay) +
               ngtcp2_put_varint_len(fr->num_blks) +
               ngtcp2_put_varint_len(fr->first_ack_blklen);
  uint8_t *p;
  size_t i;
  const ngtcp2_ack_blk *blk;

  for (i = 0; i < fr->num_blks; ++i) {
    blk = &fr->blks[i];
    len += ngtcp2_put_varint_len(blk->gap);
    len += ngtcp2_put_varint_len(blk->blklen);
  }

  if (fr->type == NGTCP2_FRAME_ACK_ECN) {
    len += ngtcp2_put_varint_len(fr->ecn.ect0) +
           ngtcp2_put_varint_len(fr->ecn.ect1) +
           ngtcp2_put_varint_len(fr->ecn.ce);
  }

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = fr->type;
  p = ngtcp2_put_varint(p, (uint64_t)fr->largest_ack);
  p = ngtcp2_put_varint(p, fr->ack_delay);
  p = ngtcp2_put_varint(p, fr->num_blks);
  p = ngtcp2_put_varint(p, fr->first_ack_blklen);

  for (i = 0; i < fr->num_blks; ++i) {
    blk = &fr->blks[i];
    p = ngtcp2_put_varint(p, blk->gap);
    p = ngtcp2_put_varint(p, blk->blklen);
  }

  if (fr->type == NGTCP2_FRAME_ACK_ECN) {
    p = ngtcp2_put_varint(p, fr->ecn.ect0);
    p = ngtcp2_put_varint(p, fr->ecn.ect1);
    p = ngtcp2_put_varint(p, fr->ecn.ce);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_padding_frame(uint8_t *out, size_t outlen,
                                             const ngtcp2_padding *fr) {
  if (outlen < fr->len) {
    return NGTCP2_ERR_NOBUF;
  }

  memset(out, 0, fr->len);

  return (ngtcp2_ssize)fr->len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_reset_stream_frame(uint8_t *out, size_t outlen,
                                     const ngtcp2_reset_stream *fr) {
  size_t len = 1 + ngtcp2_put_varint_len((uint64_t)fr->stream_id) +
               ngtcp2_put_varint_len(fr->app_error_code) +
               ngtcp2_put_varint_len(fr->final_size);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_RESET_STREAM;
  p = ngtcp2_put_varint(p, (uint64_t)fr->stream_id);
  p = ngtcp2_put_varint(p, fr->app_error_code);
  p = ngtcp2_put_varint(p, fr->final_size);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_connection_close_frame(uint8_t *out, size_t outlen,
                                         const ngtcp2_connection_close *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->error_code) +
               (fr->type == NGTCP2_FRAME_CONNECTION_CLOSE
                    ? ngtcp2_put_varint_len(fr->frame_type)
                    : 0) +
               ngtcp2_put_varint_len(fr->reasonlen) + fr->reasonlen;
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = fr->type;
  p = ngtcp2_put_varint(p, fr->error_code);
  if (fr->type == NGTCP2_FRAME_CONNECTION_CLOSE) {
    p = ngtcp2_put_varint(p, fr->frame_type);
  }
  p = ngtcp2_put_varint(p, fr->reasonlen);
  if (fr->reasonlen) {
    p = ngtcp2_cpymem(p, fr->reason, fr->reasonlen);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_max_data_frame(uint8_t *out, size_t outlen,
                                              const ngtcp2_max_data *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->max_data);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_MAX_DATA;
  p = ngtcp2_put_varint(p, fr->max_data);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_max_stream_data_frame(uint8_t *out, size_t outlen,
                                        const ngtcp2_max_stream_data *fr) {
  size_t len = 1 + ngtcp2_put_varint_len((uint64_t)fr->stream_id) +
               ngtcp2_put_varint_len(fr->max_stream_data);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_MAX_STREAM_DATA;
  p = ngtcp2_put_varint(p, (uint64_t)fr->stream_id);
  p = ngtcp2_put_varint(p, fr->max_stream_data);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_max_streams_frame(uint8_t *out, size_t outlen,
                                                 const ngtcp2_max_streams *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->max_streams);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = fr->type;
  p = ngtcp2_put_varint(p, fr->max_streams);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_ping_frame(uint8_t *out, size_t outlen,
                                          const ngtcp2_ping *fr) {
  (void)fr;

  if (outlen < 1) {
    return NGTCP2_ERR_NOBUF;
  }

  *out++ = NGTCP2_FRAME_PING;

  return 1;
}

ngtcp2_ssize
ngtcp2_pkt_encode_data_blocked_frame(uint8_t *out, size_t outlen,
                                     const ngtcp2_data_blocked *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->offset);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_DATA_BLOCKED;
  p = ngtcp2_put_varint(p, fr->offset);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_stream_data_blocked_frame(
    uint8_t *out, size_t outlen, const ngtcp2_stream_data_blocked *fr) {
  size_t len = 1 + ngtcp2_put_varint_len((uint64_t)fr->stream_id) +
               ngtcp2_put_varint_len(fr->offset);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_STREAM_DATA_BLOCKED;
  p = ngtcp2_put_varint(p, (uint64_t)fr->stream_id);
  p = ngtcp2_put_varint(p, fr->offset);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_streams_blocked_frame(uint8_t *out, size_t outlen,
                                        const ngtcp2_streams_blocked *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->max_streams);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = fr->type;
  p = ngtcp2_put_varint(p, fr->max_streams);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_new_connection_id_frame(uint8_t *out, size_t outlen,
                                          const ngtcp2_new_connection_id *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->seq) +
               ngtcp2_put_varint_len(fr->retire_prior_to) + 1 +
               fr->cid.datalen + NGTCP2_STATELESS_RESET_TOKENLEN;
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_NEW_CONNECTION_ID;
  p = ngtcp2_put_varint(p, fr->seq);
  p = ngtcp2_put_varint(p, fr->retire_prior_to);
  *p++ = (uint8_t)fr->cid.datalen;
  p = ngtcp2_cpymem(p, fr->cid.data, fr->cid.datalen);
  p = ngtcp2_cpymem(p, fr->stateless_reset_token,
                    NGTCP2_STATELESS_RESET_TOKENLEN);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_stop_sending_frame(uint8_t *out, size_t outlen,
                                     const ngtcp2_stop_sending *fr) {
  size_t len = 1 + ngtcp2_put_varint_len((uint64_t)fr->stream_id) +
               ngtcp2_put_varint_len(fr->app_error_code);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_STOP_SENDING;
  p = ngtcp2_put_varint(p, (uint64_t)fr->stream_id);
  p = ngtcp2_put_varint(p, fr->app_error_code);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_path_challenge_frame(uint8_t *out, size_t outlen,
                                       const ngtcp2_path_challenge *fr) {
  size_t len = 1 + 8;
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_PATH_CHALLENGE;
  p = ngtcp2_cpymem(p, fr->data, sizeof(fr->data));

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_path_response_frame(uint8_t *out, size_t outlen,
                                      const ngtcp2_path_response *fr) {
  size_t len = 1 + 8;
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_PATH_RESPONSE;
  p = ngtcp2_cpymem(p, fr->data, sizeof(fr->data));

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_crypto_frame(uint8_t *out, size_t outlen,
                                            const ngtcp2_crypto *fr) {
  size_t len = 1;
  uint8_t *p;
  size_t i;
  size_t datalen = 0;

  len += ngtcp2_put_varint_len(fr->offset);

  for (i = 0; i < fr->datacnt; ++i) {
    datalen += fr->data[i].len;
  }

  len += ngtcp2_put_varint_len(datalen);
  len += datalen;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_CRYPTO;

  p = ngtcp2_put_varint(p, fr->offset);
  p = ngtcp2_put_varint(p, datalen);

  for (i = 0; i < fr->datacnt; ++i) {
    assert(fr->data[i].base);
    p = ngtcp2_cpymem(p, fr->data[i].base, fr->data[i].len);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_new_token_frame(uint8_t *out, size_t outlen,
                                               const ngtcp2_new_token *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->token.len) + fr->token.len;
  uint8_t *p;

  assert(fr->token.len);

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_NEW_TOKEN;

  p = ngtcp2_put_varint(p, fr->token.len);
  p = ngtcp2_cpymem(p, fr->token.base, fr->token.len);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_encode_retire_connection_id_frame(
    uint8_t *out, size_t outlen, const ngtcp2_retire_connection_id *fr) {
  size_t len = 1 + ngtcp2_put_varint_len(fr->seq);
  uint8_t *p;

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = NGTCP2_FRAME_RETIRE_CONNECTION_ID;

  p = ngtcp2_put_varint(p, fr->seq);

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize
ngtcp2_pkt_encode_handshake_done_frame(uint8_t *out, size_t outlen,
                                       const ngtcp2_handshake_done *fr) {
  (void)fr;

  if (outlen < 1) {
    return NGTCP2_ERR_NOBUF;
  }

  *out++ = NGTCP2_FRAME_HANDSHAKE_DONE;

  return 1;
}

ngtcp2_ssize ngtcp2_pkt_encode_datagram_frame(uint8_t *out, size_t outlen,
                                              const ngtcp2_datagram *fr) {
  size_t datalen = ngtcp2_vec_len(fr->data, fr->datacnt);
  size_t len =
      1 +
      (fr->type == NGTCP2_FRAME_DATAGRAM ? 0 : ngtcp2_put_varint_len(datalen)) +
      datalen;
  uint8_t *p;
  size_t i;

  assert(fr->type == NGTCP2_FRAME_DATAGRAM ||
         fr->type == NGTCP2_FRAME_DATAGRAM_LEN);

  if (outlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = out;

  *p++ = fr->type;
  if (fr->type == NGTCP2_FRAME_DATAGRAM_LEN) {
    p = ngtcp2_put_varint(p, (uint64_t)datalen);
  }

  for (i = 0; i < fr->datacnt; ++i) {
    assert(fr->data[i].len);
    assert(fr->data[i].base);
    p = ngtcp2_cpymem(p, fr->data[i].base, fr->data[i].len);
  }

  assert((size_t)(p - out) == len);

  return (ngtcp2_ssize)len;
}

ngtcp2_ssize ngtcp2_pkt_write_version_negotiation(
    uint8_t *dest, size_t destlen, uint8_t unused_random, const uint8_t *dcid,
    size_t dcidlen, const uint8_t *scid, size_t scidlen, const uint32_t *sv,
    size_t nsv) {
  size_t len = 1 + 4 + 1 + dcidlen + 1 + scidlen + nsv * 4;
  uint8_t *p;
  size_t i;

  assert(dcidlen < 256);
  assert(scidlen < 256);

  if (destlen < len) {
    return NGTCP2_ERR_NOBUF;
  }

  p = dest;

  *p++ = 0x80 | unused_random;
  p = ngtcp2_put_uint32be(p, 0);
  *p++ = (uint8_t)dcidlen;
  if (dcidlen) {
    p = ngtcp2_cpymem(p, dcid, dcidlen);
  }
  *p++ = (uint8_t)scidlen;
  if (scidlen) {
    p = ngtcp2_cpymem(p, scid, scidlen);
  }

  for (i = 0; i < nsv; ++i) {
    p = ngtcp2_put_uint32be(p, sv[i]);
  }

  assert((size_t)(p - dest) == len);

  return (ngtcp2_ssize)len;
}

size_t ngtcp2_pkt_decode_version_negotiation(uint32_t *dest,
                                             const uint8_t *payload,
                                             size_t payloadlen) {
  const uint8_t *end = payload + payloadlen;

  assert((payloadlen % sizeof(uint32_t)) == 0);

  for (; payload != end; payload += sizeof(uint32_t)) {
    *dest++ = ngtcp2_get_uint32(payload);
  }

  return payloadlen / sizeof(uint32_t);
}

int ngtcp2_pkt_decode_stateless_reset(ngtcp2_pkt_stateless_reset *sr,
                                      const uint8_t *payload,
                                      size_t payloadlen) {
  const uint8_t *p = payload;

  if (payloadlen <
      NGTCP2_MIN_STATELESS_RESET_RANDLEN + NGTCP2_STATELESS_RESET_TOKENLEN) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  sr->rand = p;
  sr->randlen = payloadlen - NGTCP2_STATELESS_RESET_TOKENLEN;
  p += sr->randlen;
  memcpy(sr->stateless_reset_token, p, NGTCP2_STATELESS_RESET_TOKENLEN);

  return 0;
}

int ngtcp2_pkt_decode_retry(ngtcp2_pkt_retry *dest, const uint8_t *payload,
                            size_t payloadlen) {
  size_t len = /* token */ 1 + NGTCP2_RETRY_TAGLEN;

  if (payloadlen < len) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  dest->token.base = (uint8_t *)payload;
  dest->token.len = (size_t)(payloadlen - NGTCP2_RETRY_TAGLEN);
  ngtcp2_cpymem(dest->tag, payload + dest->token.len, NGTCP2_RETRY_TAGLEN);

  return 0;
}

int64_t ngtcp2_pkt_adjust_pkt_num(int64_t max_pkt_num, int64_t pkt_num,
                                  size_t n) {
  int64_t expected = max_pkt_num + 1;
  int64_t win = (int64_t)1 << n;
  int64_t hwin = win / 2;
  int64_t mask = win - 1;
  int64_t cand = (expected & ~mask) | pkt_num;

  if (cand <= expected - hwin) {
    assert(cand <= (int64_t)NGTCP2_MAX_VARINT - win);
    return cand + win;
  }
  if (cand > expected + hwin && cand >= win) {
    return cand - win;
  }
  return cand;
}

int ngtcp2_pkt_validate_ack(ngtcp2_ack *fr) {
  int64_t largest_ack = fr->largest_ack;
  size_t i;

  if (largest_ack < (int64_t)fr->first_ack_blklen) {
    return NGTCP2_ERR_ACK_FRAME;
  }

  largest_ack -= (int64_t)fr->first_ack_blklen;

  for (i = 0; i < fr->num_blks; ++i) {
    if (largest_ack < (int64_t)fr->blks[i].gap + 2) {
      return NGTCP2_ERR_ACK_FRAME;
    }

    largest_ack -= (int64_t)fr->blks[i].gap + 2;

    if (largest_ack < (int64_t)fr->blks[i].blklen) {
      return NGTCP2_ERR_ACK_FRAME;
    }

    largest_ack -= (int64_t)fr->blks[i].blklen;
  }

  return 0;
}

ngtcp2_ssize
ngtcp2_pkt_write_stateless_reset(uint8_t *dest, size_t destlen,
                                 const uint8_t *stateless_reset_token,
                                 const uint8_t *rand, size_t randlen) {
  uint8_t *p;

  if (destlen <
      NGTCP2_MIN_STATELESS_RESET_RANDLEN + NGTCP2_STATELESS_RESET_TOKENLEN) {
    return NGTCP2_ERR_NOBUF;
  }

  if (randlen < NGTCP2_MIN_STATELESS_RESET_RANDLEN) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  p = dest;

  randlen = ngtcp2_min(destlen - NGTCP2_STATELESS_RESET_TOKENLEN, randlen);

  p = ngtcp2_cpymem(p, rand, randlen);
  p = ngtcp2_cpymem(p, stateless_reset_token, NGTCP2_STATELESS_RESET_TOKENLEN);
  *dest = (uint8_t)((*dest & 0x7fu) | 0x40u);

  return p - dest;
}

ngtcp2_ssize ngtcp2_pkt_write_retry(
    uint8_t *dest, size_t destlen, uint32_t version, const ngtcp2_cid *dcid,
    const ngtcp2_cid *scid, const ngtcp2_cid *odcid, const uint8_t *token,
    size_t tokenlen, ngtcp2_encrypt encrypt, const ngtcp2_crypto_aead *aead,
    const ngtcp2_crypto_aead_ctx *aead_ctx) {
  ngtcp2_pkt_hd hd;
  uint8_t pseudo_retry[1500];
  ngtcp2_ssize pseudo_retrylen;
  uint8_t tag[NGTCP2_RETRY_TAGLEN];
  int rv;
  uint8_t *p;
  size_t offset;
  const uint8_t *nonce;
  size_t noncelen;

  assert(tokenlen > 0);
  assert(!ngtcp2_cid_eq(scid, odcid));

  /* Retry packet is sent at most once per one connection attempt.  In
     the first connection attempt, client has to send random DCID
     which is at least 8 bytes long. */
  if (odcid->datalen < 8) {
    return NGTCP2_ERR_INVALID_ARGUMENT;
  }

  ngtcp2_pkt_hd_init(&hd, NGTCP2_PKT_FLAG_LONG_FORM, NGTCP2_PKT_RETRY, dcid,
                     scid, /* pkt_num = */ 0, /* pkt_numlen = */ 1, version,
                     /* len = */ 0);

  pseudo_retrylen =
      ngtcp2_pkt_encode_pseudo_retry(pseudo_retry, sizeof(pseudo_retry), &hd,
                                     /* unused = */ 0, odcid, token, tokenlen);
  if (pseudo_retrylen < 0) {
    return pseudo_retrylen;
  }

  if (version == NGTCP2_PROTO_VER_V1) {
    nonce = (const uint8_t *)NGTCP2_RETRY_NONCE_V1;
    noncelen = sizeof(NGTCP2_RETRY_NONCE_V1) - 1;
  } else {
    nonce = (const uint8_t *)NGTCP2_RETRY_NONCE_DRAFT;
    noncelen = sizeof(NGTCP2_RETRY_NONCE_DRAFT) - 1;
  }

  /* OpenSSL does not like NULL plaintext. */
  rv = encrypt(tag, aead, aead_ctx, (const uint8_t *)"", 0, nonce, noncelen,
               pseudo_retry, (size_t)pseudo_retrylen);
  if (rv != 0) {
    return rv;
  }

  offset = 1 + odcid->datalen;
  if (destlen < (size_t)pseudo_retrylen + sizeof(tag) - offset) {
    return NGTCP2_ERR_NOBUF;
  }

  p = ngtcp2_cpymem(dest, pseudo_retry + offset,
                    (size_t)pseudo_retrylen - offset);
  p = ngtcp2_cpymem(p, tag, sizeof(tag));

  return p - dest;
}

ngtcp2_ssize ngtcp2_pkt_encode_pseudo_retry(
    uint8_t *dest, size_t destlen, const ngtcp2_pkt_hd *hd, uint8_t unused,
    const ngtcp2_cid *odcid, const uint8_t *token, size_t tokenlen) {
  uint8_t *p = dest;
  ngtcp2_ssize nwrite;

  if (destlen < 1 + odcid->datalen) {
    return NGTCP2_ERR_NOBUF;
  }

  *p++ = (uint8_t)odcid->datalen;
  p = ngtcp2_cpymem(p, odcid->data, odcid->datalen);
  destlen -= (size_t)(p - dest);

  nwrite = ngtcp2_pkt_encode_hd_long(p, destlen, hd);
  if (nwrite < 0) {
    return nwrite;
  }

  if (destlen < (size_t)nwrite + tokenlen) {
    return NGTCP2_ERR_NOBUF;
  }

  *p &= 0xf0;
  *p |= unused;

  p += nwrite;

  p = ngtcp2_cpymem(p, token, tokenlen);

  return p - dest;
}

int ngtcp2_pkt_verify_retry_tag(uint32_t version, const ngtcp2_pkt_retry *retry,
                                const uint8_t *pkt, size_t pktlen,
                                ngtcp2_encrypt encrypt,
                                const ngtcp2_crypto_aead *aead,
                                const ngtcp2_crypto_aead_ctx *aead_ctx) {
  uint8_t pseudo_retry[1500];
  size_t pseudo_retrylen;
  uint8_t *p = pseudo_retry;
  int rv;
  uint8_t tag[NGTCP2_RETRY_TAGLEN];
  const uint8_t *nonce;
  size_t noncelen;

  assert(pktlen >= sizeof(retry->tag));

  if (sizeof(pseudo_retry) <
      1 + retry->odcid.datalen + pktlen - sizeof(retry->tag)) {
    return NGTCP2_ERR_PROTO;
  }

  *p++ = (uint8_t)retry->odcid.datalen;
  p = ngtcp2_cpymem(p, retry->odcid.data, retry->odcid.datalen);
  p = ngtcp2_cpymem(p, pkt, pktlen - sizeof(retry->tag));

  pseudo_retrylen = (size_t)(p - pseudo_retry);

  if (version == NGTCP2_PROTO_VER_V1) {
    nonce = (const uint8_t *)NGTCP2_RETRY_NONCE_V1;
    noncelen = sizeof(NGTCP2_RETRY_NONCE_V1) - 1;
  } else {
    nonce = (const uint8_t *)NGTCP2_RETRY_NONCE_DRAFT;
    noncelen = sizeof(NGTCP2_RETRY_NONCE_DRAFT) - 1;
  }

  /* OpenSSL does not like NULL plaintext. */
  rv = encrypt(tag, aead, aead_ctx, (const uint8_t *)"", 0, nonce, noncelen,
               pseudo_retry, pseudo_retrylen);
  if (rv != 0) {
    return rv;
  }

  if (0 != memcmp(retry->tag, tag, sizeof(retry->tag))) {
    return NGTCP2_ERR_PROTO;
  }

  return 0;
}

size_t ngtcp2_pkt_stream_max_datalen(int64_t stream_id, uint64_t offset,
                                     size_t len, size_t left) {
  size_t n = 1 /* type */ + ngtcp2_put_varint_len((uint64_t)stream_id) +
             (offset ? ngtcp2_put_varint_len(offset) : 0);

  if (left <= n) {
    return (size_t)-1;
  }

  left -= n;

  if (left > 8 + 1073741823 && len > 1073741823) {
#if SIZE_MAX > UINT32_MAX
    len = ngtcp2_min(len, 4611686018427387903lu);
#endif /* SIZE_MAX > UINT32_MAX */
    return ngtcp2_min(len, left - 8);
  }

  if (left > 4 + 16383 && len > 16383) {
    len = ngtcp2_min(len, 1073741823);
    return ngtcp2_min(len, left - 4);
  }

  if (left > 2 + 63 && len > 63) {
    len = ngtcp2_min(len, 16383);
    return ngtcp2_min(len, left - 2);
  }

  len = ngtcp2_min(len, 63);
  return ngtcp2_min(len, left - 1);
}

size_t ngtcp2_pkt_crypto_max_datalen(uint64_t offset, size_t len, size_t left) {
  size_t n = 1 /* type */ + ngtcp2_put_varint_len(offset);

  /* CRYPTO frame must contain nonzero length data.  Return -1 if
     there is no space to write crypto data. */
  if (left <= n + 1) {
    return (size_t)-1;
  }

  left -= n;

  if (left > 8 + 1073741823 && len > 1073741823) {
#if SIZE_MAX > UINT32_MAX
    len = ngtcp2_min(len, 4611686018427387903lu);
#endif /* SIZE_MAX > UINT32_MAX */
    return ngtcp2_min(len, left - 8);
  }

  if (left > 4 + 16383 && len > 16383) {
    len = ngtcp2_min(len, 1073741823);
    return ngtcp2_min(len, left - 4);
  }

  if (left > 2 + 63 && len > 63) {
    len = ngtcp2_min(len, 16383);
    return ngtcp2_min(len, left - 2);
  }

  len = ngtcp2_min(len, 63);
  return ngtcp2_min(len, left - 1);
}

size_t ngtcp2_pkt_datagram_framelen(size_t len) {
  return 1 /* type */ + ngtcp2_put_varint_len((uint64_t)len) + len;
}

uint8_t ngtcp2_pkt_get_type_long(uint8_t c) {
  return (uint8_t)((c & NGTCP2_LONG_TYPE_MASK) >> 4);
}

int ngtcp2_pkt_verify_reserved_bits(uint8_t c) {
  if (c & NGTCP2_HEADER_FORM_BIT) {
    return (c & NGTCP2_LONG_RESERVED_BIT_MASK) == 0 ? 0 : NGTCP2_ERR_PROTO;
  }
  return (c & NGTCP2_SHORT_RESERVED_BIT_MASK) == 0 ? 0 : NGTCP2_ERR_PROTO;
}
