#include <stdio.h>
#include <Fw/Cfg/FwConfig.hpp>
// The following header will need to be modified when test code is moved
// If the component tester is regenerated, this will need to be modified again.
// Make the compile fail to make sure it is changed
#include <Svc/TlmChan/test/ut/TelemStoreComponentTestAc.hpp>
#include <Fw/Types/FwAssert.hpp>

namespace Svc {
// public methods
    Fw::InputComPort *TelemStoreComponentTesterBase::get_pktSend_InputPort(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum < this->getNum_pktSend_InputPorts());
        return &this->m_pktSend_InputPort[portNum];
    }

    void TelemStoreComponentTesterBase::set_tlmRecv_OutputPort(NATIVE_INT_TYPE portNum, Fw::InputTlmPort* port) {
        FW_ASSERT(portNum < this->getNum_tlmRecv_OutputPorts());        
        this->m_tlmRecv_OutputPort[portNum].addCallPort(port);
    }

    void TelemStoreComponentTesterBase::set_tlmGet_OutputPort(NATIVE_INT_TYPE portNum, Fw::InputTlmPort* port) {
        FW_ASSERT(portNum < this->getNum_tlmGet_OutputPorts());        
        this->m_tlmGet_OutputPort[portNum].addCallPort(port);
    }

    void TelemStoreComponentTesterBase::set_run_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputSchedPort* port) {
        FW_ASSERT(portNum < this->getNum_run_OutputPorts());        
        this->m_run_OutputPort[portNum].addCallPort(port);
    }

// protected methods
    TelemStoreComponentTesterBase::TelemStoreComponentTesterBase(const char* compName) : Fw::PassiveComponentBase(compName) {
    }

    TelemStoreComponentTesterBase::~TelemStoreComponentTesterBase(void) {
    }

    void TelemStoreComponentTesterBase::init(NATIVE_INT_TYPE instance) {


	    // initialize base class
		Fw::PassiveComponentBase::init(instance);
        // Input ports attached to component here with external component interfaces
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_pktSend_InputPorts(); port++) {
            this->m_pktSend_InputPort[port].init();
            this->m_pktSend_InputPort[port].addCallComp(this, m_p_pktSend_in);
            this->m_pktSend_InputPort[port].setPortNum(port);
#if FW_OBJECT_NAMES == 1     
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_pktSend_InputPort[%d]", this->m_objName, port);
            this->m_pktSend_InputPort[port].setObjName(portName);
#endif
        }

        // Set output ports
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_tlmRecv_OutputPorts(); port++) {
            this->m_tlmRecv_OutputPort[port].init();
#if FW_OBJECT_NAMES == 1     
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_tlmRecv_OutputPort[%d]", this->m_objName, port);
            this->m_tlmRecv_OutputPort[port].setObjName(portName);
#endif      
        }
                
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_tlmGet_OutputPorts(); port++) {
            this->m_tlmGet_OutputPort[port].init();
#if FW_OBJECT_NAMES == 1     
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_tlmGet_OutputPort[%d]", this->m_objName, port);
            this->m_tlmGet_OutputPort[port].setObjName(portName);
#endif      
        }
                
        for (NATIVE_INT_TYPE port = 0; port < this->getNum_run_OutputPorts(); port++) {
            this->m_run_OutputPort[port].init();
#if FW_OBJECT_NAMES == 1     
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_run_OutputPort[%d]", this->m_objName, port);
            this->m_run_OutputPort[port].setObjName(portName);
#endif      
        }
                

	}

    // Up-calls, calls for output ports
    void TelemStoreComponentTesterBase::tlmRecv_out(NATIVE_INT_TYPE portNum, U32 id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
        FW_ASSERT(portNum < this->getNum_tlmRecv_OutputPorts());
        m_tlmRecv_OutputPort[portNum].invoke(id, timeTag, val);
    }

    void TelemStoreComponentTesterBase::tlmGet_out(NATIVE_INT_TYPE portNum, U32 id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
        FW_ASSERT(portNum < this->getNum_tlmGet_OutputPorts());
        m_tlmGet_OutputPort[portNum].invoke(id, timeTag, val);
    }

    void TelemStoreComponentTesterBase::run_out(NATIVE_INT_TYPE portNum, I32 order) {
        FW_ASSERT(portNum < this->getNum_run_OutputPorts());
        m_run_OutputPort[portNum].invoke(order);
    }

    NATIVE_INT_TYPE TelemStoreComponentTesterBase::getNum_tlmRecv_OutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_tlmRecv_OutputPort);
    }
    NATIVE_INT_TYPE TelemStoreComponentTesterBase::getNum_tlmGet_OutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_tlmGet_OutputPort);
    }
    NATIVE_INT_TYPE TelemStoreComponentTesterBase::getNum_run_OutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_run_OutputPort);
    }
    NATIVE_INT_TYPE TelemStoreComponentTesterBase::getNum_pktSend_InputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_pktSend_InputPort);
    }
    bool TelemStoreComponentTesterBase::isConnected_tlmRecv_OutputPort(NATIVE_INT_TYPE portNum) {
         FW_ASSERT(portNum < this->getNum_tlmRecv_OutputPorts(),portNum);
         return this->m_tlmRecv_OutputPort[portNum].isConnected();
    }
    bool TelemStoreComponentTesterBase::isConnected_tlmGet_OutputPort(NATIVE_INT_TYPE portNum) {
         FW_ASSERT(portNum < this->getNum_tlmGet_OutputPorts(),portNum);
         return this->m_tlmGet_OutputPort[portNum].isConnected();
    }
    bool TelemStoreComponentTesterBase::isConnected_run_OutputPort(NATIVE_INT_TYPE portNum) {
         FW_ASSERT(portNum < this->getNum_run_OutputPorts(),portNum);
         return this->m_run_OutputPort[portNum].isConnected();
    }


// private methods

    // call for incoming port pktSend
    void TelemStoreComponentTesterBase::m_p_pktSend_in(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::ComBuffer &data) {
          FW_ASSERT(callComp);
        TelemStoreComponentTesterBase* compPtr = (TelemStoreComponentTesterBase*)callComp;
        compPtr->pktSend_handlerBase(portNum, data);
    }

    void TelemStoreComponentTesterBase::pktSend_handlerBase(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data) {

        // make sure port number is valid
        FW_ASSERT(portNum < this->getNum_pktSend_InputPorts());

        // Down call to pure virtual handler method implemented in Impl class
        this->pktSend_handler(portNum, data);

    }

 
}

