// ====================================================================== 
// \title  Records.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file records
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_FPrime_Records_HPP
#define Svc_SequenceFiles_FPrime_Records_HPP

#include "gtest/gtest.h"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace Records {

        typedef CmdSequencerComponentImpl::Sequence::Record::Descriptor Descriptor;

        enum Constants {
          //! Record descriptor size
          RECORD_DESCRIPTOR_SIZE = sizeof(U8),
          //! Seconds size
          SECONDS_SIZE = sizeof(U32),
          //! Microseconds size
          MICROSECONDS_SIZE = sizeof(U32),
          //! Size of record size
          RECORD_SIZE_SIZE = sizeof(U32),
          //! Size of standard arguments
          STANDARD_ARG_SIZE = sizeof(U32),
          //! Standard record size
          STANDARD_SIZE =
            RECORD_DESCRIPTOR_SIZE +
            SECONDS_SIZE +
            MICROSECONDS_SIZE +
            RECORD_SIZE_SIZE +
            sizeof(FwPacketDescriptorType) +
            sizeof(FwOpcodeType) +
            STANDARD_ARG_SIZE,
          //! EOS record size
          EOS_SIZE = RECORD_DESCRIPTOR_SIZE
        };

        //! Serialize a record with pre-serialized opcode and argument
        void serialize(
            Records::Descriptor desc, //!< Descriptor
            const Fw::Time &time, //!< Time
            const Fw::ComBuffer &opcodeAndArgument, //!< Serialized opcode and argument
            Fw::SerializeBufferBase& destBuffer //!< Destination buffer
        );

        //! Serialize a record with an opcode and one U32 argument
        void serialize(
            Descriptor desc, //!< Descriptor
            const Fw::Time &time, //!< Time
            const FwOpcodeType opcode, //!< Opcode
            const U32 argument, //!< Argument
            Fw::SerializeBufferBase& destBuffer //!< Destination buffer
        );

        //! Serialize a record with empty opcode and argument
        void serialize(
            Descriptor desc, //!< Descriptor
            const Fw::Time &time, //!< Time
            Fw::SerializeBufferBase& destBuffer //!< Destination buffer
        );

      }

    }

  }

}

#endif
