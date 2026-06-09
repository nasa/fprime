// ======================================================================
// \title  TestState.hpp
// \author Gerik Kubiak
// \brief  Test state for testing DpCompressProc
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_TestState_HPP
#define Svc_TestState_HPP

#include "Svc/DpCompressProc/test/ut/DpCompressProcTester.hpp"

#define TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)          \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

namespace Svc {

class TestState : public DpCompressProcTester {
  public:
    // ----------------------------------------------------------------------
    // Rule definitions
    // ----------------------------------------------------------------------

    TEST_STATE_DEF_RULE(ProcRequest, Compressible)
};

}  // namespace Svc

#endif
