#![no_std]

use core::{
    ffi::c_void,
    ptr::null_mut,
    sync::atomic::{AtomicUsize, Ordering},
};

pub use spacewasm_c_api::*;

const MAX_PAGES: usize = 8;
const PAGE_SIZE: usize = 8192;
const MAX_SEQUENCERS: usize = 8;

// TODO(tumbar) Remove these aliases.
// SpaceWasm currently only exports these when the global alloc feature is active

/// Allocate `size` bytes aligned to `align`. Return NULL on failure. Per page allocation.
type SpacewasmGlobalAllocFnT =
    Option<unsafe extern "C" fn(userdata: *mut c_void, size: usize, align: usize) -> *mut u8>;

/// Free the `size`-byte allocation at `ptr` (alignment `align`). Per page deallocation.
type SpacewasmGlobalDeallocFnT =
    Option<unsafe extern "C" fn(userdata: *mut c_void, ptr: *mut u8, size: usize, align: usize)>;

struct FprimeWasmGlobalAllocator {
    alloc: SpacewasmGlobalAllocFnT,
    dealloc: SpacewasmGlobalDeallocFnT,
    userdata: *mut c_void,
}

impl FprimeWasmGlobalAllocator {
    pub const fn new() -> FprimeWasmGlobalAllocator {
        FprimeWasmGlobalAllocator {
            alloc: None,
            dealloc: None,
            userdata: null_mut(),
        }
    }
}

unsafe impl spacewasm::Allocator for FprimeWasmGlobalAllocator {
    unsafe fn alloc(&self, layout: core::alloc::Layout) -> Result<*mut u8, spacewasm::AllocError> {
        if let Some(alloc) = self.alloc {
            let ptr = unsafe { alloc(self.userdata, layout.size(), layout.align()) };
            // A NULL page allocation means the C backend is out of memory.
            if ptr.is_null() {
                Err(spacewasm::AllocError::OutOfMemory)
            } else {
                Ok(ptr)
            }
        } else {
            panic!("allocator not registered");
        }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, layout: core::alloc::Layout) {
        if let Some(dealloc) = self.dealloc {
            unsafe { dealloc(self.userdata, ptr, layout.size(), layout.align()) };
        } else {
            panic!("allocator not registered");
        }
    }

    fn memory_statistics(&self) -> spacewasm_memory_statistics_t {
        unreachable!()
    }
}

struct SequencerPageAllocator(spacewasm::PageAllocator<FprimeWasmGlobalAllocator, MAX_PAGES>);

unsafe impl Sync for SequencerPageAllocator {}

static ALLOCATORS: [SequencerPageAllocator; MAX_SEQUENCERS] = [const {
    SequencerPageAllocator(spacewasm::PageAllocator::new(
        FprimeWasmGlobalAllocator::new(),
        PAGE_SIZE,
    ))
}; MAX_SEQUENCERS];

static CURRENT_ALLOCATOR: AtomicUsize = AtomicUsize::new(usize::MAX);

struct SwapableAllocator {}

impl SwapableAllocator {
    fn get(&self) -> &SequencerPageAllocator {
        if let Some(alloc) = ALLOCATORS.get(CURRENT_ALLOCATOR.load(Ordering::Relaxed)) {
            alloc
        } else {
            panic!("no allocator set")
        }
    }
}

unsafe impl spacewasm::Allocator for SwapableAllocator {
    unsafe fn alloc(&self, layout: core::alloc::Layout) -> Result<*mut u8, spacewasm::AllocError> {
        unsafe { self.get().0.alloc(layout) }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, layout: core::alloc::Layout) {
        unsafe { self.get().0.dealloc(ptr, layout) }
    }

    fn memory_statistics(&self) -> spacewasm_memory_statistics_t {
        self.get().0.memory_statistics()
    }
}

spacewasm::global_allocator!(SwapableAllocator, SwapableAllocator {});

/// Register one of [MAX_SEQUENCERS] allocators to define a page group.
/// While there can only be a single "active" global allocator, multiple
/// concurent allocators can exist.
///
/// This function will fill one of the global allocator slots key-ed on
/// the [userdata] pointer. The alloc and dealloc function pointers should
/// be non-null (or BAD_ARG is returned).
///
/// If no free slot could be found, return ERR_CAPACITY
#[unsafe(no_mangle)]
pub extern "C" fn spacewasm_fprime_register_global_allocator(
    alloc: SpacewasmGlobalAllocFnT,
    dealloc: SpacewasmGlobalDeallocFnT,
    userdata: *mut c_void,
) -> spacewasm_status_t {
    let (Some(alloc), Some(dealloc)) = (alloc, dealloc) else {
        return spacewasm_status_t::SPACEWASM_ERR_BAD_ARG; // a null callback
    };

    // Find a free slot
    match ALLOCATORS
        .iter()
        .find(|alloc| alloc.0.with_inner(|a| a.userdata.is_null()))
    {
        // Fill the free slot
        Some(slot) => {
            slot.0.with_inner(|a| {
                a.alloc = Some(alloc);
                a.dealloc = Some(dealloc);
                a.userdata = userdata
            });
            spacewasm_status_t::SPACEWASM_OK
        }
        None => spacewasm_status_t::SPACEWASM_ERR_CAPACITY,
    }
}

/// Release the slot previously claimed by [spacewasm_fprime_register_global_allocator]
/// for [userdata]. The slot is returned to the free pool so it can be reused by a
/// later registration; without this a process that constructs more than
/// [MAX_SEQUENCERS] sequencers over its lifetime would exhaust the registry.
///
/// If this slot is the current active allocator, the active selection is cleared.
///
/// Returns NOT_FOUND if no slot is keyed on [userdata].
#[unsafe(no_mangle)]
pub extern "C" fn spacewasm_fprime_deregister_global_allocator(
    userdata: *mut c_void,
) -> spacewasm_status_t {
    match ALLOCATORS
        .iter()
        .enumerate()
        .find(|(_, alloc): &(usize, &SequencerPageAllocator)| {
            alloc.0.with_inner(|a| a.userdata == userdata)
        }) {
        Some((i, slot)) => {
            // If this slot is currently active, relinquish it first.
            let _ = CURRENT_ALLOCATOR.compare_exchange(
                i,
                usize::MAX,
                Ordering::Relaxed,
                Ordering::Relaxed,
            );
            slot.0.with_inner(|a| {
                a.alloc = None;
                a.dealloc = None;
                a.userdata = null_mut();
            });
            spacewasm_status_t::SPACEWASM_OK
        }
        None => spacewasm_status_t::SPACEWASM_ERR_NOT_FOUND,
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn spacewasm_fprime_acquire_global_allocator(
    userdata: *mut c_void,
) -> spacewasm_status_t {
    // Find the registered slot given a component pointer
    if let Some((i, _)) =
        ALLOCATORS
            .iter()
            .enumerate()
            .find(|(_, alloc): &(usize, &SequencerPageAllocator)| {
                alloc.0.with_inner(|a| a.userdata == userdata)
            })
    {
        match CURRENT_ALLOCATOR.compare_exchange(
            usize::MAX,
            i,
            Ordering::Relaxed,
            Ordering::Relaxed,
        ) {
            Ok(_) => spacewasm_status_t::SPACEWASM_OK,
            // Someone already has the allocator
            Err(_) => spacewasm_status_t::SPACEWASM_ERR_WRONG_STATE,
        }
    } else {
        spacewasm_status_t::SPACEWASM_ERR_NOT_FOUND
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn spacewasm_fprime_release_global_allocator(
    userdata: *mut c_void,
) -> spacewasm_status_t {
    if let Some((i, _)) =
        ALLOCATORS
            .iter()
            .enumerate()
            .find(|(_, alloc): &(usize, &SequencerPageAllocator)| {
                alloc.0.with_inner(|a| a.userdata == userdata)
            })
    {
        match CURRENT_ALLOCATOR.compare_exchange(
            i,
            usize::MAX,
            Ordering::Relaxed,
            Ordering::Relaxed,
        ) {
            Ok(_) => spacewasm_status_t::SPACEWASM_OK,
            // A different allocator took this atomic
            Err(_) => spacewasm_status_t::SPACEWASM_ERR_WRONG_STATE,
        }
    } else {
        spacewasm_status_t::SPACEWASM_ERR_NOT_FOUND
    }
}
