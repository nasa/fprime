#![cfg_attr(target_os = "none", no_std)]

mod allocator;
mod store;

#[allow(non_camel_case_types, non_snake_case, dead_code)]
mod ffi;

#[cfg(target_os = "none")]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    // TODO(tumbar) We should have a graceful handler for this
    // I.e. call into the WasmSeqeuencer which should emit WARNING_HI and reset the Rust state
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn fprime_spacewasm_probe() -> u32 {
    42
}
