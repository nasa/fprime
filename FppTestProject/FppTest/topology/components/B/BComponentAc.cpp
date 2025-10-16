// ======================================================================
// \title  BComponentAc.cpp
// \author bocchino
// \brief  cpp file for B component base class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ExternalString.hpp"
#if FW_ENABLE_TEXT_LOGGING
#include "Fw/Types/String.hpp"
#endif
#include "FppTest/topology/components/B/BComponentAc.hpp"

namespace FppTest {

namespace {
enum MsgTypeEnum {
    B_COMPONENT_EXIT = Fw::ActiveComponentBase::ACTIVE_COMPONENT_EXIT,
    DATAIN_DATA,
};

// Get the max size by constructing a union of the async input, command, and
// internal port serialization sizes
union BuffUnion {
    BYTE dataInPortSize[FppTest::DataPortConstants::INPUT_SERIALIZED_SIZE];
};

// Define a message buffer class large enough to handle all the
// asynchronous inputs to the component
class ComponentIpcSerializableBuffer : public Fw::SerializeBufferBase {
  public:
    enum {
        // Offset into data in buffer: Size of message ID and port number
        DATA_OFFSET = sizeof(FwEnumStoreType) + sizeof(FwIndexType),
        // Max data size
        MAX_DATA_SIZE = sizeof(BuffUnion),
        // Max message size: Size of message id + size of port + max data size
        SERIALIZATION_SIZE = DATA_OFFSET + MAX_DATA_SIZE
    };

    Fw::Serializable::SizeType getBuffCapacity() const { return sizeof(m_buff); }

    U8* getBuffAddr() { return m_buff; }

    const U8* getBuffAddr() const { return m_buff; }

  private:
    // Should be the max of all the input ports serialized sizes...
    U8 m_buff[SERIALIZATION_SIZE];
};
}  // namespace

// ----------------------------------------------------------------------
// Component initialization
// ----------------------------------------------------------------------

void BComponentBase::init(FwSizeType queueDepth, FwEnumStoreType instance) {
    // Initialize base class
    Fw::ActiveComponentBase::init(instance);

#if !FW_DIRECT_PORT_CALLS
    // Connect input port dataIn
    for (
      FwIndexType port = 0;
      port < NUM_DATAIN_INPUT_PORTS;
      port++
    ) {
      this->m_dataIn_InputPort[port].init();
      this->m_dataIn_InputPort[port].addCallComp(
        this,
        m_p_dataIn_in
      );
      this->m_dataIn_InputPort[port].setPortNum(port);

#if FW_OBJECT_NAMES == 1
      Fw::ObjectName portName;
      portName.format(
        "%s_dataIn_InputPort[%" PRI_FwIndexType "]",
        this->m_objName.toChar(),
        port
      );
      this->m_dataIn_InputPort[port].setObjName(portName.toChar());
#endif
    }

    // Connect output port dataOut
    for (
      FwIndexType port = 0;
      port < NUM_DATAOUT_OUTPUT_PORTS;
      port++
    ) {
      this->m_dataOut_OutputPort[port].init();

#if FW_OBJECT_NAMES == 1
      Fw::ObjectName portName;
      portName.format(
        "%s_dataOut_OutputPort[%" PRI_FwIndexType "]",
        this->m_objName.toChar(),
        port
      );
      this->m_dataOut_OutputPort[port].setObjName(portName.toChar());
#endif
    }
#endif

    // Create the queue
    Os::Queue::Status qStat =
        this->createQueue(queueDepth, static_cast<FwSizeType>(ComponentIpcSerializableBuffer::SERIALIZATION_SIZE));
    FW_ASSERT(Os::Queue::Status::OP_OK == qStat, static_cast<FwAssertArgType>(qStat));
}

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BComponentBase::BComponentBase(const char* compName) : Fw::ActiveComponentBase(compName) {}

BComponentBase::~BComponentBase() {}

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Getters for typed input ports
// ----------------------------------------------------------------------

FppTest::InputDataPort* BComponentBase::get_dataIn_InputPort(FwIndexType portNum) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAIN_INPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    return &this->m_dataIn_InputPort[portNum];
}

#endif

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Connect typed input ports to typed output ports
// ----------------------------------------------------------------------

void BComponentBase::set_dataOut_OutputPort(FwIndexType portNum, FppTest::InputDataPort* port) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    this->m_dataOut_OutputPort[portNum].addCallPort(port);
}

#endif

#if !FW_DIRECT_PORT_CALLS && FW_PORT_SERIALIZATION

// ----------------------------------------------------------------------
// Connect serial input ports to typed output ports
// ----------------------------------------------------------------------

void BComponentBase::set_dataOut_OutputPort(FwIndexType portNum, Fw::InputSerializePort* port) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    this->m_dataOut_OutputPort[portNum].registerSerialPort(port);
}

#endif

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Connection status queries for typed output ports
// ----------------------------------------------------------------------

bool BComponentBase ::isConnected_dataOut_OutputPort(FwIndexType portNum) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    return this->m_dataOut_OutputPort[portNum].isConnected();
}

#endif

// ----------------------------------------------------------------------
// Port handler base-class functions for typed input ports
//
// Call these functions directly to bypass the corresponding ports
// ----------------------------------------------------------------------

void BComponentBase::dataIn_handlerBase(FwIndexType portNum, U32 data) {
    // Make sure port number is valid
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAIN_INPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    // Call pre-message hook
    dataIn_preMsgHook(portNum, data);
    ComponentIpcSerializableBuffer msg;
    Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;

    // Serialize message ID
    _status = msg.serializeFrom(static_cast<FwEnumStoreType>(DATAIN_DATA));
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_status));

    // Serialize port number
    _status = msg.serializeFrom(portNum);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_status));

    // Serialize argument data
    _status = msg.serializeFrom(data);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_status));

    // Send message
    Os::Queue::BlockingType _block = Os::Queue::NONBLOCKING;
    Os::Queue::Status qStatus = this->m_queue.send(msg, 0, _block);

    FW_ASSERT(qStatus == Os::Queue::OP_OK, static_cast<FwAssertArgType>(qStatus));
}

// ----------------------------------------------------------------------
// Pre-message hooks for typed async input ports
//
// Each of these functions is invoked just before processing a message
// on the corresponding port. By default, they do nothing. You can
// override them to provide specific pre-message behavior.
// ----------------------------------------------------------------------

void BComponentBase::dataIn_preMsgHook(FwIndexType portNum, U32 data) {
    // Default: no-op
}

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Invocation functions for typed output ports
// ----------------------------------------------------------------------

void BComponentBase ::dataOut_out(FwIndexType portNum, U32 data) {
    FW_ASSERT((0 <= portNum) && (portNum < NUM_DATAOUT_OUTPUT_PORTS), static_cast<FwAssertArgType>(portNum));

    FW_ASSERT(this->m_dataOut_OutputPort[portNum].isConnected(), static_cast<FwAssertArgType>(portNum));
    this->m_dataOut_OutputPort[portNum].invoke(data);
}

#endif

// ----------------------------------------------------------------------
// Message dispatch functions
// ----------------------------------------------------------------------

Fw::QueuedComponentBase::MsgDispatchStatus BComponentBase::doDispatch() {
    ComponentIpcSerializableBuffer _msg;
    FwQueuePriorityType _priority = 0;

    Os::Queue::Status _msgStatus = this->m_queue.receive(_msg, Os::Queue::BLOCKING, _priority);
    FW_ASSERT(_msgStatus == Os::Queue::OP_OK, static_cast<FwAssertArgType>(_msgStatus));

    // Reset to beginning of buffer
    _msg.resetDeser();

    FwEnumStoreType _desMsg = 0;
    Fw::SerializeStatus _deserStatus = _msg.deserializeTo(_desMsg);
    FW_ASSERT(_deserStatus == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_deserStatus));

    MsgTypeEnum _msgType = static_cast<MsgTypeEnum>(_desMsg);

    if (_msgType == B_COMPONENT_EXIT) {
        return MSG_DISPATCH_EXIT;
    }

    FwIndexType portNum = 0;
    _deserStatus = _msg.deserializeTo(portNum);
    FW_ASSERT(_deserStatus == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_deserStatus));

    switch (_msgType) {
        // Handle async input port dataIn
        case DATAIN_DATA: {
            // Deserialize argument data
            U32 data;
            _deserStatus = _msg.deserializeTo(data);
            FW_ASSERT(_deserStatus == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(_deserStatus));
            // Call handler function
            this->dataIn_handler(portNum, data);

            break;
        }

        default:
            return MSG_DISPATCH_ERROR;
    }

    return MSG_DISPATCH_OK;
}

#if !FW_DIRECT_PORT_CALLS

// ----------------------------------------------------------------------
// Calls for messages received on typed input ports
// ----------------------------------------------------------------------

void BComponentBase ::m_p_dataIn_in(Fw::PassiveComponentBase* callComp, FwIndexType portNum, U32 data) {
    FW_ASSERT(callComp);
    BComponentBase* compPtr = static_cast<BComponentBase*>(callComp);
    compPtr->dataIn_handlerBase(portNum, data);
}

#endif

}  // namespace FppTest
