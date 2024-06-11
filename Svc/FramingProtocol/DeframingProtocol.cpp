// ======================================================================
// \title  DeframingProtocol.cpp
// \author mstarch
// \brief  cpp file for DeframingProtocol class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include "DeframingProtocol.hpp"
#include "DeframingProtocolInterface.hpp"

namespace Svc {

DeframingProtocol::DeframingProtocol() : m_interface(nullptr) {}

void DeframingProtocol::setup(DeframingProtocolInterface& interface) {
    FW_ASSERT(m_interface == nullptr);
    m_interface = &interface;
}
}
