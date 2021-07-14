// ======================================================================
// \title  FramingProtocol.cpp
// \author mstarch
// \brief  cpp file for FramingProtocol class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include "FramingProtocol.hpp"
#include "FramingProtocolInterface.hpp"

namespace Svc {

FramingProtocol::FramingProtocol() : m_interface(NULL) {}

void FramingProtocol::setup(FramingProtocolInterface& interface) {
    FW_ASSERT(m_interface == NULL);
    m_interface = &interface;
}
};
