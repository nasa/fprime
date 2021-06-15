// ====================================================================== 
// \title  Records.hpp
// \author Rob Bocchino
// \brief  AMPCS sequence file records
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

#ifndef Svc_SequenceFiles_AMPCS_Records_HPP
#define Svc_SequenceFiles_AMPCS_Records_HPP

#include "gtest/gtest.h"
#include "Svc/CmdSequencer/formats/AMPCSSequence.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace Records {

        enum Constants {
          //! Size of standard arguments
          STANDARD_ARG_SIZE = sizeof(U32),
          //! Standard record size
          STANDARD_SIZE =
            sizeof(AMPCSSequence::Record::TimeFlag::t) +
            sizeof(AMPCSSequence::Record::Time::t) +
            sizeof(AMPCSSequence::Record::CmdLength::t) +
            sizeof(AMPCSSequence::Record::Opcode::t) +
            STANDARD_ARG_SIZE
        };

        //! Serialize a record with a binary command field
        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag, //!< Time flag
            const AMPCSSequence::Record::Time::t time, //!< Time
            const Fw::SerializeBufferBase &cmdField, //!< Command field
            Fw::SerializeBufferBase& dest //!< Destination buffer
        );

        //! Serialize a record with a command field containing an opcode 
        //! and one U32 argument
        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag, //!< Time flag
            const AMPCSSequence::Record::Time::t time, //!< Time
            const AMPCSSequence::Record::Opcode::t opcode, //!< Opcode
            const U32 argument, //!< Argument
            Fw::SerializeBufferBase& dest //!< Destination buffer
        );

        //! Serialize a record with an empty command field
        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag, //!< Time flag
            const AMPCSSequence::Record::Time::t time, //!< Time
            Fw::SerializeBufferBase& dest //!< Destination buffer
        );

      }

    }

  }

}

#endif
