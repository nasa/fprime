/*
 * nghttp3
 *
 * Copyright (c) 2019 nghttp3 contributors
 * Copyright (c) 2017 ngtcp2 contributors
 * Copyright (c) 2014 nghttp2 contributors
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
#include "nghttp3_mem.h"

static void *default_malloc(size_t size, void *mem_user_data) {
  (void)mem_user_data;

  return malloc(size);
}

static void default_free(void *ptr, void *mem_user_data) {
  (void)mem_user_data;

  free(ptr);
}

static void *default_calloc(size_t nmemb, size_t size, void *mem_user_data) {
  (void)mem_user_data;

  return calloc(nmemb, size);
}

static void *default_realloc(void *ptr, size_t size, void *mem_user_data) {
  (void)mem_user_data;

  return realloc(ptr, size);
}

static nghttp3_mem mem_default = {NULL, default_malloc, default_free,
                                  default_calloc, default_realloc};

const nghttp3_mem *nghttp3_mem_default(void) { return &mem_default; }

void *nghttp3_mem_malloc(const nghttp3_mem *mem, size_t size) {
  return mem->malloc(size, mem->mem_user_data);
}

void nghttp3_mem_free(const nghttp3_mem *mem, void *ptr) {
  mem->free(ptr, mem->mem_user_data);
}

void nghttp3_mem_free2(const nghttp3_free free_func, void *ptr,
                       void *mem_user_data) {
  free_func(ptr, mem_user_data);
}

void *nghttp3_mem_calloc(const nghttp3_mem *mem, size_t nmemb, size_t size) {
  return mem->calloc(nmemb, size, mem->mem_user_data);
}

void *nghttp3_mem_realloc(const nghttp3_mem *mem, void *ptr, size_t size) {
  return mem->realloc(ptr, size, mem->mem_user_data);
}
