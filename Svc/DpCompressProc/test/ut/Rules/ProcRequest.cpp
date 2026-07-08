// ======================================================================
// \title  ProcRequest.cpp
// \author Gerik Kubiak
// \brief  ProcRequest class implementation
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include <limits>
#include <string>

#include "Os/Stub/test/File.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpCompressProc/test/ut/Rules/ProcRequest.hpp"
#include "Svc/DpCompressProc/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__ProcRequest__Compressible() const {
    return true;
}

void TestState ::action__ProcRequest__Compressible() {
    this->clearHistory();
    if (abstractState.param_enabled_) {
        test_chunks(abstractState.chunk_size_, abstractState.chunks_);
    } else {
        test_chunks_disabled(abstractState.chunk_size_, abstractState.chunks_);
    }
}

namespace ProcRequest {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::CompressTest() {
    this->ruleCompressible.apply(this->testState);
    // this->testState.printEvents();
}

}  // namespace ProcRequest

}  // namespace Svc
