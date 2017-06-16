// ====================================================================== 
// \title  CmdSequencer/test/ut/Tester.hpp
// \author tim
// \brief  hpp file for CmdSequencer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
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

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  class Tester :
    public CmdSequencerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester(void);

      //! Destroy object Tester
      //!
      ~Tester(void);

    public:

      // ---------------------------------------------------------------------- 
      // Tests
      // ---------------------------------------------------------------------- 

      // initialize component
      void initializeTest(void);

      void missingFile(void);

      void badFileCrc(void);

      void emptyFile(void);

      void invalidRecord(void);

      void fileTooLarge(void);

      void lengthZero(void);

      void nominalImmediate(void);

      void nominalRelative(void);

      void nominalTimedRelative(void);

      void nominalImmediatePort(void);

      void cancelCommand(void);

      void manualImmediate(void);

      void manualImmediate2(void);

      void failedCommand(void);

      void pingTest(void);

      void invalidMode(void);

      void noSequence(void);

      void invalidRecordEntries(void);

      void invalidSequenceTime(void);

      void unexpectedCompletion(void);

      void sequenceTimeout(void);

      void invalidManualModes(void);

      void fileLoadErrors(void);

      void dataAfterRecords(void);

    private:

      static const NATIVE_UINT_TYPE TEST_SEQ_BUFFER_SIZE = 255;

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_seqDone
      //!
      void from_seqDone_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Handler for from_comCmdOut
      //!
      void from_comCmdOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts(void);

      //! Initialize components
      //!
      void initComponents(void);

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      CmdSequencerComponentImpl component;

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::TextLogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            );

      void writeBuffer(const Fw::SerializeBufferBase& buffer, const char* fileName);

      void serializeHeader(U32 size,
              U32 records,
              FwTimeBaseStoreType timeBase,
              FwTimeContextStoreType timeContext,
              Fw::SerializeBufferBase& destBuffer);

      void serializeRecord(
              CmdSequencerComponentImpl::CmdRecordDescriptor desc,
              const Fw::Time &time,
              const Fw::ComBuffer &buff,
              Fw::SerializeBufferBase& destBuffer);

      // add CRC to end of buffer
      void serializeCRC(Fw::SerializeBufferBase& destBuffer);

      // helper to clear history and dispatch messages
      void clearAndDispatch(void);

      // file intercepters
      static bool OpenIntercepter(Os::File::Status &stat, const char* fileName, Os::File::Mode mode, void* ptr);
      Os::File::Status m_testOpenStatus;

      // read call modifiers

      static bool ReadIntercepter(Os::File::Status &stat, void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr);
      Os::File::Status m_testReadStatus;
      // How many read calls to let pass before modifying
      NATIVE_UINT_TYPE m_readsToWait;
      // enumeration to tell what kind of error to inject
      typedef enum {
          NO_ERRORS, // don't inject any errors
          FILE_READ_READ_ERROR, // return a bad read status
          FILE_READ_SIZE_ERROR, // return a bad size
          FILE_READ_DATA_ERROR  // return unexpected data
      } FileReadTestType;
      FileReadTestType m_readTestType;
      NATIVE_UINT_TYPE m_readSize;
      BYTE m_readData[TEST_SEQ_BUFFER_SIZE];

  };

} // end namespace Svc

#endif
