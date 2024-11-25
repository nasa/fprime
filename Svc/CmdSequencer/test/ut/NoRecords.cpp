// ======================================================================
// \title  NoRecords.hpp
// \author Joaquim Silveira
// \brief  Test command sequence with no records
//
// ======================================================================

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/NoRecords.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/NoRecordsFile.hpp"

namespace Svc {

    namespace NoRecords {

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        CmdSequencerTester ::
            CmdSequencerTester(const SequenceFiles::File::Format::t format) :
            Svc::CmdSequencerTester(format)
        {

        }

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        void CmdSequencerTester ::
            Init()
        {
            // Nothing to do
        }

        void CmdSequencerTester ::
            ValidateNoRecords()
        {
            SequenceFiles::NoRecordsFile file(this->format);

            // Set the time
            Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
            this->setTestTime(testTime);

            // Write the file
            const char* const fileName = file.getName().toChar();
            file.write();

            // Validate the file
            this->sendCmd_CS_VALIDATE(0, 0, Fw::CmdStringArg(fileName));
            this->clearAndDispatch();

            // Assert command response
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                0,
                CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
                0,
                Fw::CmdResponse::EXECUTION_ERROR
            );
            // Assert events
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_CS_NoRecords(0, fileName);
        }

        void CmdSequencerTester ::
            RunNoRecords()
        {
            SequenceFiles::NoRecordsFile file(this->format);
            
            // Set the time
            Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
            this->setTestTime(testTime);

            // Write the file
            const char* const fileName = file.getName().toChar();
            file.write();

            // Send run command
            this->sendCmd_CS_RUN(0, 0, Fw::CmdStringArg(fileName), Svc::CmdSequencer_BlockState::NO_BLOCK);
            this->clearAndDispatch();
            // Assert command response
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                0,
                CmdSequencerComponentBase::OPCODE_CS_RUN,
                0,
                Fw::CmdResponse::EXECUTION_ERROR
            );
            // Assert events
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_CS_NoRecords(0, fileName);

        }

    }

}
