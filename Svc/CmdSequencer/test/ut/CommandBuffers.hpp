// ====================================================================== 
// \title  CommandBuffers.hpp
// \author Canham/Bocchino
// \brief  Command buffers for testing sequences
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
