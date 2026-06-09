// ======================================================================
// \title  Rules.hpp
// \author Gerik Kubiak
// \brief  Rules for testing DpCompressProc
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Rules_HPP
#define Svc_Rules_HPP

#include "STest/Rule/Rule.hpp"

#include "Svc/DpCompressProc/test/ut/TestState/TestState.hpp"

#define RULES_DEF_RULE(GROUP_NAME, RULE_NAME)                         \
    namespace GROUP_NAME {                                            \
                                                                      \
    struct RULE_NAME : public STest::Rule<TestState> {                \
        RULE_NAME() : Rule<TestState>(#GROUP_NAME "." #RULE_NAME) {}  \
                                                                      \
        bool precondition(const TestState& state) {                   \
            return state.precondition__##GROUP_NAME##__##RULE_NAME(); \
        }                                                             \
                                                                      \
        void action(TestState& state) {                               \
            state.action__##GROUP_NAME##__##RULE_NAME();              \
        }                                                             \
    };                                                                \
    }

namespace Svc {

namespace Rules {

RULES_DEF_RULE(ProcRequest, Compressible)

}  // namespace Rules

}  // namespace Svc

#endif
