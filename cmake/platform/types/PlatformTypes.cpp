/**
 * \brief PlatformTypes.cpp provides fallback defaults for the platform types
 *
 * This fill contains default implementations for types typically defined in
 * PlatformTypes.hpp. These default implementations are based on x86_64 Linux
 * but are often appropriate for most systems.
 */
#include <PlatformTypes.hpp>

const PlatformIntType PlatformIntType_MIN = std::numeric_limits<int>::min();
const PlatformIntType PlatformIntType_MAX = std::numeric_limits<int>::max();

const PlatformUIntType PlatformUIntType_MIN = std::numeric_limits<unsigned int>::min();
const PlatformUIntType PlatformUIntType_MAX = std::numeric_limits<unsigned int>::max();

const PlatformIndexType PlatformIndexType_MIN = PlatformIntType_MIN;
const PlatformIndexType PlatformIndexType_MAX = PlatformIntType_MAX;

const PlatformSizeType PlatformSizeType_MIN = PlatformUIntType_MIN;
const PlatformSizeType PlatformSizeType_MAX = PlatformUIntType_MAX;

const PlatformAssertArgType PlatformAssertArgType_MIN = PlatformIntType_MIN;
const PlatformAssertArgType PlatformAssertArgType_MAX = PlatformIntType_MAX;

// Pointer sizes are determined by size of compiler
#if __SIZEOF_POINTER__ == 8
    const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint64_t>::min();
    const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint64_t>::max();
#elif __SIZEOF_POINTER__ == 4
    const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint32_t>::min();
    const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint32_t>::max();
#elif __SIZEOF_POINTER__ == 2
    const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint16_t>::min();
    const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint16_t>::max();
#else
    const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint8_t>::min();
    const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint8_t>::max();
#endif
