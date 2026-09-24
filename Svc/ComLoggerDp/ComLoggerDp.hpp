// ======================================================================
// \title  ComLoggerDp.hpp
// \author tcanham
// \brief  hpp file for ComLoggerDp component implementation class
// ======================================================================

#ifndef Svc_ComLoggerDp_HPP
#define Svc_ComLoggerDp_HPP

#include "Fw/Dp/DpContainer.hpp"
#include "Svc/ComLoggerDp/ComLoggerDpComponentAc.hpp"
#include "config/ComLoggerDpCfg.hpp"

namespace Svc {

class ComLoggerDp final : public ComLoggerDpComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Public interface
    // ----------------------------------------------------------------------

    //! Serialized size of one ComBufferRecord holding a sentry plus a maximum-size ComBuffer
    static constexpr FwSizeType RECORD_SIZE =
        SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE + sizeof(ComLoggerDpSentry));

    //! Calculate total buffer size needed for a data product container
    //! This is the TOTAL size including:
    //! - DpContainer header (packet descriptor, ID, priority, time tag, etc.)
    //! - Header hash
    //! - Record data (N packets × (sentry + ComBuffer data))
    //! - Data hash
    //!
    //! Use this function to compute the size of buffers to allocate from
    //! Svc::BufferManager or equivalent for holding complete data product containers.
    //!
    //! NOTE: This is NOT the size to pass to dpGet() - DpManager adds the header
    //! overhead internally. This is for external buffer allocation only.
    //!
    //! \param packetsPerContainer: Number of packets that will fit in the container
    //! \return Total buffer size in bytes needed for the complete container
    static constexpr FwSizeType ComLoggerDpBuffSize(U32 packetsPerContainer) {
        return DpContainer::MIN_PACKET_SIZE + packetsPerContainer * RECORD_SIZE;
    }

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ComLoggerDp object
    explicit ComLoggerDp(const char* const compName  //!< The component name
    );

    //! Destroy ComLoggerDp object
    ~ComLoggerDp();

    //! Copy constructor (deleted)
    ComLoggerDp(const ComLoggerDp&) = delete;

    //! Copy assignment operator (deleted)
    ComLoggerDp& operator=(const ComLoggerDp&) = delete;

    //! Move constructor (deleted)
    ComLoggerDp(ComLoggerDp&&) = delete;

    //! Move assignment operator (deleted)
    ComLoggerDp& operator=(ComLoggerDp&&) = delete;

    //! Configure initial logging state; must be called once after init()
    //! If enabled is true and packetsPerContainer == 0, logging stays disabled (no event is emitted)
    //! \param enabled: whether data product logging is initially enabled
    //! \param packetsPerContainer: number of packets per container (must be > 0 if enabled is true, ignored otherwise)
    //! \param priority: data product priority (ignored if enabled is false)
    //! \param flushTimeout: number of schedIn calls without packets before auto-flush (0 = disable auto-flush)
    void configure(bool enabled, U32 packetsPerContainer, FwDpPriorityType priority, U32 flushTimeout);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comIn
    //!
    //! Com input port
    void comIn_handler(FwIndexType portNum,  //!< The port number
                       Fw::ComBuffer& data,  //!< Buffer containing packet data
                       U32 context           //!< Call context value; meaning chosen by user
                       ) override;

    //! Handler implementation for pingIn
    //!
    //! Ping input port
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

    //! Handler implementation for schedIn
    //!
    //! Sched input port for writing telemetry
    void schedIn_handler(FwIndexType portNum,  //!< The port number
                         U32 context           //!< The call order
                         ) override;

    //! Handler implementation for startRecordingIn
    //!
    //! Port to start recording
    void startRecordingIn_handler(FwIndexType portNum,       //!< The port number
                                  U32 packetsPerContainer,   //!< Number of packets per container
                                  FwDpPriorityType priority  //!< Data product priority
                                  ) override;

    //! Handler implementation for stopRecordingIn
    //!
    //! Port to stop recording
    void stopRecordingIn_handler(FwIndexType portNum  //!< The port number
                                 ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command StartComDp
    //!
    //! Starts recording ComBuffers at the specified priority
    void StartComDp_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                               U32 cmdSeq,           //!< The command sequence number
                               U32 packetsPerContainer,
                               FwDpPriorityType priority) override;

    //! Handler implementation for command UpdatePriority
    //!
    //! Updates currently generating data products. If off, no effect
    void UpdatePriority_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq,           //!< The command sequence number
                                   FwDpPriorityType priority) override;

    //! Handler implementation for command StopComDp
    //!
    //! Stops recording buffers
    void StopComDp_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                              U32 cmdSeq            //!< The command sequence number
                              ) override;

    //! Handler implementation for command CLEAR_COUNTERS
    //!
    //! Clears NumBuffersLogged counter and DpBufferError event throttle
    void CLEAR_COUNTERS_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq            //!< The command sequence number
                                   ) override;

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Internal function to start recording
    //! \param packetsPerContainer: Number of packets per container
    //! \param priority: Data product priority
    //! \return true if successful, false if validation failed
    bool startRecordingInternal(U32 packetsPerContainer, FwDpPriorityType priority);

    //! Internal function to stop recording
    //! \return Number of partial containers sent
    U32 stopRecordingInternal();

    //! Internal function to allocate and setup a new container
    //! \return true if allocation succeeded, false if it failed
    bool allocateAndSetupContainer();

    //! Internal function to serialize packet with automatic retry on container full
    //! \param dataPtr: Pointer to the packet data
    //! \param dataSize: Size of the packet data
    //! \return true if serialization succeeded, false if it failed
    bool serializePacketWithRetry(const U8* dataPtr, FwSizeType dataSize);

    //! Send container if it has packets and reset counter
    //! Handles both full and partial containers
    void sendContainerIfNonEmpty();

    //! Internal function to finalize a full container (send and reset)
    void finalizeContainer();

    //! Internal function to handle buffer drop (log event and increment counter)
    //! \param size: Size of the buffer being dropped
    void handleBufferDrop(U32 size);

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Whether data product logging is enabled
    bool m_enabled{false};

    //! Current data product container
    DpContainer m_container;

    //! Target number of packets per container
    U32 m_packetsPerContainer{0};

    //! Current count of packets in container
    U32 m_currentPacketCount{0};

    //! Total number of buffers logged since initialization
    U32 m_numBuffersLogged{0};

    //! Number of buffers dropped due to allocation failure
    U32 m_numBuffersDropped{0};

    //! Number of times packet serialization failed and required retry
    U32 m_numSerializationFailures{0};

    //! Counter for schedIn calls since last packet received
    U32 m_schedCallsSinceLastPacket{0};

    //! Number of schedIn calls without packets before auto-flush (0 = disabled)
    U32 m_flushTimeout{0};

    //! Priority for data products
    FwDpPriorityType m_priority{ContainerPriority::ComBuffContainer};  // Default priority from FPP

    //! Buffer for building records with sentry + ComBuffer data
    //! Size: sentry (4 bytes) + max ComBuffer size
    U8 m_recordBuffer[FW_COM_BUFFER_MAX_SIZE + sizeof(ComLoggerDpSentry)];
};

}  // namespace Svc

#endif
