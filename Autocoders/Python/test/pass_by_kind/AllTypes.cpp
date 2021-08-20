#include "gtest/gtest.h"

#include <Autocoders/Python/test/pass_by_kind/Port1PortAc.hpp>
#include <Autocoders/Python/test/pass_by_kind/AllTypes.hpp>

namespace Example {


    AllTypes::AllTypes(bool is_async) {
        // Instantiate argument values
        arg1 = 0;
        arg2 = 0;
        arg3 = 0;
        arg4.setstate(0);
        arg5.setstate(0);
        arg6.setstate(0);
        // Store whether port is async
        this->is_async = is_async;
    }

    AllTypes::~AllTypes(void) {

    }

    void AllTypes::checkAsserts(void) {
        // Check output from port invocations worked
        // Declare enum with possible port pass by logic
        enum output_options {VALUE = 0, POINTER = 1, CONST_REF = 2, REFERENCE = 3};
        // Declare array with expected output for above pass by logic
        // Value arguments result in current state being 0
        // Pointer arguments result in current state being 1
        // Const reference arguments result in current state being 0
        // If reference and sync, state will be 1
        // If reference and async, state will be 0
        int expected_output[] = {0, 1, 0, (this->is_async) ? 0 : 1};

        // Check output values
        ASSERT_EQ(this->arg1, expected_output[POINTER]);
        ASSERT_EQ(this->arg2, expected_output[REFERENCE]);
        ASSERT_EQ(this->arg3, expected_output[VALUE]);
        ASSERT_EQ((this->arg4).getstate(), expected_output[POINTER]);
        ASSERT_EQ((this->arg5).getstate(), expected_output[REFERENCE]);
        ASSERT_EQ((this->arg6).getstate(), expected_output[CONST_REF]);
    }

}
