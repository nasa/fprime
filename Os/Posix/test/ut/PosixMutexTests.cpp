// ======================================================================
// \title Os/Posix/test/ut/PosixMutexTests.cpp
// \brief tests for posix implementation for Os::Mutex
// ======================================================================
#include <gtest/gtest.h>
#include <unistd.h>
#include <list>
#include "Os/Mutex.hpp"
#include "Os/Posix/Mutex.hpp"
// #include "Os/test/ut/file/CommonTests.hpp"
#include "STest/Pick/Pick.hpp"
#include <cstdio>
#include <csignal>
namespace Os {
namespace Test {
namespace Mutex {

}  // namespace Mutex
}  // namespace Test
}  // namespace Os

int main(int argc, char** argv) {
    // STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
