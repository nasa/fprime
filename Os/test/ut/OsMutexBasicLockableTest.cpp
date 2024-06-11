#include <gtest/gtest.h>

#include <Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>

#include <mutex>


// This is exclusively a compile-time check
void testMutexBasicLockableTest() {
  Os::Mutex mux;

  {
    std::lock_guard<Os::Mutex> lock(mux);
  }

  ASSERT_TRUE(true); // if runs will pass
}

extern "C" {
  void mutexBasicLockableTest();
}

void mutexBasicLockableTest() {
  testMutexBasicLockableTest();
}
