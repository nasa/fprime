#ifndef FPRIME_SPACEWASM_H
#define FPRIME_SPACEWASM_H

#include <Svc/WasmSequencer/spacewasm_include/spacewasm.h>

extern "C" {
spacewasm_status_t spacewasm_fprime_register_global_allocator(spacewasm_global_alloc_fn_t alloc,
                                                              spacewasm_global_dealloc_fn_t dealloc,
                                                              void* userdata);

spacewasm_status_t spacewasm_fprime_acquire_global_allocator(void* userdata);
spacewasm_status_t spacewasm_fprime_release_global_allocator(void* userdata);
};

#endif
