#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <App2/PartitionHubPartitionHubComponentAc.hpp>

#include <Fw/Cfg/FwConfig.hpp>
#include <Fw/Types/FwAssert.hpp>

#include <Fw/Types/FwStringType.hpp>
#include <Fw/Types/FwEightyCharString.hpp>
#include <Fw/Port/FwInputSerializePort.hpp>
#include <Fw/Port/FwOutputSerializePort.hpp>

namespace PartitionHub {

    Fw::InputSerializePort *PartitionHubBase::getPortASerializeInputPort(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum < this->m_getNumPortASerializeInputPorts());
        return &this->m_PortASerializeInputPort[portNum];
    }

    void PartitionHubBase::setPortBSerializeOutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort* port) {
        FW_ASSERT(portNum < this->m_getNumPortBSerializeOutputPorts());
        return this->m_PortBSerializeOutputPort[portNum].registerSerialPort(port);
    }

    void PartitionHubBase::setPortBSerializeOutputPort(NATIVE_INT_TYPE portNum, Fw::InputPortBase *port) {
        FW_ASSERT(portNum < this->m_getNumPortBSerializeOutputPorts());
        return this->m_PortBSerializeOutputPort[portNum].registerSerialPort(port);
    }

#if FW_OBJECT_NAMES == 1
    PartitionHubBase::PartitionHubBase(const char* compName) : Fw::ComponentBase(compName) {
    };
#else
    PartitionHubBase::PartitionHubBase() : Fw::ComponentBase() {
    };
#endif

    void PartitionHubBase::init(NATIVE_INT_TYPE queueDepth) {
        // Input ports attached to component here with external component interfaces
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNumPortASerializeInputPorts(); port++) {
            this->m_PortASerializeInputPort[port].init();
            this->m_PortASerializeInputPort[port].registerSerialComponent(this);
            this->m_PortASerializeInputPort[port].setPortNum(port);
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_PortASerializeInputPort_%d", this->m_objName, port);
            this->m_PortASerializeInputPort[port].setObjName(portName);
#endif
        }

        // Set output ports
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNumPortBSerializeOutputPorts(); port++) {
            this->m_PortBSerializeOutputPort[port].init();
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_PortBSerializeOutputPort_%d", this->m_objName, port);
            this->m_PortBSerializeOutputPort[port].setObjName(portName);
#endif
        }
	}


	PartitionHubBase::~PartitionHubBase(void) {
	}

    // Up-calls, calls for output ports
    void PartitionHubBase::PortB_Serialize_out(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer) {
        FW_ASSERT(portNum < this->m_getNumPortBSerializeOutputPorts());
        m_PortBSerializeOutputPort[portNum].invokeSerial(Buffer);
    }

    NATIVE_INT_TYPE PartitionHubBase::m_getNumPortASerializeInputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_PortASerializeInputPort);
    }

    NATIVE_INT_TYPE PartitionHubBase::m_getNumPortBSerializeOutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_PortBSerializeOutputPort);
    }

    // call for incoming serialized ports
    void PartitionHubBase::invokeSerial(I32 portNum, Fw::SerializeBufferBase &Buffer) {

        // Make the down-call to the implementation
    	assert(portNum < 2);
        this->PortA_Serialize_handler(portNum, Buffer);
    }

}

