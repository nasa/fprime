// ======================================================================
// \title Os/Stub/test/atomic_test.hpp
// \brief test-specific header for atomic stub testing
// ======================================================================
#ifndef OS_STUB_TEST_ATOMIC_TEST_HPP
#define OS_STUB_TEST_ATOMIC_TEST_HPP

// Create a test namespace that contains our atomic stub implementation
// This allows us to test the stub on any platform without namespace conflicts
namespace TestStub {
    namespace std {
        #include "Os/Stub/atomic/atomic_impl.hpp"
    } // namespace std
} // namespace TestStub

#endif // OS_STUB_TEST_ATOMIC_TEST_HPP
