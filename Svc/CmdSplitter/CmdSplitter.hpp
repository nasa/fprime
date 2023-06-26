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

class CmdSplitter : public CmdSplitterComponentBase {
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

    PRIVATE :

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for CmdBuff
    //!
    void CmdBuff_handler(const NATIVE_INT_TYPE portNum, /*!< The port number */
                         Fw::ComBuffer& data,           /*!< Buffer containing packet data */
                         U32 context                    /*!< Call context value; meaning chosen by user */
    );

    //! Handler implementation for seqCmdStatus
    //!
    void seqCmdStatus_handler(const NATIVE_INT_TYPE portNum,  /*!< The port number */
                              FwOpcodeType opCode,            /*!< Command Op Code */
                              U32 cmdSeq,                     /*!< Command Sequence */
                              const Fw::CmdResponse& response /*!< The command response argument */
    );
};

}  // end namespace Svc

#endif
