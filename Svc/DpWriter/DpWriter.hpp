// ======================================================================
// \title  DpWriter.hpp
// \author bocchino
// \brief  hpp file for DpWriter component implementation class
// ======================================================================

#ifndef Svc_DpWriter_HPP
#define Svc_DpWriter_HPP

#include <DpCfg.hpp>

#include "Fw/Dp/DpContainer.hpp"
#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/String.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Svc/DpWriter/DpWriterComponentAc.hpp"

namespace Svc {

class DpWriter : public DpWriterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpWriter
    //!
    DpWriter(const char* const compName  //!< The component name
    );

    //! Destroy object DpWriter
    //!
    ~DpWriter();

    //! Configure writer
    void configure(const Fw::StringBase& dpFileNamePrefix  //!< The file name prefix for writing DP files
    );

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferSendIn
    //!
    void bufferSendIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                              Fw::Buffer& fwBuffer            //!< The buffer
                              ) final;

    //! Handler implementation for schedIn
    //!
    void schedIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                         U32 context                     //!< The call order
                         ) final;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command CLEAR_EVENT_THROTTLE
    //!
    //! Clear event throttling
    void CLEAR_EVENT_THROTTLE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                         U32 cmdSeq            //!< The command sequence number
                                         ) final;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Deserialize the packet header
    //! \return Success or failure
    Fw::Success::T deserializePacketHeader(Fw::Buffer& buffer,         //!< The packet buffer
                                           Fw::DpContainer& container  //!< The container
    );

    //! Perform processing on a packet buffer
    void performProcessing(const Fw::DpContainer& container  //!< The container
    );

    //! Write the file
    //! \return Success or failure
    Fw::Success::T writeFile(const Fw::DpContainer& container,    //!< The container (input)
                             const Fw::FileNameString& fileName,  //!< The file name
                             FwSizeType& fileSize                 //!< The file size (output)
    );

    //! Send the DpWritten notification
    void sendNotification(const Fw::DpContainer& container,    //!< The container
                          const Fw::FileNameString& fileName,  //!< The file name
                          FwSizeType packetSize                //!< The packet size
    );

  PRIVATE:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The number of buffers received
    U32 m_numBuffersReceived = 0;

    //! The number of bytes written
    U64 m_numBytesWritten = 0;

    //! The number of successful writes
    U32 m_numSuccessfulWrites = 0;

    //! The number of failed writes
    U32 m_numFailedWrites = 0;

    //! The number of errors
    U32 m_numErrors = 0;

    //! The file name prefix for writing DP files
    //! The precise meaning depends on the DP format string
    //! For example, this could be a directory path prefix
    Fw::FileNameString m_dpFileNamePrefix;
};

}  // end namespace Svc

#endif
