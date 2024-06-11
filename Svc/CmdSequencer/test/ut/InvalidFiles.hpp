// ======================================================================
// \title  InvalidFiles.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_InvalidFiles_HPP
#define Svc_InvalidFiles_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace InvalidFiles {

    //! Test sequences with immediate commands followed by an EOS marker
    class CmdSequencerTester :
      public Svc::CmdSequencerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object CmdSequencerTester
        CmdSequencerTester(
            const SequenceFiles::File::Format::t format =
            SequenceFiles::File::Format::F_PRIME //!< The file format to use
        );

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Bad CRC
        void BadCRC();

        //! Bad record descriptor
        void BadRecordDescriptor();

        //! Bad time base
        void BadTimeBase();

        //! Bad time context
        void BadTimeContext();

        //! Empty file
        void EmptyFile();

        //! Extra data after command records
        void DataAfterRecords();

        //! File too large
        void FileTooLarge();

        //! Microseconds field too short
        void USecFieldTooShort();

        //! Missing CRC
        void MissingCRC();

        //! Missing file
        void MissingFile();

        //! Size field too large
        void SizeFieldTooLarge();

        //! Size field too small
        void SizeFieldTooSmall();

    };

  }

}

#endif
