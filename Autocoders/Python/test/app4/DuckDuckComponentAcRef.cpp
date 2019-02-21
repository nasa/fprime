#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <Os/Queue.hpp>

#include <Duck/DuckDuckComponentAc.hpp>

#include <Fw/Cfg/FwConfig.hpp>
#include <Fw/Types/FwAssert.hpp>

#include <Fw/Types/FwStringType.hpp>
#include <Fw/Types/FwEightyCharString.hpp>
#include <Ports/Msg2PortAc.hpp>
#include <Ports/Msg1PortAc.hpp>

namespace Duck {

    namespace { // Do an anonymous namespace to prevent namespace collisions
        typedef enum {
            DUCK_COMPONENT_EXIT = Fw::ActiveComponentBase::ACTIVE_COMPONENT_EXIT,
            INPUTPORT2_MSG2,
            INPUTPORT3_MSG1
        } MsgTypeEnum;

        // Here's a trick to get the max size. Do a union of the input port serialization sizes.

        typedef union {
            char port1[Ports::InputMsg2Port::SERIALIZED_SIZE];
            char port2[Ports::InputMsg1Port::SERIALIZED_SIZE];
        } BuffUnion;

        // This can be the same generated code. Only dependent only union, which can be the same name in anonymous namespace
        class ComponentIpcSerializableBuffer : public Fw::SerializeBufferBase {
            public:

                typedef enum {
                    // Compute total max. size of message here...
                    SERIALIZATION_SIZE = sizeof(BuffUnion) + sizeof(NATIVE_INT_TYPE) + sizeof(I32)
                };

                I32 getBuffCapacity(void) const {
                    return sizeof(m_buff);
                }

                U8* getBuffAddr(void) {
                    return m_buff;
                }

                const U8* getBuffAddr(void) const {
                    return m_buff;
                }

            private:
                // Should be the max of all the input ports' serialized sizes...
                U8 m_buff[SERIALIZATION_SIZE];
        };

    }


    Ports::InputMsg2Port *DuckBase::getinputPort1Msg2InputPort(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum < this->m_getNuminputPort1Msg2InputPorts());
        return &this->m_inputPort1Msg2InputPort[portNum];
    }

    Ports::InputMsg2Port *DuckBase::getinputPort2Msg2InputPort(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum < this->m_getNuminputPort2Msg2InputPorts());
        return &this->m_inputPort2Msg2InputPort[portNum];
    }

    Ports::InputMsg1Port *DuckBase::getinputPort3Msg1InputPort(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum < this->m_getNuminputPort3Msg1InputPorts());
        return &this->m_inputPort3Msg1InputPort[portNum];
    }


    void DuckBase::setoutputPort1Msg2OutputPort(NATIVE_INT_TYPE portNum, Ports::InputMsg2Port* port) {
        FW_ASSERT(portNum < this->m_getNumoutputPort1Msg2OutputPorts());
        return this->m_outputPort1Msg2OutputPort[portNum].addCallPort(port);
    }

    void DuckBase::setoutputPort2Msg1OutputPort(NATIVE_INT_TYPE portNum, Ports::InputMsg1Port* port) {
        FW_ASSERT(portNum < this->m_getNumoutputPort2Msg1OutputPorts());
        return this->m_outputPort2Msg1OutputPort[portNum].addCallPort(port);
    }

    void DuckBase::setoutputPort1Msg2OutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort *port) {
        FW_ASSERT(portNum < this->m_getNumoutputPort1Msg2OutputPorts());
        return this->m_outputPort1Msg2OutputPort[portNum].registerSerialPort(port);
    }

    void DuckBase::setoutputPort2Msg1OutputPort(NATIVE_INT_TYPE portNum, Fw::InputSerializePort *port) {
        FW_ASSERT(portNum < this->m_getNumoutputPort2Msg1OutputPorts());
        return this->m_outputPort2Msg1OutputPort[portNum].registerSerialPort(port);
    }

#if FW_OBJECT_NAMES == 1
    DuckBase::DuckBase(const char* compName) : Fw::ActiveComponentBase(compName) {
    }
#else
    DuckBase::DuckBase() : Fw::ActiveComponentBase() {
    }
#endif

    void DuckBase::init(NATIVE_INT_TYPE queueDepth) {
        // Input ports attached to component here with external component interfaces
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNuminputPort1Msg2InputPorts(); port++) {
            this->m_inputPort1Msg2InputPort[port].init();
            this->m_inputPort1Msg2InputPort[port].addCallComp(this, m_p_inputPort1_Msg2_in);
            this->m_inputPort1Msg2InputPort[port].setPortNum(port);
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_inputPort1Msg2InputPort_%d", this->m_objName, port);
            this->m_inputPort1Msg2InputPort[port].setObjName(portName);
#endif
        }
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNuminputPort2Msg2InputPorts(); port++) {
            this->m_inputPort2Msg2InputPort[port].init();
            this->m_inputPort2Msg2InputPort[port].addCallComp(this, m_p_inputPort2_Msg2_in);
            this->m_inputPort2Msg2InputPort[port].setPortNum(port);
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_inputPort2Msg2InputPort_%d", this->m_objName, port);
            this->m_inputPort2Msg2InputPort[port].setObjName(portName);
#endif
        }
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNuminputPort3Msg1InputPorts(); port++) {
            this->m_inputPort3Msg1InputPort[port].init();
            this->m_inputPort3Msg1InputPort[port].addCallComp(this, m_p_inputPort3_Msg1_in);
            this->m_inputPort3Msg1InputPort[port].setPortNum(port);
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_inputPort3Msg1InputPort_%d", this->m_objName, port);
            this->m_inputPort3Msg1InputPort[port].setObjName(portName);
#endif
        }

        // Set output ports
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNumoutputPort1Msg2OutputPorts(); port++) {
            this->m_outputPort1Msg2OutputPort[port].init();
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_outputPort1Msg2OutputPort_%d", this->m_objName, port);
            this->m_outputPort1Msg2OutputPort[port].setObjName(portName);
#endif
        }
        for (NATIVE_INT_TYPE port = 0; port < this->m_getNumoutputPort2Msg1OutputPorts(); port++) {
            this->m_outputPort2Msg1OutputPort[port].init();
#if FW_OBJECT_NAMES == 1
            char portName[80];
            snprintf(portName, sizeof(portName), "%s_outputPort2Msg1OutputPort_%d", this->m_objName, port);
            this->m_outputPort2Msg1OutputPort[port].setObjName(portName);
#endif
        }
		this->createQueue(queueDepth, ComponentIpcSerializableBuffer::SERIALIZATION_SIZE);
	}


	DuckBase::~DuckBase(void) {
	}

    // Up-calls, calls for output ports
    void DuckBase::outputPort1_Msg2_out(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) {
        FW_ASSERT(portNum < this->m_getNumoutputPort1Msg2OutputPorts());
        m_outputPort1Msg2OutputPort[portNum].invoke(str, cmd);
    }

    void DuckBase::outputPort2_Msg1_out(NATIVE_INT_TYPE portNum, U32 cmd) {
        FW_ASSERT(portNum < this->m_getNumoutputPort2Msg1OutputPorts());
        m_outputPort2Msg1OutputPort[portNum].invoke(cmd);
    }

    NATIVE_INT_TYPE DuckBase::m_getNumoutputPort1Msg2OutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_outputPort1Msg2OutputPort);
    }

    NATIVE_INT_TYPE DuckBase::m_getNumoutputPort2Msg1OutputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_outputPort2Msg1OutputPort);
    }

    NATIVE_INT_TYPE DuckBase::m_getNuminputPort1Msg2InputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_inputPort1Msg2InputPort);
    }

    NATIVE_INT_TYPE DuckBase::m_getNuminputPort2Msg2InputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_inputPort2Msg2InputPort);
    }

    NATIVE_INT_TYPE DuckBase::m_getNuminputPort3Msg1InputPorts(void) {
        return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_inputPort3Msg1InputPort);
    }


    // call for incoming port inputPort1
    void DuckBase::m_p_inputPort1_Msg2_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) {
        DuckBase* compPtr = (DuckBase*)callComp;
        compPtr->inputPort1_Msg2_handler_base(portNum, str, cmd);
    }

    void DuckBase::inputPort1_Msg2_handler_base(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) {
        // Down call to pure virtual handler method implemented in Impl class
        this->inputPort1_Msg2_handler(portNum, str, cmd);
    }

    // call for incoming port inputPort2
    void DuckBase::m_p_inputPort2_Msg2_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) {
        DuckBase* compPtr = (DuckBase*)callComp;
        compPtr->inputPort2_Msg2_handler_base(portNum, str, cmd);
    }

    void DuckBase::inputPort2_Msg2_handler_base(NATIVE_INT_TYPE portNum, Fw::EightyCharString str, U32 cmd) {
        ComponentIpcSerializableBuffer msg;
        Fw::SerializeStatus status;

        status = msg.serialize(str);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);

        status = msg.serialize(cmd);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);

        status = msg.serialize(portNum);
        FW_ASSERT (status == Fw::FW_SERIALIZE_OK);

        status = msg.serialize((I32)INPUTPORT2_MSG2);
        FW_ASSERT (status == Fw::FW_SERIALIZE_OK);

        Os::Queue::QueueStatus qStatus = this->m_queue.send(msg,0);

        FW_ASSERT(qStatus == Os::Queue::QUEUE_OK);
    }

    // call for incoming port inputPort3
    void DuckBase::m_p_inputPort3_Msg1_in(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, U32 cmd) {
        DuckBase* compPtr = (DuckBase*)callComp;
        compPtr->inputPort3_Msg1_handler_base(portNum, cmd);
    }

    void DuckBase::inputPort3_Msg1_handler_base(NATIVE_INT_TYPE portNum, U32 cmd) {
        ComponentIpcSerializableBuffer msg;
        Fw::SerializeStatus status;

        status = msg.serialize(cmd);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);

        status = msg.serialize(portNum);
        FW_ASSERT (status == Fw::FW_SERIALIZE_OK);

        status = msg.serialize((I32)INPUTPORT3_MSG1);
        FW_ASSERT (status == Fw::FW_SERIALIZE_OK);

        Os::Queue::QueueStatus qStatus = this->m_queue.send(msg,0);

        FW_ASSERT(qStatus == Os::Queue::QUEUE_OK);
    }

    void DuckBase::preamble(void) {
        // default - empty
    }

    void DuckBase::finalizer(void) {
        // default - empty
    }

    // task entry point for active task
    Fw::QueuedComponentBase::MsgDispatchStatus DuckBase::doDispatch(void) {
        ComponentIpcSerializableBuffer msg;
        I32 priority;
        Os::Queue::QueueStatus msgStatus = this->m_queue.receive(msg,priority);
        FW_ASSERT(msgStatus == Os::Queue::QUEUE_OK);

        I32 desMsg;
        Fw::SerializeStatus deserStatus = msg.deserialize(desMsg);
        FW_ASSERT(deserStatus == Fw::FW_SERIALIZE_OK);

        MsgTypeEnum msgType = static_cast<MsgTypeEnum>(desMsg);

        NATIVE_INT_TYPE portNum;
        deserStatus = msg.deserialize(portNum);
        FW_ASSERT(deserStatus == Fw::FW_SERIALIZE_OK);

        switch (msgType) {
            case DUCK_COMPONENT_EXIT: // exit task
                return MSG_DISPATCH_EXIT;
            case INPUTPORT2_MSG2: {
                // call derived handler function for port inputPort2
                U32 cmd;
                deserStatus = msg.deserialize(cmd);
                FW_ASSERT(deserStatus == Fw::FW_SERIALIZE_OK);

                Fw::EightyCharString str;
                deserStatus = msg.deserialize(str);
                FW_ASSERT(deserStatus == Fw::FW_SERIALIZE_OK);

                this->inputPort2_Msg2_handler(portNum, str, cmd);
                break;
            }
            case INPUTPORT3_MSG1: {
                // call derived handler function for port inputPort3
                U32 cmd;
                deserStatus = msg.deserialize(cmd);
                FW_ASSERT(deserStatus == Fw::FW_SERIALIZE_OK);

                this->inputPort3_Msg1_handler(portNum, cmd);
                break;
            }
            default:
                return MSG_DISPATCH_ERROR;
        }
        return MSG_DISPATCH_OK;
    }
}

