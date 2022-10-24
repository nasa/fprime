/*
 * TelemStoreComponentComponentTestAc.hpp
 *
 *  Created on: Monday, 13 April 2015
 *  Author:     tcanham
 *
 */
#ifndef TELEMSTORECOMPONENTCOMP_TESTER_HPP_
#define TELEMSTORECOMPONENTCOMP_TESTER_HPP_
#include <Fw/Cfg/FwConfig.hpp>
#include <Fw/Comp/FwPassiveComponentBase.hpp>

// type includes
#include <Fw/Tlm/FwTlmBuffer.hpp>
#include <Fw/Time/FwTime.hpp>
#include <Fw/Com/FwComBuffer.hpp>

// port includes
#include <Fw/Tlm/FwTlmPortAc.hpp>
#include <Fw/Com/FwComPortAc.hpp>
#include <Svc/Sched/SchedPortAc.hpp>

// serializable includes

namespace Svc {

    class TelemStoreComponentTesterBase : public Fw::PassiveComponentBase  {

    public:
        
        void set_tlmRecv_OutputPort(NATIVE_INT_TYPE portNum, Fw::InputTlmPort *port);
        void set_tlmGet_OutputPort(NATIVE_INT_TYPE portNum, Fw::InputTlmPort *port);
        void set_run_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputSchedPort *port);
        Fw::InputComPort* get_pktSend_InputPort(NATIVE_INT_TYPE portNum);
    protected:
        // Only called by derived class
        TelemStoreComponentTesterBase(const char* compName);

        virtual ~TelemStoreComponentTesterBase(void);
        virtual void init(NATIVE_INT_TYPE instance = 0);
        // downcalls for input ports
        virtual void pktSend_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data) = 0;
        // base class function (can be called by derived classes to bypass port)
        void pktSend_handlerBase(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data);
        // upcalls for output ports
        void tlmRecv_out(NATIVE_INT_TYPE portNum, U32 id, Fw::Time &timeTag, Fw::TlmBuffer &val);
        void tlmGet_out(NATIVE_INT_TYPE portNum, U32 id, Fw::Time &timeTag, Fw::TlmBuffer &val);
        void run_out(NATIVE_INT_TYPE portNum, I32 order);
        NATIVE_INT_TYPE getNum_tlmRecv_OutputPorts(void);
        NATIVE_INT_TYPE getNum_tlmGet_OutputPorts(void);
        NATIVE_INT_TYPE getNum_run_OutputPorts(void);
        NATIVE_INT_TYPE getNum_pktSend_InputPorts(void);

        // check to see if output port is connected

        bool isConnected_tlmRecv_OutputPort(NATIVE_INT_TYPE portNum);

        bool isConnected_tlmGet_OutputPort(NATIVE_INT_TYPE portNum);

        bool isConnected_run_OutputPort(NATIVE_INT_TYPE portNum);

             
    private:
        // output ports
        Fw::OutputTlmPort m_tlmRecv_OutputPort[1];
        Fw::OutputTlmPort m_tlmGet_OutputPort[1];
        Svc::OutputSchedPort m_run_OutputPort[1];

        // input ports
        Fw::InputComPort m_pktSend_InputPort[1];

        // calls for incoming ports
        static void m_p_pktSend_in(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::ComBuffer &data);
    };
};

#endif /* TELEMSTORECOMPONENTCOMP_TEST_HPP_ */


