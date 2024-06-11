// ======================================================================
// \title  JoinWait.hpp
// \author janamian
// \brief  hpp file for CmdSequencer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef JOIN_WAIT_HPP
#define JOIN_WAIT_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace JoinWait {

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

        //! Test
        void test_join_wait_without_active_seq();

        void test_join_wait_with_active_seq();

    };

  }

}

#endif
