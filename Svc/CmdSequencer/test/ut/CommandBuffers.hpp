// ====================================================================== 
// \title  CommandBuffers.hpp
// \author Canham/Bocchino
// \brief  Command buffers for testing sequences
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef Svc_CommandBuffers_HPP
#define Svc_CommandBuffers_HPP

#include "Fw/Com/ComBuffer.hpp"

namespace Svc {

  namespace CommandBuffers {

    //! Create a command buffer with an opcode and one U32 argument
    void create(
        Fw::ComBuffer& comBuff, //!< The com buffer
        const FwOpcodeType opcode, //!< The opcode
        const U32 argument //!< The argument
    );

  }

}

#endif
