#include <cstdio>
#include <FpConfig.hpp>
// The following header will need to be modified when test code is moved
// If the component tester is regenerated, this will need to be modified again.
// Make the compile fail to make sure it is changed
#include <Svc/PolyDb/test/ut/PolyDbComponentTestAc.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
// public methods
    void PolyDbTesterComponentBase::set_getValue_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputPolyPort* port) {
        FW_ASSERT(portNum < this->getNum_getValue_OutputPorts());
        this->m_getValue_OutputPort[portNum].addCallPort(port);
    }

    void PolyDbTesterComponentBase::set_setValue_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputPolyPort* port) {
        FW_ASSERT(portNum < this->getNum_setValue_OutputPorts());
        this->m_setValue_OutputPort[portNum].addCallPort(port);
    }

    PolyDbTesterComponentBase::PolyDbTesterComponentBase(const char* compName) : Fw::PassiveComponentBase(compName) {
    }

    PolyDbTesterComponentBase::~PolyDbTesterComponentBase() {
    }

    void PolyDbTesterComponentBase::init(NATIVE_INT_TYPE instance) {


	    // initialize base class
		Fw::PassiveComponentBase::init(instance);
        // Input ports attached to component here with external component interfaces

        // Set output ports
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_getValue_OutputPorts(); port++) {
            this->m_getValue_OutputPort[port].init();
#if FW_OBJECT_NAMES == 1
            char portName[120];
            snprintf(portName, sizeof(portName), "%s_getValue_OutputPort[%d]", this->m_objName.toChar(), port);
            this->m_getValue_OutputPort[port].setObjName(portName);
#endif
        }

        for (NATIVE_INT_TYPE port = 0; port < this->getNum_setValue_OutputPorts(); port++) {
            this->m_setValue_OutputPort[port].init();
#if FW_OBJECT_NAMES == 1
            char portName[120];
            snprintf(portName, sizeof(portName), "%s_setValue_OutputPort[%d]", this->m_objName.toChar(), port);
            this->m_setValue_OutputPort[port].setObjName(portName);
#endif
        }



	}

    // Up-calls, calls for output ports
    void PolyDbTesterComponentBase::getValue_out(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {
        FW_ASSERT(portNum < this->getNum_getValue_OutputPorts());
        this->m_getValue_OutputPort[portNum].invoke(entry, status, time, val);
    }

    void PolyDbTesterComponentBase::setValue_out(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {
        FW_ASSERT(portNum < this->getNum_setValue_OutputPorts());
        this->m_setValue_OutputPort[portNum].invoke(entry, status, time, val);
    }

    NATIVE_INT_TYPE PolyDbTesterComponentBase::getNum_getValue_OutputPorts() {
        return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_getValue_OutputPort));
    }
    NATIVE_INT_TYPE PolyDbTesterComponentBase::getNum_setValue_OutputPorts() {
        return static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_setValue_OutputPort));
    }
    bool PolyDbTesterComponentBase::isConnected_getValue_OutputPort(NATIVE_INT_TYPE portNum) {
         FW_ASSERT(portNum < this->getNum_getValue_OutputPorts(),portNum);
         return this->m_getValue_OutputPort[portNum].isConnected();
    }
    bool PolyDbTesterComponentBase::isConnected_setValue_OutputPort(NATIVE_INT_TYPE portNum) {
         FW_ASSERT(portNum < this->getNum_setValue_OutputPorts(),portNum);
         return this->m_setValue_OutputPort[portNum].isConnected();
    }


// private methods


}

