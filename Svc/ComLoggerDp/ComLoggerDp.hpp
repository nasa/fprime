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
                               U32 priority) override;

    //! Handler implementation for command UpdatePriority
    //!
    //! Updates currently generating data products. If off, no effect
    void UpdatePriority_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq,           //!< The command sequence number
                                   U32 priority) override;

    //! Handler implementation for command StopComDp
    //!
    //! Stops recording buffers
    void StopComDp_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                              U32 cmdSeq            //!< The command sequence number
                              ) override;

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
};

}  // namespace Svc

#endif
