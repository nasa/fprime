// ======================================================================
// \title  CmdSplitter.hpp
// \author watney
// \brief  hpp file for CmdSplitter component implementation class
// ======================================================================

#ifndef CmdSplitter_HPP
#define CmdSplitter_HPP

#include <Fw/Cmd/CmdResponsePortAc.hpp>
#include "Svc/CmdSplitter/CmdSplitterComponentAc.hpp"

namespace Svc {

class CmdSplitter final : public CmdSplitterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object CmdSplitter
    //!
    CmdSplitter(const char* const compName /*!< The component name*/
    );

    //! Destroy object CmdSplitter
    //!
    ~CmdSplitter();

    //! Configure this splitter
    //!
    void configure(const FwOpcodeType remoteBaseOpcode /*!< Base remote opcode*/);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for CmdBuff
    //!
    void CmdBuff_handler(const FwIndexType portNum,      /*!< The port number */
                         Fw::ComBuffer& data,            /*!< Buffer containing command */
                         const ComCfg::Apid& packetType, /*!< APID of the packet */
                         U32 context                     /*!< User context */
                         ) override;

    //! Handler implementation for seqCmdStatus
    //!
    void seqCmdStatus_handler(const FwIndexType portNum,      /*!< The port number */
                              FwOpcodeType opCode,            /*!< Command Op Code */
                              U32 cmdSeq,                     /*!< Command Sequence */
                              const Fw::CmdResponse& response /*!< The command response argument */
                              ) override;

    FwOpcodeType m_remoteBase;  // Opcodes greater than or equal than this value will route remotely
};

}  // end namespace Svc

#endif
