/*
 * ngtcp2
 *
 * Copyright (c) 2019 ngtcp2 contributors
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
#ifndef NGTCP2_CRYPTO_OPENSSL_H
#define NGTCP2_CRYPTO_OPENSSL_H

#include <ngtcp2/ngtcp2.h>

#include <openssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @macrosection
 *
 * OpenSSL specific error codes
 */

/**
 * @macro
 *
 * :macro:`NGTCP2_CRYPTO_OPENSSL_ERR_TLS_WANT_X509_LOOKUP` is the
 * error code which indicates that TLS handshake routine is
 * interrupted by X509 certificate lookup.  See
 * :macro:`SSL_ERROR_WANT_X509_LOOKUP` error description from
 * `SSL_do_handshake`.
 */
#define NGTCP2_CRYPTO_OPENSSL_ERR_TLS_WANT_X509_LOOKUP -10001

/**
 * @macro
 *
 * :macro:`NGTCP2_CRYPTO_OPENSSL_ERR_TLS_WANT_CLIENT_HELLO_CB` is the
 * error code which indicates that TLS handshake routine is
 * interrupted by client hello callback.  See
 * :macro:`SSL_ERROR_WANT_CLIENT_HELLO_CB` error description from
 * `SSL_do_handshake`.
 */
#define NGTCP2_CRYPTO_OPENSSL_ERR_TLS_WANT_CLIENT_HELLO_CB -10002

/**
 * @function
 *
 * `ngtcp2_crypto_openssl_from_ossl_encryption_level` translates
 * |ossl_level| to :type:`ngtcp2_crypto_level`.  This function is only
 * available for OpenSSL backend.
 */
NGTCP2_EXTERN ngtcp2_crypto_level
ngtcp2_crypto_openssl_from_ossl_encryption_level(
    OSSL_ENCRYPTION_LEVEL ossl_level);

/**
 * @function
 *
 * `ngtcp2_crypto_openssl_from_ngtcp2_crypto_level` translates
 * |crypto_level| to OSSL_ENCRYPTION_LEVEL.  This function is only
 * available for OpenSSL backend.
 */
NGTCP2_EXTERN OSSL_ENCRYPTION_LEVEL
ngtcp2_crypto_openssl_from_ngtcp2_crypto_level(
    ngtcp2_crypto_level crypto_level);

#ifdef __cplusplus
}
#endif

#endif /* NGTCP2_CRYPTO_OPENSSL_H */
