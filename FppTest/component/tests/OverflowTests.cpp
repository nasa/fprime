// ======================================================================
// \title  OverflowTests.cpp
// \author mstarch
// \brief  cpp file for overflow tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Time/Time.hpp"
#include "STest/Pick/Pick.hpp"
#include "Tester.hpp"

// ----------------------------------------------------------------------
// Overflow assert test
// ----------------------------------------------------------------------

void Tester ::testOverflowAssert() {
    FormalParamEnum x = FormalParamEnum::T::X;
    FormalParamEnum y = FormalParamEnum::T::Y;
    FormalParamEnum z = FormalParamEnum::T::Z;

    for (FwSizeType i = 0; i < Tester::TEST_INSTANCE_QUEUE_DEPTH; i++) {
        this->invoke_to_enumArgsAsync(i % 2, x, y);
    }
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_enumArgsAsync(0, y, z), "");
}

// ----------------------------------------------------------------------
// Overflow drop test
// ----------------------------------------------------------------------

void Tester ::testOverflowDrop() {
    FormalParamStruct x;
    FormalParamStruct y;
    FormalParamStruct z;
    for (FwSizeType i = 0; i < Tester::TEST_INSTANCE_QUEUE_DEPTH; i++) {
        this->invoke_to_structArgsAsync(i % 2, x, y);
    }
    // This will overflow and should not crash
    this->invoke_to_structArgsAsync(0, y, z);
}

// ----------------------------------------------------------------------
// Overflow hook test
// ----------------------------------------------------------------------

void Tester ::testOverflowHook() {
    FormalParamEnum x = FormalParamEnum::T::X;
    FormalParamEnum y = FormalParamEnum::T::Y;
    FormalParamEnum z = FormalParamEnum::T::Z;

    for (FwSizeType i = 0; i < Tester::TEST_INSTANCE_QUEUE_DEPTH; i++) {
        this->invoke_to_enumArgsHook(i % 2, x, y);
    }
    this->invoke_to_enumArgsHook(0, y, z);
    this->invoke_to_enumArgsHook(1, z, x);
    ASSERT_from_enumArgsHookOverflowed_SIZE(2);
    ASSERT_from_enumArgsHookOverflowed(0, y, z);
    ASSERT_from_enumArgsHookOverflowed(1, z, x);

}

// ----------------------------------------------------------------------
// Handler to support overflow hook test
// ----------------------------------------------------------------------

void Tester ::from_enumArgsHookOverflowed_handler(const NATIVE_INT_TYPE portNum,
                                                  const FormalParamEnum& en,
                                                  FormalParamEnum& enRef) {
    this->pushFromPortEntry_enumArgsHookOverflowed(en, enRef);
}
