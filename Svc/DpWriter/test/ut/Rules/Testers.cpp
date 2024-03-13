// ======================================================================
// \title  Testers.cpp
// \author Rob Bocchino
// \brief  Testers class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "Svc/DpWriter/test/ut/Rules/Testers.hpp"

namespace Svc {

  namespace Testers {

    BufferSendIn::Tester bufferSendIn;

    FileOpenStatus::Tester fileOpenStatus;

    FileWriteStatus::Tester fileWriteStatus;

    SchedIn::Tester schedIn;

  }

}
