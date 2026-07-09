use core::alloc::Layout;

use crate::ffi;

struct FprimeAlloc;

unsafe impl spacewasm::Allocator for FprimeAlloc {
    unsafe fn alloc(&self, layout: Layout) -> Result<*mut u8, spacewasm::AllocError> {
        let mut result = ffi::AllocResult::AllocationFailed;
        let ptr = unsafe { ffi::svc_wasmsequencer_alloc_page(&mut result, layout.size() as u32) };
        match result {
            ffi::AllocResult::Ok => Ok(ptr),
            ffi::AllocResult::AllocationFailed => Err(spacewasm::AllocError::AllocationFailed),
            ffi::AllocResult::OutOfMemory => Err(spacewasm::AllocError::OutOfMemory),
        }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
        unsafe {
            ffi::svc_wasmsequencer_dealloc_page(ptr, layout.size() as u32);
        }
    }

    fn memory_statistics(&self) -> spacewasm::MemoryStatistics {
        spacewasm::MemoryStatistics::default()
    }
}

spacewasm::global_allocator!(FprimeAlloc, FprimeAlloc);
