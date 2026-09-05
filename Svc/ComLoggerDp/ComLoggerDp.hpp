// ======================================================================
// \title  ComLoggerDp.hpp
// \author tcanham
// \brief  hpp file for ComLoggerDp component implementation class
// ======================================================================

#ifndef Svc_ComLoggerDp_HPP
#define Svc_ComLoggerDp_HPP

#include "Svc/ComLoggerDp/ComLoggerDpComponentAc.hpp"

namespace Svc {

class ComLoggerDp final : public ComLoggerDpComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ComLoggerDp object
    ComLoggerDp(const char* const compName  //!< The component name
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

    // ----------------------------------------------------------------------
    // Public interface
    // ----------------------------------------------------------------------

    //! Configure the ComLoggerDp
    //! \param enabled: whether data product logging is initially enabled
    void configure(bool enabled);

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
    void startRecordingIn_handler(FwIndexType portNum,          //!< The port number
                                  U32 packetsPerContainer,       //!< Number of packets per container
                                  FwDpPriorityType priority      //!< Data product priority
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

    //! Internal function to finalize a full container (send and reset)
    void finalizeFullContainer();

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

    //! Priority for data products
    FwDpPriorityType m_priority{5};  // Default priority from FPP
};

}  // namespace Svc

#endif
