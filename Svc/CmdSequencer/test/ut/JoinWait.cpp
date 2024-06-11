// ====================================================================== 
// \title  JoinWait.hpp
// \author janamian
// \brief  cpp file for CmdSequencer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include "Svc/CmdSequencer/test/ut/JoinWait.hpp"
#include "Svc/CmdSequencer/test/ut/Relative.hpp"

namespace Svc {

  namespace JoinWait {

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
      test_join_wait_without_active_seq()
    {
      // Send join wait command when there is no active seq
      this->sendCmd_CS_JOIN_WAIT(0, 0);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);
    }

    void CmdSequencerTester ::test_join_wait_with_active_seq() {
      const U32 numRecords = 1;
      SequenceFiles::RelativeFile file(numRecords, this->format);
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Run the sequence
      this->runSequence(0, fileName);
      
      // Assert that timer is set
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimer.m_state
      );
      
      // Run one cycle to make sure nothing is dispatched yet
      this->invoke_to_schedIn(0, 0);
      this->clearAndDispatch();
      ASSERT_from_comCmdOut_SIZE(0);
      ASSERT_EVENTS_SIZE(0);
      
      // Assert that timer hasn't expired
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimer.m_state
      );

      // Request join wait
      this->sendCmd_CS_JOIN_WAIT(0, 0);
      this->clearAndDispatch();
      // Make sure JOIN_WAIT is active
      ASSERT_EVENTS_CS_NoSequenceActive_SIZE(0);
      ASSERT_TRUE(this->component.m_join_waiting);
      
      // Send status back
      this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
      this->clearAndDispatch();

      // Make sure we received completion for both command and join_wait
      ASSERT_EVENTS_SIZE(2);
      // Make sure join wait has been cleared
      ASSERT_FALSE(this->component.m_join_waiting);

      }

  }

}
