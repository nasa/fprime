// ======================================================================
// \title  Framework.hpp
// \author tumbar
// \brief  hpp file for Framework component implementation class
// ======================================================================

#ifndef FppTest_Framework_HPP
#define FppTest_Framework_HPP

#include <Fw/DataStructures/FifoQueue.hpp>
#include "FppTest/topology/components/Framework/FrameworkComponentAc.hpp"

#include "FppTest/topology/types/CmdRegSerializableAc.hpp"
#include "FppTest/topology/types/CmdResponseSerializableAc.hpp"
#include "FppTest/topology/types/DpGetSerializableAc.hpp"
#include "FppTest/topology/types/DpRequestSerializableAc.hpp"
#include "FppTest/topology/types/DpResponseSerializableAc.hpp"
#include "FppTest/topology/types/DpSendSerializableAc.hpp"
#include "FppTest/topology/types/LogSerializableAc.hpp"
#include "FppTest/topology/types/LogTextSerializableAc.hpp"
#include "FppTest/topology/types/PingSerializableAc.hpp"
#include "FppTest/topology/types/PrmGetSerializableAc.hpp"
#include "FppTest/topology/types/PrmSetSerializableAc.hpp"
#include "FppTest/topology/types/TlmSerializableAc.hpp"

#include "Fw/DataStructures/ArrayMap.hpp"

namespace FppTest {

class Framework final : public FrameworkComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Framework object
    Framework(const char* const compName  //!< The component name
    );

    //! Destroy Framework object
    ~Framework();

    void init(FwSizeType queueDepth,        //!< The queue depth
              FwEnumStoreType instance = 0  //!< The instance number
    );

    void setTime(const Fw::Time& time);
    Fw::CmdResponse sendCommand(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args);
    void ping(U32 key);
    void sync();
    void syncFor(FwIndexType portNum);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port syncIn
    void syncIn_handler(FwIndexType portNum,  //!< The port number
                        U32 context           //!< The call order
                        ) override;

    //! Handler implementation for LogRecv
    //!
    //! Event input port
    void LogRecv_handler(FwIndexType portNum,              //!< The port number
                         FwEventIdType id,                 //!< Log ID
                         Fw::Time& timeTag,                //!< Time Tag
                         const Fw::LogSeverity& severity,  //!< The severity argument
                         Fw::LogBuffer& args               //!< Buffer containing serialized log entry
                         ) override;

    //! Handler implementation for ParamGetIn
    //!
    //! Port that replies with parameter value
    Fw::ParamValid ParamGetIn_handler(FwIndexType portNum,  //!< The port number
                                      FwPrmIdType id,       //!< Parameter ID
                                      Fw::ParamBuffer& val  //!< Buffer containing serialized parameter value.
                                                            //!< Unmodified if param not found.
                                      ) override;

    //! Handler implementation for ParamSetIn
    //!
    //! Port that sets a parameter
    void ParamSetIn_handler(FwIndexType portNum,  //!< The port number
                            FwPrmIdType id,       //!< Parameter ID
                            Fw::ParamBuffer& val  //!< Buffer containing serialized parameter value
                            ) override;

    //! Handler implementation for PingReturn
    //!
    //! Ping return port
    void PingReturn_handler(FwIndexType portNum,  //!< The port number
                            U32 key               //!< Value to return to pinger
                            ) override;

    //! Handler implementation for TextLogRecv
    //!
    //! Text Event input port
    void TextLogRecv_handler(FwIndexType portNum,              //!< The port number
                             FwEventIdType id,                 //!< Log ID
                             Fw::Time& timeTag,                //!< Time Tag
                             const Fw::LogSeverity& severity,  //!< The severity argument
                             Fw::TextLogString& text           //!< Text of log message
                             ) override;

    //! Handler implementation for TlmRecv
    //!
    //! Guarded port for receiving telemetry values
    void TlmRecv_handler(FwIndexType portNum,  //!< The port number
                         FwChanIdType id,      //!< Telemetry Channel ID
                         Fw::Time& timeTag,    //!< Time Tag
                         Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value
                         ) override;

    //! Handler implementation for compCmdReg
    //!
    //! Command Registration Port. max_number should match dispatch port.
    void compCmdReg_handler(FwIndexType portNum,  //!< The port number
                            FwOpcodeType opCode   //!< Command Op Code
                            ) override;

    //! Handler implementation for compCmdStat
    //!
    //! Input Command Status Port
    void compCmdStat_handler(FwIndexType portNum,             //!< The port number
                             FwOpcodeType opCode,             //!< Command Op Code
                             U32 cmdSeq,                      //!< Command Sequence
                             const Fw::CmdResponse& response  //!< The command response argument
                             ) override;

    //! Handler implementation for productGetIn
    //!
    //! Ports for responding to a data product get from a client component
    Fw::Success productGetIn_handler(FwIndexType portNum,  //!< The port number
                                     FwDpIdType id,        //!< The container ID (input)
                                     FwSizeType dataSize,  //!< The data size of the requested buffer (input)
                                     Fw::Buffer& buffer    //!< The buffer (output)
                                     ) override;

    //! Handler implementation for productRequestIn
    //!
    //! Ports for receiving data product buffer requests from a client component
    void productRequestIn_handler(FwIndexType portNum,  //!< The port number
                                  FwDpIdType id,        //!< The container ID
                                  FwSizeType dataSize   //!< The data size of the requested buffer
                                  ) override;

    //! Handler implementation for timeGetIn
    //!
    //! Port for getting current time
    void timeGetIn_handler(FwIndexType portNum,  //!< The port number
                           Fw::Time& time        //!< Reference to Time object
                           ) override;

    //! Handler for input port productSendIn
    void productSendIn_handler(FwIndexType portNum,      //!< The port number
                               FwDpIdType id,            //!< The container ID
                               const Fw::Buffer& buffer  //!< The buffer
                               ) override;

  private:
    Fw::ArrayMap<FwOpcodeType, FwIndexType, 10> m_opcode_registrations;
    Os::Queue m_completion_queue;
    Fw::Time m_time;
    Fw::Array<U8[1024], 5> m_memory_pool;
    Fw::FifoQueue<Fw::Buffer, 5> m_memory_queue;

  public:
    constexpr static U32 QueueLength = 10;
    Fw::FifoQueue<FrameworkPortData::CmdReg, QueueLength> cmd_reg_queue;
    Fw::FifoQueue<FrameworkPortData::CmdResponse, QueueLength> cmd_response_queue;
    Fw::FifoQueue<FrameworkPortData::Log, QueueLength> log_queue;
    Fw::FifoQueue<FrameworkPortData::LogText, QueueLength> log_text_queue;
    Fw::FifoQueue<FrameworkPortData::Tlm, QueueLength> tlm_queue;
    Fw::FifoQueue<FrameworkPortData::PrmGet, QueueLength> prm_get_queue;
    Fw::FifoQueue<FrameworkPortData::PrmSet, QueueLength> prm_set_queue;
    Fw::FifoQueue<FrameworkPortData::DpGet, QueueLength> dp_get_queue;
    Fw::FifoQueue<FrameworkPortData::DpRequest, QueueLength> dp_request_queue;
    Fw::FifoQueue<FrameworkPortData::DpSend, QueueLength> dp_send_queue;
    Fw::FifoQueue<FrameworkPortData::Ping, QueueLength> ping_queue;
};

}  // namespace FppTest

#endif
