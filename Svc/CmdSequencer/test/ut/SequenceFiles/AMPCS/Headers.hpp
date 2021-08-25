// ====================================================================== 
// \title  Headers.hpp
// \author Rob Bocchino
// \brief  AMPCS sequence file headers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
