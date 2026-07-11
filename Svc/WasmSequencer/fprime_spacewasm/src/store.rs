use core::ffi::c_void;
use spacewasm::{Box, Store};

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

    let ptr: Box<Store> = unsafe { core::mem::transmute(store) };
    core::mem::drop(ptr);
}
