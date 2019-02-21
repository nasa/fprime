/*
 * DuckComponentAc.hpp
 *
 *  Created on: Thursday, 15 August 2013
 *  Author:     parallels
 *
 * Patterned after Tim's initial NGAT component pattern.
 *
 */
#ifndef DUCKCOMP_HPP_
#define DUCKCOMP_HPP_

#include <Fw/Comp/FwActiveComponentBase.hpp>
#include <Fw/Types/FwStringType.hpp>
#include <Fw/Types/FwBasicTypes.hpp>
#include <Fw/Port/FwInputSerializePort.hpp>

#include <Ports/Msg2PortAc.hpp>
#include <Ports/Msg1PortAc.hpp>

namespace Duck {

    class DuckBase : public Fw::ActiveComponentBase  {
    public:
        Ports::InputMsg2Port* getinputPort1Msg2InputPort(NATIVE_INT_TYPE portNum);
        Ports::InputMsg2Port* getinputPort2Msg2InputPort(NATIVE_INT_TYPE portNum);
        Ports::InputMsg1Port* getinputPort3Msg1InputPort(NATIVE_INT_TYPE portNum);

        void setoutputPort1Msg2OutputPort(NATIVE_INT_TYPE portNum, Ports::InputMsg2Port *port);
        void setoutputPort2Msg1OutputPort(NATIVE_INT_TYPE portNum, Ports::InputMsg1Port *port);

        // Overload the setoutputPort's for serialized connections
        void setoutputPort1Msg2OutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort *port);
        void setoutputPort2Msg1OutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort *port);
    protected:
        // Only called by derived class
#if FW_OBJECT_NAMES == 1
        DuckBase(const char* compName);
#else
        DuckBase();
#endif

        virtual ~DuckBase(void);
        virtual void init(NATIVE_INT_TYPE queueDepth);

        // downcalls for input ports
        virtual void inputPort1_Msg2_handler(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) = 0;
        // base class function (can be called by derived classes to bypass port)
        void inputPort1_Msg2_handler_base(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd);

        virtual void inputPort2_Msg2_handler(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) = 0;
        // base class function (can be called by derived classes to bypass port)
        void inputPort2_Msg2_handler_base(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd);

        virtual void inputPort3_Msg1_handler(NATIVE_INT_TYPE portNum, U32 cmd) = 0;
        // base class function (can be called by derived classes to bypass port)
        void inputPort3_Msg1_handler_base(NATIVE_INT_TYPE portNum, U32 cmd);

        // upcalls for output ports
        void outputPort1_Msg2_out(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd);
        void outputPort2_Msg1_out(NATIVE_INT_TYPE portNum, U32 cmd);
        NATIVE_INT_TYPE m_getNumoutputPort1Msg2OutputPorts(void);
        NATIVE_INT_TYPE m_getNumoutputPort2Msg1OutputPorts(void);
        NATIVE_INT_TYPE m_getNuminputPort1Msg2InputPorts(void);
        NATIVE_INT_TYPE m_getNuminputPort2Msg2InputPorts(void);
        NATIVE_INT_TYPE m_getNuminputPort3Msg1InputPorts(void);

        // check to see if output port is connected
        bool m_isConnectedoutputPort1Msg2OutputPorts(NATIVE_INT_TYPE portNum);
        bool m_isConnectedoutputPort2Msg1OutputPorts(NATIVE_INT_TYPE portNum);
    private:
        // input ports
        Ports::InputMsg2Port m_inputPort1Msg2InputPort[1];
        Ports::InputMsg2Port m_inputPort2Msg2InputPort[1];
        Ports::InputMsg1Port m_inputPort3Msg1InputPort[1];

        // output ports
        Ports::OutputMsg2Port m_outputPort1Msg2OutputPort[1];
        Ports::OutputMsg1Port m_outputPort2Msg1OutputPort[1];

        // calls for incoming ports
        static void m_p_inputPort1_Msg2_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd);
        static void m_p_inputPort2_Msg2_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd);
        static void m_p_inputPort3_Msg1_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, U32 cmd);
        virtual void preamble(void); //!< A function that will be called before the event loop is entered
        virtual MsgDispatchStatus doDispatch(void); //!< Function that will be run in new task context
        virtual void finalizer(void); //!< Function run after task loop exits

    };
};

#endif /* DUCKCOMP_HPP_ */


