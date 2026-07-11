// ======================================================================
// store.rs
//
// C ABI lifecycle for the interpreter's `spacewasm::Store`. The Store is
// allocated as a `Box<Store>` out of the page-backed global allocator (see
// `allocator.rs`), whose pages are physically owned by the C++
// `Svc::WasmSequencer` component. Ownership of the Store lives on the Rust side
// as `Box<Store>`; C++ only ever holds the opaque leaked pointer and hands it
// back to `fprime_spacewasm_store_delete` for teardown.
// ======================================================================

use core::alloc::Layout;
use core::ffi::c_void;

use spacewasm::{Allocator, Box, GlobalAllocator, Store};

/// Allocate a new `Store` as a `Box<Store>` in page-backed memory and hand the
/// caller an opaque pointer to it.
///
/// Returns `0` on success; otherwise the `spacewasm::AllocError` code
/// (`1 = OutOfMemory`, `2 = PageTooSmall`, other = AllocationFailed). On failure
/// `*out` is set to null.
///
/// # Safety
/// `out` must be a valid, writable pointer to a `*mut c_void`.
#[unsafe(no_mangle)]
pub extern "C" fn fprime_spacewasm_store_new(max_modules: u32, out: *mut *mut c_void) -> u32 {
    // No host modules are registered yet; pass an empty array.
    let store = match Store::new(max_modules as usize, []) {
        Ok(store) => store,
        Err(e) => {
            unsafe {
                *out = core::ptr::null_mut();
            }
            return u32::from(e);
        }
    };

    match Box::new(store) {
        Ok(boxed) => {
            // Leak the Box, transferring ownership of the Store to the C++ side.
            // It is reclaimed by `fprime_spacewasm_store_delete`.
            let ptr = Box::leak(boxed) as *mut Store;
            unsafe {
                *out = ptr as *mut c_void;
            }
            0
        }
        Err(e) => {
            unsafe {
                *out = core::ptr::null_mut();
            }
            u32::from(e)
        }
    }
}

/// Destroy a `Store` previously returned by [`fprime_spacewasm_store_new`].
/// A null pointer is a no-op.
///
/// # Safety
/// `store` must be a pointer previously returned by `fprime_spacewasm_store_new`
/// and not already passed to this function.
#[unsafe(no_mangle)]
pub extern "C" fn fprime_spacewasm_store_delete(store: *mut c_void) {
    if store.is_null() {
        return;
    }

    let ptr = store as *mut Store;
    // Reproduce `Box<Store>`'s Drop: drop the value in place, then release its
    // backing allocation through the same global allocator that produced it.
    // (`spacewasm::Box::from_raw` is crate-private, so we cannot re-box it.)
    unsafe {
        core::ptr::drop_in_place(ptr);
        GlobalAllocator.dealloc(ptr as *mut u8, Layout::new::<Store>());
    }
}
