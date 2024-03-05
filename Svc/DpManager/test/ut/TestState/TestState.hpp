// ======================================================================
// \title  TestState.hpp
// \author Rob Bocchino
// \brief  Test state for testing DpManager
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
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
};

}  // namespace Svc

#endif
