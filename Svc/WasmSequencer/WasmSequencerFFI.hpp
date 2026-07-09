#ifndef SVC_WASMSEQUENCER_FFI_HPP
#define SVC_WASMSEQUENCER_FFI_HPP

#include <cstdint>

enum class AllocResult { Ok, AllocationFailed, OutOfMemory };

#ifdef __cplusplus
extern "C" {
#endif

/// Allocate a new page with a constant size.
/// This size should be predetermined and accepted on both ends of the FFI
uint8_t* svc_wasmsequencer_alloc_page(AllocResult* result, uint32_t size);
void svc_wasmsequencer_dealloc_page(uint8_t* ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
