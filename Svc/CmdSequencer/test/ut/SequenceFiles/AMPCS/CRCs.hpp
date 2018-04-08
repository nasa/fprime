// ====================================================================== 
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  AMPCS CRCs
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

#ifndef Svc_SequenceFiles_AMPCS_CRCs_HPP
#define Svc_SequenceFiles_AMPCS_CRCs_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace CRCs {

        //! Type alias
        typedef CmdSequencerComponentImpl::FPrimeSequence::CRC CRC;

        //! Compute a CRC
        void computeCRC(
            Fw::SerializeBufferBase& buffer, //!< Buffer containing the data
            CRC& crc //!< The CRC
        );

        //! Create a CRC32 file
        void createFile(
            Fw::SerializeBufferBase& buffer, //!< Buffer containing the data
            const char *const fileName //!< The source file name
        );

        //! Remove a CRC file
        void removeFile(
            const char *const fileName //!< The source file name
        );

        //! Write a computed CRC to a file
        void writeCRC(
            const U32 crc, //!< The CRC value
            const char *const fileName //!< The source file name
        );

      }

    }

  }

}

#endif
