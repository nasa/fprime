# F´ and CMake Platforms

Users can create platform-specific build files for the purposes of tailoring fprime
for given platform targets. Any CMake toolchain file should work, but it will require a platform file created here to add target-specific configuration using the name "${FPRIME_PLATFORM}.cmake".

Platforms should register a configuration module using `register_fprime_config` that sets the `AUTOCODER_INPUTS`, `HEADERS` and
`CHOOSES_IMPLEMENTATIONS` directives.

`AUTOCODER_INPUTS`: must include one .fpp file defining the platform's [platform types](../../reference/numerical-types.md#platform-configured-types)
`HEADERS`: lists the `PlatformTypes.h` header defining `PlatformPointerCastType`
`CHOOSES_IMPLEMENTATIONS`: lists all implementations chosen for the current platform. See: [CMake Implementations](./cmake-implementations.md).
