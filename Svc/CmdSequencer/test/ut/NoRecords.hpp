// ======================================================================
// \title  NoRecords.hpp
// \author Joaquim Silveira
// \brief  Test command sequence with no records
//
// ======================================================================

#ifndef Svc_NoRecords_HPP
#define Svc_NoRecords_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

    namespace NoRecords {

        //! Test sequencer behavior with no input files
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

            //! Initialization
            void Init();

            //! Issue a validate command on an empty sequence
            void ValidateNoRecords();

            //! Issue a run command on an empty sequence
            void RunNoRecords();
        };

    }

}

#endif
