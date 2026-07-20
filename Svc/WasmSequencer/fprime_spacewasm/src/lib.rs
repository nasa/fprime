//! Thin staticlib wrapper over the published `spacewasm_c_api` crate.
//!
//! The `Svc::WasmSequencer` C++ component links `libwasm_seq_shim.a` and calls
//! the `spacewasm_*` C ABI directly (declared in the vendored `spacewasm.h`).
//! This crate exists only to turn the crates.io `spacewasm_c_api` dependency
//! into a single, stably-named staticlib artifact and to guarantee the
//! dependency's exported symbols survive archiving.
//!
//! Deliberately NOT defined here: `#[panic_handler]`, `#[global_allocator]`, and
//! `rust_eh_personality`. `spacewasm_c_api` (default features) provides all
//! three; defining a second copy of any is a duplicate lang item and fails to
//! link.
#![no_std]

extern crate spacewasm_c_api;

/// A `*const ()` is not `Sync`, so a bare pointer array cannot be a `static`.
/// The pointers are only ever held to keep symbols alive, never dereferenced, so
/// asserting `Sync` is sound.
#[repr(transparent)]
struct KeepPtr(*const ());
// SAFETY: the pointers are never read or dereferenced; the wrapper exists solely
// to satisfy the `Sync` bound on a `static`.
unsafe impl Sync for KeepPtr {}

// Nothing in the fprime C++ references these Rust symbols by their Rust path, so
// without a hard reference the linker/archiver can drop the dependency's object
// files and the `#[no_mangle] extern "C"` `spacewasm_*` symbols vanish from the
// archive. Taking the address of every C entry point in a `#[used]` table forces
// those objects to be retained. The list mirrors the exports declared in
// `spacewasm.h`; `spacewasm_panic` (integrator-supplied) and `rust_eh_personality`
// (crate-provided) are intentionally omitted. Addresses are stored as `usize`
// so the static is `Sync`.
#[used]
static KEEP: [KeepPtr; 21] = [
    KeepPtr(spacewasm_c_api::global_alloc::spacewasm_set_global_allocator as *const ()),
    KeepPtr(spacewasm_c_api::spacewasm_memory_statistics as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_allocator_new as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_allocator_destroy as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_host_new as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_host_destroy as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_add_host_module as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_add_host_function as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_new as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_load_module as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_find_export_func as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_module_needs_start as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_run_start as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_invoke as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_run as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_run_to_completion as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_get_result as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_store_destroy as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_mem_read as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_mem_write as *const ()),
    KeepPtr(spacewasm_c_api::capi::spacewasm_mem_size as *const ()),
];
