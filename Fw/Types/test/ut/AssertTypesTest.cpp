#include <gtest/gtest.h>
#include <config/FpConfig.hpp>

/**
 * \file AssertTypesTest.h
 * \author Vince Woo
 * \brief Tests for FW_ASSERT_LEVEL fall back when ASSERT_FILE_ID or
 *        ASSERT_RELATIVE_PATH are missing.
 */

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#undef ASSERT_FILE_ID
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT
#undef ASSERT_RELATIVE_PATH
#endif

#include <Fw/Types/Assert.hpp>

#include "Fw/Types/test/ut/TestAssertHook.hpp"

using Fw::TestAssertHook;

//
TEST(AssertTypesTest, FileDefaultTest) {
    // register the class
    TestAssertHook hook;
    hook.registerHook();

    // issue an assert
    FW_ASSERT(0);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    // ASSERT_FILE_ID was undefined above, it should have defaulted to 0
    ASSERT_EQ(0, hook.getFile());
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT
    // ASSERT_RELATIVE_PATH was undefined above, it should have defaulted
    // to the full path
    ASSERT_EQ(__FILE__, hook.getFile());
#endif
}
