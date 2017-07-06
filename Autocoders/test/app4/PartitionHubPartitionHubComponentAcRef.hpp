/*
 * PartitionHubComponentAc.hpp
 *
 *  Created on: Tuesday, 20 August 2013
 *  Author:     parallels
 *
 * Patterned after Tim's initial NGAT component pattern.
 *
 */
#ifndef PARTITIONHUBCOMP_HPP_
#define PARTITIONHUBCOMP_HPP_

#include <Fw/Port/FwInputSerializePort.hpp>
#include <Fw/Port/FwOutputSerializePort.hpp>
#include <Fw/Comp/FwActiveComponentBase.hpp>
#include <Fw/Types/FwStringType.hpp>
#include <Fw/Types/FwBasicTypes.hpp>


namespace PartitionHub {

    class PartitionHubBase : public Fw::ComponentBase  {
    public:
        Fw::InputSerializePort* getPortASerializeInputPort(NATIVE_INT_TYPE portNum);

        void setPortBSerializeOutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort *port);

        // Overload the setPortBSerializeOutputPort(...) for serialized connections
        void setPortBSerializeOutputPort(NATIVE_INT_TYPE portNum, Fw::InputPortBase *port);

    protected:
        // Only called by derived class
#if FW_OBJECT_NAMES == 1
        PartitionHubBase(const char* compName);
#else
        PartitionHubBase();
#endif

        virtual ~PartitionHubBase(void);
        virtual void init(NATIVE_INT_TYPE queueDepth);

        // downcalls for input ports
        virtual void PortA_Serialize_handler(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer) = 0;
        // base class function (can be called by derived classes to bypass port)
        void PortA_Serialize_handler_base(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer);
        // upcalls for output ports
        void PortB_Serialize_out(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer);
        NATIVE_INT_TYPE m_getNumPortASerializeInputPorts(void);
        NATIVE_INT_TYPE m_getNumPortBSerializeOutputPorts(void);

        // check to see if output port is connected
        bool m_isConnectedPortBSerializeOutputPorts(NATIVE_INT_TYPE portNum);
    private:
        // input ports
        Fw::InputSerializePort m_PortASerializeInputPort[2];

        // output ports
        Fw::OutputSerializePort m_PortBSerializeOutputPort[2];

        // calls for incoming ports
        void invokeSerial(I32 portNum, Fw::SerializeBufferBase &Buffer);

    };
};

#endif /* PARTITIONHUBCOMP_HPP_ */


