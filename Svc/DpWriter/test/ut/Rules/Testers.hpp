// ======================================================================
// \title  Testers.hpp
// \author Rob Bocchino
// \brief  Testers class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_Testers_HPP
#define Svc_Testers_HPP

#include "Svc/DpWriter/test/ut/Rules/BufferSendIn.hpp"
#include "Svc/DpWriter/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
#include "Svc/DpWriter/test/ut/Rules/FileOpenStatus.hpp"
#include "Svc/DpWriter/test/ut/Rules/FileWriteStatus.hpp"
#include "Svc/DpWriter/test/ut/Rules/SchedIn.hpp"

namespace Svc {

  namespace Testers {

    extern BufferSendIn::Tester bufferSendIn;

    extern CLEAR_EVENT_THROTTLE::Tester clearEventThrottle;

    extern FileOpenStatus::Tester fileOpenStatus;

    extern FileWriteStatus::Tester fileWriteStatus;

    extern SchedIn::Tester schedIn;

  }

}

#endif
