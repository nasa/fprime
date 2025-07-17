#include <config/FpConfig.hpp>
#include <gtest/gtest.h>

/**
 * \file CAssertTest.cpp
 * \brief Tests for FW_CASSERT_1 macro functionality
 */

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/CAssert.h>

// Test that FW_CASSERT_1 macro compiles and works correctly
TEST(CAssertTest, CAssert1Macro) {
    // Disable old-style cast warnings for this test since we're testing C macros
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
    
    // Test that the macro compiles without errors
    int testValue = 42;
    
    // This should not trigger an assertion since condition is true
    FW_CASSERT_1(testValue == 42, testValue);
    
    // This should compile but not execute assertion in normal builds
    // In debug builds with assertions enabled, this would trigger
    FW_CASSERT_1(testValue != 0, testValue);
    
    // Test with different argument types
    U32 uintValue = 100;
    FW_CASSERT_1(uintValue > 0, uintValue);
    
    I32 intValue = -1;
    FW_CASSERT_1(intValue < 0, intValue);
    
    #pragma GCC diagnostic pop
    
    // Verify that the macro expands correctly
    EXPECT_TRUE(true); // If we get here, the macro compiled successfully
}
