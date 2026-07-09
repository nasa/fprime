//! C ABI wrapper crate over the `spacewasm` WebAssembly interpreter.
#![no_std]

use core::alloc::Layout;
use core::panic::PanicInfo;

/// Stub allocator backing `spacewasm`. It performs no real allocation yet; every
/// request fails. Replace with a real allocator (arena / pool) when interpreter
/// functionality is wired in.
struct StubAlloc;

unsafe impl spacewasm::Allocator for StubAlloc {
    unsafe fn alloc(&self, _layout: Layout) -> Result<*mut u8, spacewasm::AllocError> {
        // TODO: back with a real bare-metal allocator (e.g. a static arena).
        Err(spacewasm::AllocError::AllocationFailed)
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
        // No-op: the stub allocator never hands out memory.
    }

    fn memory_statistics(&self) -> spacewasm::MemoryStatistics {
        spacewasm::MemoryStatistics::default()
    }
}

spacewasm::global_allocator!(StubAlloc, StubAlloc);

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // TODO(tumbar) We should have a graceful handler for this
    // I.e. call into the WasmSeqeuencer which should emit WARNING_HI and reset the Rust state
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn fprime_spacewasm_probe() -> u32 {
    42
}
