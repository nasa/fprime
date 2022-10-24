// ======================================================================
// \title  GenericRepeaterComponentImpl.cpp
// \author joshuaa
// \brief  cpp file for GenericRepeater component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/GenericRepeater/GenericRepeaterComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

GenericRepeaterComponentImpl ::GenericRepeaterComponentImpl(const char* const compName)
    : GenericRepeaterComponentBase(compName) {}

void GenericRepeaterComponentImpl ::init(const NATIVE_INT_TYPE instance) {
    GenericRepeaterComponentBase::init(instance);
}

GenericRepeaterComponentImpl ::~GenericRepeaterComponentImpl() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void GenericRepeaterComponentImpl ::portIn_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                                                   Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    for (NATIVE_INT_TYPE i = 0; i < NUM_PORTOUT_OUTPUT_PORTS; i++) {
        if (!isConnected_portOut_OutputPort(i)) {
            continue;
        }

        portOut_out(i, Buffer);
    }
}

}  // end namespace Svc
