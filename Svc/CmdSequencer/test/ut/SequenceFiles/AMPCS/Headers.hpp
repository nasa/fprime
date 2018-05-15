// ====================================================================== 
// \title  Headers.hpp
// \author Rob Bocchino
// \brief  AMPCS sequence file headers
//
// \copyright
// Copyright (C) 2018 California Institute of Technology.
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

#ifndef Svc_SequenceFiles_AMPCS_Headers_HPP
#define Svc_SequenceFiles_AMPCS_Headers_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace Headers {

        //! Serialize a header with a standard U32 value
        void serialize(
            Fw::SerializeBufferBase& buffer //!< The destination buffer
        );

        //! Serialize a header from a U32 value
        void serialize(
            const U32 value, //!< The value
            Fw::SerializeBufferBase& buffer //!< The destination buffer
        );

      }

    }

  }

}

#endif
