/**
 * \file DirectPortCalls.cpp
 * \author F Prime Architecture Team
 * \brief Direct port calls support implementation
 *
 * \copyright
 * Copyright 2009-2026, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */

#include "Fw/Ports/DirectPortCalls.hpp"

#if FW_DIRECT_PORT_CALLS

namespace Fw {

// Static instance initialization
DirectPortCallRegistry* DirectPortCallRegistry::s_instance = nullptr;

DirectPortCallRegistry& DirectPortCallRegistry::instance() {
    if (s_instance == nullptr) {
        static DirectPortCallRegistry registry;
        s_instance = &registry;
    }
    return *s_instance;
}

DirectPortCallRegistry::DirectPortCallRegistry() : m_dispatcherCount(0) {
    // Initialize dispatcher array
    for (U32 i = 0; i < MAX_DISPATCHERS; ++i) {
        m_dispatchers[i].componentName = nullptr;
        m_dispatchers[i].portName = nullptr;
        m_dispatchers[i].dispatcher = nullptr;
    }
}

DirectPortCallRegistry::~DirectPortCallRegistry() {
    // Registry cleanup (dispatchers are owned by components)
}

void DirectPortCallRegistry::registerDispatcher(const char* componentName,
                                                const char* portName,
                                                DirectPortDispatcher* dispatcher) {
    if (m_dispatcherCount >= MAX_DISPATCHERS) {
        FW_ASSERT(false, m_dispatcherCount);
        return;
    }

    m_dispatchers[m_dispatcherCount].componentName = componentName;
    m_dispatchers[m_dispatcherCount].portName = portName;
    m_dispatchers[m_dispatcherCount].dispatcher = dispatcher;
    ++m_dispatcherCount;
}

U32 DirectPortCallRegistry::getDispatcherCount() const {
    return m_dispatcherCount;
}

}  // namespace Fw

#else  // !FW_DIRECT_PORT_CALLS

namespace Fw {

// Static instance initialization
DirectPortCallRegistry* DirectPortCallRegistry::s_instance = nullptr;

DirectPortCallRegistry& DirectPortCallRegistry::instance() {
    if (s_instance == nullptr) {
        static DirectPortCallRegistry registry;
        s_instance = &registry;
    }
    return *s_instance;
}

DirectPortCallRegistry::DirectPortCallRegistry() {}

DirectPortCallRegistry::~DirectPortCallRegistry() {}

}  // namespace Fw

#endif  // FW_DIRECT_PORT_CALLS

