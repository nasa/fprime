// ======================================================================
// \title  TestState.hpp
// \author Rob Bocchino
// \brief  Test state for testing DpManager
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_TestState_HPP
#define Svc_TestState_HPP

#include "Svc/DpManager/test/ut/DpManagerTester.hpp"

#define TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)          \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

namespace Svc {

class TestState : public DpManagerTester {
  public:
    // ----------------------------------------------------------------------
    // Rule definitions
    // ----------------------------------------------------------------------

    TEST_STATE_DEF_RULE(BufferGetStatus, Invalid)
    TEST_STATE_DEF_RULE(BufferGetStatus, Valid)
    TEST_STATE_DEF_RULE(CLEAR_EVENT_THROTTLE, OK)
    TEST_STATE_DEF_RULE(ProductGetIn, BufferInvalid)
    TEST_STATE_DEF_RULE(ProductGetIn, BufferValid)
    TEST_STATE_DEF_RULE(ProductRequestIn, BufferInvalid)
    TEST_STATE_DEF_RULE(ProductRequestIn, BufferValid)
    TEST_STATE_DEF_RULE(ProductSendIn, OK)
    TEST_STATE_DEF_RULE(SchedIn, OK)

  public:
    // ----------------------------------------------------------------------
    // Non-rule tests
    // ----------------------------------------------------------------------

    //! Test that a full queue drops the DP, records it, and returns its buffer (no assert)
    void testProductSendInOverflowHook();

    //! Overflow hook for productRequestIn: responds with an invalid buffer + FAILURE, never hangs
    void testProductRequestInOverflowHook();
};

}  // namespace Svc

#endif
