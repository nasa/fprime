// ====================================================================== 
// \title  Headers.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
