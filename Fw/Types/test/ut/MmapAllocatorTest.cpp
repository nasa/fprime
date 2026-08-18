
#include <gtest/gtest.h>

#include <Fw/Types/MmapAllocator.hpp>
#include <config/FpConfig.hpp>

#include <unistd.h>

// Test that all available advertized allocation can be made
TEST(MmapAlloc, AllocAll) {
    constexpr FwSizeType AllocCount = 4;
    Fw::MmapAllocator<AllocCount> alloc;

    // All
    void* ptrs[AllocCount] = {nullptr};
    for (FwSizeType i = 0; i < AllocCount; i++) {
        bool recoverable = false;
        FwSizeType alloc_size = 4096 * (i + 1);
        ptrs[i] = alloc.allocate(static_cast<FwEnumStoreType>(i), alloc_size, recoverable);
        ASSERT_NE(ptrs[i], nullptr);
        ASSERT_EQ(alloc_size, 4096 * (i + 1));
        ASSERT_FALSE(recoverable);
    }

    // Expect graceful failure to allocate due to too many allocations
    bool recoverable = false;
    FwSizeType alloc_size = 4096;
    void* bad_ptr = alloc.allocate(0, alloc_size, recoverable);
    ASSERT_EQ(bad_ptr, nullptr);
    ASSERT_EQ(alloc_size, 0);

    // Deallocate pointers
    for (FwSizeType i = 0; i < AllocCount; i++) {
        alloc.deallocate(static_cast<FwEnumStoreType>(i), ptrs[i]);
        ptrs[i] = nullptr;
    }

    // Reallocate and deallocate one memory region now that
    // entries in the size_map should be free
    alloc_size = 4096;
    ptrs[0] = alloc.allocate(0, alloc_size, recoverable);
    ASSERT_NE(ptrs[0], nullptr);
    ASSERT_EQ(alloc_size, 4096);

    alloc.deallocate(0, ptrs[0]);
}

// Check that bad deallocates assert
TEST(MmapAlloc, BadFree) {
    Fw::MmapAllocator<4> alloc;
    EXPECT_DEATH({ alloc.deallocate(0, reinterpret_cast<void*>(0x1000)); }, ".*");
}

// Check the maximum allowed alignment
TEST(MmapAlloc, BadAlignment) {
    Fw::MmapAllocator<4> alloc;

    FwSizeType max_align = static_cast<FwSizeType>(sysconf(_SC_PAGESIZE));
    FwSizeType alloc_size = 4096;
    bool recoverable = false;

    void* ptr = alloc.allocate(0, alloc_size, recoverable, max_align * 2);
    ASSERT_EQ(ptr, nullptr);
    ASSERT_EQ(alloc_size, 0);
}
