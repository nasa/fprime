// ======================================================================
// \title  ProcRequest.hpp
// \author Gerik Kubiak
// \brief  ProcRequest class interface
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_BufferSendIn_HPP
#define Svc_BufferSendIn_HPP

#include "Svc/DpCompressProc/test/ut/Rules/Rules.hpp"
#include "Svc/DpCompressProc/test/ut/TestState/TestState.hpp"

namespace Svc {

namespace ProcRequest {

class Tester {
  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Compressible
    void CompressTest();

  public:
    // ----------------------------------------------------------------------
    // Rules
    // ----------------------------------------------------------------------

    //! Rule BufferSendIn::OK
    Rules::ProcRequest::Compressible ruleCompressible;

  public:
    // ----------------------------------------------------------------------
    // Public member variables
    // ----------------------------------------------------------------------

    //! Test state
    TestState testState;
};

}  // namespace ProcRequest

}  // namespace Svc

#endif
