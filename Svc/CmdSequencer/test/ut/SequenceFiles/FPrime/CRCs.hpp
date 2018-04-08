// ====================================================================== 
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file CRCs
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
