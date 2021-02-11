//
// Created by Starch, Michael D (348C) on 12/30/20.
//

#include "DeframingProtocol.hpp"
#include "DeframingProtocolInterface.hpp"

namespace Svc {

DeframingProtocol::DeframingProtocol() : m_interface(NULL) {}

void DeframingProtocol::setup(DeframingProtocolInterface& interface) {
    FW_ASSERT(m_interface == NULL);
    m_interface = &interface;
}
};