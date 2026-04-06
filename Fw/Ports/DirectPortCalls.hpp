/**
 * \file DirectPortCalls.hpp
 * \author F Prime Architecture Team
 * \brief Direct port calls support for constrained systems
 *
 * This header provides the infrastructure for direct port calls when
 * FW_DIRECT_PORT_CALLS is enabled. Direct port calls eliminate dynamic
 * dispatch overhead by using compile-time knowledge of port connections.
 *
 * \copyright
 * Copyright 2009-2026, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */

#ifndef FW_DIRECT_PORT_CALLS_HPP
#define FW_DIRECT_PORT_CALLS_HPP

#include "FpConfig.h"

#if FW_DIRECT_PORT_CALLS

#include "Fw/Types/BasicTypes.h"

namespace Fw {

/**
 * \class DirectPortDispatcher
 * \brief Base class for direct port call dispatching
 *
 * When FW_DIRECT_PORT_CALLS is enabled, the topology generates a dispatcher
 * for each output port that uses switch statements to directly invoke receiver
 * handlers instead of going through the port object indirection.
 */
class DirectPortDispatcher {
  public:
    DirectPortDispatcher() = default;
    virtual ~DirectPortDispatcher() = default;

  protected:
    /**
     * Helper to assert that a port is connected
     * \param portNum the port index
     * \param isConnected whether the port is connected
     */
    static void assertPortConnected(FwIndexType portNum, bool isConnected) {
        FW_ASSERT(isConnected, portNum);
    }
};

/**
 * \class DirectPortCallRegistry
 * \brief Registry for direct port call dispatchers
 *
 * Optional registry that can be used to track all direct port call dispatchers
 * in a deployment for debugging and monitoring purposes.
 */
class DirectPortCallRegistry {
  public:
    static DirectPortCallRegistry& instance();

    /**
     * Register a direct port call dispatcher
     * \param componentName name of the component
     * \param portName name of the output port
     * \param dispatcher pointer to the dispatcher
     */
    void registerDispatcher(const char* componentName,
                            const char* portName,
                            DirectPortDispatcher* dispatcher);

    /**
     * Get number of registered dispatchers
     * \return number of registered dispatchers
     */
    U32 getDispatcherCount() const;

  private:
    DirectPortCallRegistry();
    ~DirectPortCallRegistry();

    // Disabled copy/assignment
    DirectPortCallRegistry(const DirectPortCallRegistry&) = delete;
    DirectPortCallRegistry& operator=(const DirectPortCallRegistry&) = delete;

    static DirectPortCallRegistry* s_instance;
    static const U32 MAX_DISPATCHERS = 256;

    struct DispatcherEntry {
        const char* componentName;
        const char* portName;
        DirectPortDispatcher* dispatcher;
    } m_dispatchers[MAX_DISPATCHERS];

    U32 m_dispatcherCount;
};

}  // namespace Fw

#else  // !FW_DIRECT_PORT_CALLS

// When direct port calls are disabled, these are no-ops
namespace Fw {

class DirectPortDispatcher {
  public:
    DirectPortDispatcher() = default;
    virtual ~DirectPortDispatcher() = default;

  protected:
    static void assertPortConnected(FwIndexType portNum, bool isConnected) {}
};

class DirectPortCallRegistry {
  public:
    static DirectPortCallRegistry& instance();
    void registerDispatcher(const char* componentName,
                            const char* portName,
                            DirectPortDispatcher* dispatcher) {}
    U32 getDispatcherCount() const { return 0; }

  private:
    DirectPortCallRegistry();
    ~DirectPortCallRegistry();
    static DirectPortCallRegistry* s_instance;
};

}  // namespace Fw

#endif  // FW_DIRECT_PORT_CALLS

#endif  // FW_DIRECT_PORT_CALLS_HPP

