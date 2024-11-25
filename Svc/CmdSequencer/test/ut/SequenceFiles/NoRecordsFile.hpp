// ====================================================================== 
// \title  NoRecordsFile.hpp
// \author Joaquim Silveira
// \brief  NoRecords interface


#ifndef Svc_SequenceFiles_NoRecordsFile_HPP
#define Svc_SequenceFiles_NoRecordsFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

    namespace SequenceFiles {

        //! A file containing no records
        class NoRecordsFile :
            public File
        {

        public:

            //! Construct a NoRecordsFile
            NoRecordsFile(
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

        };

    }

}

#endif
