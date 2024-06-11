// ======================================================================
// \title  Rules.hpp
// \author Rob Bocchino
// \brief  Rules for testing DpWriter
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Rules_HPP
#define Svc_Rules_HPP

#include "STest/Rule/Rule.hpp"

#include "Svc/DpWriter/test/ut/TestState/TestState.hpp"

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

RULES_DEF_RULE(BufferSendIn, BufferTooSmallForData)
RULES_DEF_RULE(BufferSendIn, BufferTooSmallForPacket)
RULES_DEF_RULE(BufferSendIn, FileOpenError)
RULES_DEF_RULE(BufferSendIn, FileWriteError)
RULES_DEF_RULE(BufferSendIn, InvalidBuffer)
RULES_DEF_RULE(BufferSendIn, InvalidHeader)
RULES_DEF_RULE(BufferSendIn, InvalidHeaderHash)
RULES_DEF_RULE(BufferSendIn, OK)
RULES_DEF_RULE(CLEAR_EVENT_THROTTLE, OK)
RULES_DEF_RULE(FileOpenStatus, Error)
RULES_DEF_RULE(FileOpenStatus, OK)
RULES_DEF_RULE(FileWriteStatus, Error)
RULES_DEF_RULE(FileWriteStatus, OK)
RULES_DEF_RULE(SchedIn, OK)

}  // namespace Rules

}  // namespace Svc

#endif
