#include "gtest/gtest.h"

#include <Autocoders/Python/test/pass_by_kind/Port1PortAc.hpp>
#include <Autocoders/Python/test/pass_by_kind/AllTypes.hpp>

namespace Example {


    AllTypes::AllTypes(bool is_async) {
    	// Instantiate argument pointers
    	this->arg1 = new U32(0);
    	this->arg2 = new U32(0);
    	this->arg3 = new U32(0);
    	this->arg4 = new U32(0);
    	this->arg5 = new ExampleType((U32)0);
    	this->arg6 = new ExampleType((U32)0);
    	this->arg7 = new ExampleType((U32)0);
    	this->arg8 = new ExampleType((U32)0);
    	this->arg9 = new Example::Arg9String("hello");
    	this->arg10 = new Example::Arg10String("hello");
    	this->arg11 = new Example::Arg11String("hello");
    	this->arg12 = new Example::Arg12String("hello");
    	this->is_async = is_async;
    }

    AllTypes::~AllTypes(void) {
    	// Delete argument pointers
    	delete this->arg1;
    	delete this->arg2;
    	delete this->arg3;
    	delete this->arg4;
    	delete this->arg5;
    	delete this->arg6;
    	delete this->arg7;
    	delete this->arg8;
    	delete this->arg9;
    	delete this->arg10;
    	delete this->arg11;
    	delete this->arg12;
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
        ASSERT_EQ(*(this->arg1), pointer_expected);
        ASSERT_EQ(*(this->arg2), reference_expected);
        ASSERT_EQ(*(this->arg3), value_expected);
        ASSERT_EQ(*(this->arg4), value_expected);
        ASSERT_EQ((this->arg5)->getstate(), pointer_expected);
        ASSERT_EQ((this->arg6)->getstate(), reference_expected);
        ASSERT_EQ((this->arg7)->getstate(), value_expected);
        ASSERT_EQ((this->arg8)->getstate(), constref_expected);
    }

}
