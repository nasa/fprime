// ====================================================================== 
// \title  BadCRCFile.hpp
// \author Rob Bocchino
// \brief  BadCRCFile interface
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

#ifndef Svc_SequenceFiles_BadCRCFile_HPP
#define Svc_SequenceFiles_BadCRCFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A file with a bad CRC
    class BadCRCFile :
      public File
    {

      public:

        //! Construct a BadCRCFile
        BadCRCFile(
            const Format::t format //!< The file format
        );

      public:

        //! Serialize the file in F Prime format
        void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

        //! Serialize the file in AMPCS format
        void serializeAMPCS(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

        //! Get the CRC
        const CmdSequencerComponentImpl::FPrimeSequence::CRC& getCRC(void) const;

      private:

        //! The CRC
        CmdSequencerComponentImpl::FPrimeSequence::CRC crc;

    };

  }

}

#endif
