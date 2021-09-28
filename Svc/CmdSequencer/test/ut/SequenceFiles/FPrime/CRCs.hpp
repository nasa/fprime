// ====================================================================== 
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file CRCs
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_FPrime_CRCs_HPP
#define Svc_SequenceFiles_FPrime_CRCs_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace CRCs {

        enum Constants {
          //! CRC size
          SIZE = sizeof(U32)
        };

        //! Compute and serialize a CRC
        //! destBuffer contains the input data; CRC gets added to the end
        void serialize(
            Fw::SerializeBufferBase& destBuffer //!< The buffer
        );

      }

    }

  }

}

#endif
