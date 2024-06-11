// ======================================================================
// \title  Testers.hpp
// \author Rob Bocchino
// \brief  Testers class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Testers_HPP
#define Svc_Testers_HPP

#include "Svc/DpManager/test/ut/Rules/BufferGetStatus.hpp"
#include "Svc/DpManager/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductGetIn.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductRequestIn.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductSendIn.hpp"
#include "Svc/DpManager/test/ut/Rules/SchedIn.hpp"

namespace Svc {

  namespace Testers {

    extern BufferGetStatus::Tester bufferGetStatus;

    extern CLEAR_EVENT_THROTTLE::Tester clearEventThrottle;

    extern ProductGetIn::Tester productGetIn;

    extern ProductRequestIn::Tester productRequestIn;

    extern ProductSendIn::Tester productSendIn;

    extern SchedIn::Tester schedIn;

  }

}

#endif
