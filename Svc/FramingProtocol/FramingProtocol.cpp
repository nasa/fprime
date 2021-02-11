//
// Created by Starch, Michael D (348C) on 12/30/20.
//

#include "FramingProtocol.hpp"
#include "FramingProtocolInterface.hpp"

namespace Svc {

FramingProtocol::FramingProtocol() : m_interface(NULL) {}

void FramingProtocol::setup(FramingProtocolInterface& interface) {
    FW_ASSERT(m_interface == NULL);
    m_interface = &interface;
}
};