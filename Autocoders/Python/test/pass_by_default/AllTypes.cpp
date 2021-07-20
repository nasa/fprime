#include "gtest/gtest.h"

#include <Autocoders/Python/test/pass_by_default/Port1PortAc.hpp>
#include <Autocoders/Python/test/pass_by_default/AllTypes.hpp>

namespace Example {


    AllTypes::AllTypes(bool is_async) {
        // Instantiate argument values
        arg1 = 0;
        arg2 = 0;
        arg3 = 0;
        arg4 = 0;
        arg5.setstate(0);
        arg6.setstate(0);
        arg7.setstate(0);
        arg8.setstate(0);
        // Store whether port is async
        this->is_async = is_async;
    }

    AllTypes::~AllTypes(void) {

    }

    void AllTypes::checkAsserts(void) {
        // Check output from port invocations worked

        // Value arguments result in current state being 0
        U8 value_expected = 0;
        // Pointer arguments result in current state being 1
        U8 pointer_expected = 1;
        // Const reference arguments result in current state being 0
        U8 constref_expected = 0;
        // If reference and sync, state will be 1
        // If reference and async, state will be 0
        U8 reference_expected = (this->is_async) ? 0 : 1;

        // Check output values
        ASSERT_EQ(this->arg1, pointer_expected);
        ASSERT_EQ(this->arg2, reference_expected);
        ASSERT_EQ(this->arg3, value_expected);
        ASSERT_EQ(this->arg4, value_expected);
        ASSERT_EQ((this->arg5).getstate(), pointer_expected);
        ASSERT_EQ((this->arg6).getstate(), reference_expected);
        ASSERT_EQ((this->arg7).getstate(), value_expected);
        ASSERT_EQ((this->arg8).getstate(), constref_expected);
    }

}
