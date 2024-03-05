// ======================================================================
// \title  Rules.hpp
// \author Rob Bocchino
// \brief  Rules for testing DpManager
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_Rules_HPP
#define Svc_Rules_HPP

#include "STest/Rule/Rule.hpp"

#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

#define RULES_DEF_RULE(GROUP_NAME, RULE_NAME)                                                                   \
    namespace GROUP_NAME {                                                                                      \
                                                                                                                \
    struct RULE_NAME : public STest::Rule<TestState> {                                                          \
        RULE_NAME() : Rule<TestState>(#GROUP_NAME "." #RULE_NAME) {}                                            \
                                                                                                                \
        bool precondition(const TestState& state) { return state.precondition__##GROUP_NAME##__##RULE_NAME(); } \
                                                                                                                \
        void action(TestState& state) { state.action__##GROUP_NAME##__##RULE_NAME(); }                          \
    };                                                                                                          \
    }

namespace Svc {

namespace Rules {

RULES_DEF_RULE(BufferGetStatus, Invalid)
RULES_DEF_RULE(BufferGetStatus, Valid)
RULES_DEF_RULE(CLEAR_EVENT_THROTTLE, OK)
RULES_DEF_RULE(ProductGetIn, BufferInvalid)
RULES_DEF_RULE(ProductGetIn, BufferValid)
RULES_DEF_RULE(ProductRequestIn, BufferInvalid)
RULES_DEF_RULE(ProductRequestIn, BufferValid)
RULES_DEF_RULE(ProductSendIn, OK)
RULES_DEF_RULE(SchedIn, OK)

}  // namespace Rules

}  // namespace Svc

#endif
