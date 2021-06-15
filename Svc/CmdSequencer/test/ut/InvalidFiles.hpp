// ====================================================================== 
// \title  InvalidFiles.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
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

#ifndef Svc_InvalidFiles_HPP
#define Svc_InvalidFiles_HPP

#include "Svc/CmdSequencer/test/ut/Tester.hpp"

namespace Svc {

  namespace InvalidFiles {

    //! Test sequences with immediate commands followed by an EOS marker
    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object Tester
        Tester(
            const SequenceFiles::File::Format::t format = 
            SequenceFiles::File::Format::F_PRIME //!< The file format to use
        );

      public:

        // ---------------------------------------------------------------------- 
        // Tests
        // ---------------------------------------------------------------------- 

        //! Bad CRC
        void BadCRC(void);

        //! Bad record descriptor
        void BadRecordDescriptor(void);

        //! Bad time base
        void BadTimeBase(void);

        //! Bad time context
        void BadTimeContext(void);

        //! Empty file
        void EmptyFile(void);

        //! Extra data after command records
        void DataAfterRecords(void);

        //! File too large
        void FileTooLarge(void);

        //! Microseconds field too short
        void USecFieldTooShort(void);

        //! Missing CRC
        void MissingCRC(void);

        //! Missing file
        void MissingFile(void);

        //! Size field too large
        void SizeFieldTooLarge(void);

        //! Size field too small
        void SizeFieldTooSmall(void);

    };

  }

}

#endif
