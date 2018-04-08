// ====================================================================== 
// \title  Headers.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
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

#ifndef Svc_SequenceFiles_FPrime_Headers_HPP
#define Svc_SequenceFiles_FPrime_Headers_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace Headers {

        //! Serialize a header
        void serialize(
            U32 dataSize, //!< Size of data following header, including CRC
            U32 numRecords, //!< Number of records
            FwTimeBaseStoreType timeBase, //!< Time base
            FwTimeContextStoreType timeContext, //!< Time context
            Fw::SerializeBufferBase& destBuffer //!< Destination buffer
        );

      }

    }

  }

}

#endif
