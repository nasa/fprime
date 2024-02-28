// ======================================================================
// \title  CmdSequencerTester.hpp
// \author Bocchino/Canham
// \brief  CmdSequencer test interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Tester_HPP
#define Svc_Tester_HPP

#include "Fw/Types/MallocAllocator.hpp"
#include "CmdSequencerGTestBase.hpp"
#include "Os/Posix/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
#include "Svc/CmdSequencer/formats/AMPCSSequence.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SequenceFiles.hpp"
#include "Svc/CmdSequencer/test/ut/UnitTest.hpp"

#define ALLOCATOR_ID 100
#define BUFFER_SIZE 1024
#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10
#define TIMEOUT 100

namespace Svc {

  class CmdSequencerTester :
    public CmdSequencerGTestBase
  {

    private:

      // ----------------------------------------------------------------------
      // Constants
      // ----------------------------------------------------------------------

      static const NATIVE_UINT_TYPE TEST_SEQ_BUFFER_SIZE = 255;

    protected:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      //! Mode for executing commands
      struct CmdExecMode {

        typedef enum {
          //! Don't start a new sequence
          NO_NEW_SEQUENCE,
          //! Start a new sequence
          NEW_SEQUENCE
        } t;

      };

      //! Sequences encoding binary formats
      struct Sequences {

        //! Construct a Sequences object
        Sequences(
            CmdSequencerComponentImpl& component //!< The component under test
        ) :
          ampcsSequence(component)
        {

        }

        //! The AMPCS sequence
        AMPCSSequence ampcsSequence;

      };
  public:
      class Interceptor {

          public:

            // ----------------------------------------------------------------------
            // Types
            // ----------------------------------------------------------------------

            //! Type of injected errors
            struct EnableType {

              typedef enum {
                NONE, // Don't enable interception
                OPEN, // Intercept opens
                READ, // Intercept reads
              } t;
            };

          //! Type of injected errors
          struct ErrorType {

              typedef enum {
                  NONE, // Don't inject any errors
                  READ, // Bad read status
                  SIZE, // Bad size
                  DATA  // Unexpected data
              } t;

          };

          public:

            // ----------------------------------------------------------------------
            // Constructors
            // ----------------------------------------------------------------------

            Interceptor();

          public:

            // ----------------------------------------------------------------------
            // Public instance methods
            // ----------------------------------------------------------------------

            //! Enable the interceptor
            void enable(EnableType::t enableType);

            //! Disable the interceptor
            void disable();

            class PosixFileInterceptor : public Os::Posix::File::PosixFile {
                friend class Interceptor;
              public:
                PosixFileInterceptor() = default;

                PosixFileInterceptor(const PosixFileInterceptor& other) = default;

                Os::FileInterface::Status open(const char *path, Mode mode, OverwriteType overwrite) override;

                Status read(U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

                //! Current interceptor
                static Interceptor* s_current_interceptor;
            };
          public:

            // ----------------------------------------------------------------------
            // Public member variables
            // ----------------------------------------------------------------------
            EnableType::t enabled;

            //! Error type
            ErrorType::t errorType;

            //! How many read calls to let pass before modifying
            U32 waitCount;

            //! Read data
            BYTE data[TEST_SEQ_BUFFER_SIZE];

            //! Read size
            U32 size;

            //! Status
            Os::File::Status fileStatus;

        };



      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CmdSequencerTester
      CmdSequencerTester(
          const SequenceFiles::File::Format::t format =
          SequenceFiles::File::Format::F_PRIME //!< The file format to use
      );

      //! Destroy object CmdSequencerTester
      ~CmdSequencerTester();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_seqDone
      //!
      void from_seqDone_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          FwOpcodeType opCode, //!< Command Op Code
          U32 cmdSeq, //!< Command Sequence
          const Fw::CmdResponse& response //!< The command response argument
      );

      //! Handler for from_comCmdOut
      //!
      void from_comCmdOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      );

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

#if VERBOSE
    protected:

      // ----------------------------------------------------------------------
      // TesterBase interface
      // ----------------------------------------------------------------------

      //! Handle a text event
      void textLogIn(
          const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
      );
#endif

    protected:

      // ----------------------------------------------------------------------
      // Virtual function interface
      // ----------------------------------------------------------------------

      //! Execute sequence commands for an automatic sequence
      virtual void executeCommandsAuto(
          const char *const fileName, //!< The file name
          const U32 numCommands, //!< The number of commands in the sequence
          const U32 bound, //!< The number of commands to run
          const CmdExecMode::t mode //!< The mode
      );

      //! Execute sequence commands with a command response error
      virtual void executeCommandsError(
          const char *const fileName, //!< The file name
          const U32 numCommands //!< The number of commands in the sequence
      );

      //! Execute commands for a manual sequence
      virtual void executeCommandsManual(
          const char *const fileName, //!< The file name
          const U32 numCommands //!< The number of commands in the sequence
      );

    protected:

      // ----------------------------------------------------------------------
      // Tests parameterized by file type
      // ----------------------------------------------------------------------

      //! Run an automatic sequence by command
      virtual void parameterizedAutoByCommand(
          SequenceFiles::File& file, //!< The file
          const U32 numCommands, //!< The number of commands in the sequence
          const U32 bound //!< The number of commands to execute
      );

      //! Inject data read errors
      void parameterizedDataReadErrors(
          SequenceFiles::File& file //!< The file
      );

      //! Inject file errors
      void parameterizedFileErrors(
          SequenceFiles::File& file //!< The file
      );

      //! Inject file open errors
      void parameterizedFileOpenErrors(
          SequenceFiles::File& file //!< The file
      );

      //! Inject header read errors
      void parameterizedHeaderReadErrors(
          SequenceFiles::File& file //!< The file
      );

      //! Run a sequence with failed commands
      void parameterizedFailedCommands(
          SequenceFiles::File& file, //!< The file
          const U32 numCommands //!< The number of commands to run
      );

      //! Don't load any sequence, then try to run a sequence
      void parameterizedNeverLoaded();

      //! Sequence timeout
      void parameterizedSequenceTimeout(
          SequenceFiles::File& file //!< The file
      );

      //! Start and cancel a sequence
      void parameterizedCancel(
          SequenceFiles::File& file, //!< The file
          const U32 numCommands, //!< The number of commands in the sequence
          const U32 bound //!< The number of commands to execute
      );

      //! Run a complete sequence and then issue a command response
      void parameterizedUnexpectedCommandResponse(
          SequenceFiles::File& file, //!< The file
          const U32 numCommands, //!< The number of commands in the sequence
          const U32 bound //!< The number of commands to execute
      );

      //! Validate a sequence
      void parameterizedValidate(
          SequenceFiles::File& file //!< The file
      );

    protected:

      // ----------------------------------------------------------------------
      // Instance helper methods
      // ----------------------------------------------------------------------

      //! Cancel a sequence
      void cancelSequence(
          const U32 cmdSeq, //!< The command sequence number
          const char* const fileName //!< The file name
      );

      //! Clear history and dispatch messages
      void clearAndDispatch();

      //! Connect ports
      void connectPorts();

      //! Go to auto mode
      void goToAutoMode(
          const U32 cmdSeq //!< The command sequence number
      );

      //! Go to manual mode
      void goToManualMode(
          const U32 cmdSeq //!< The command sequence number
      );

      //! Initialize components
      void initComponents();

      //! Load a sequence
      void loadSequence(
          const char* const fileName //!< The file name
      );

      //! Run a loaded sequence
      void runLoadedSequence();

      //! Run a sequence by command
      void runSequence(
          const U32 cmdSeq, //!< The command sequence number
          const char* const fileName //!< The file name
      );

      //! Run a sequence by port call
      void runSequenceByPortCall(
          const char* const fileName //!< The file name
      );

      //! Send a step command
      void stepSequence(
          const U32 cmdSeq //!< The command sequence number
      );

      //! Set the component sequence format
      void setComponentSequenceFormat();

      //! Start a new sequence while checking command buffers
      void startNewSequence(
          const char *const fileName //!< The file name
      );

      //! Start a sequence in manual mode
      void startSequence(
          const U32 cmdSeq, //!< The command sequence number
          const char* const fileName //!< The file name
      );

      //! Validate a sequence file
      void validateFile(
          const U32 cmdSeq, //!< The command sequence number
          const char* const fileName //!< The file name
      );

    protected:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      CmdSequencerComponentImpl component;

      //! The file format to use
      const SequenceFiles::File::Format::t format;

      //! Sequences encoding binary formats
      Sequences sequences;

      //! The allocator
      Fw::MallocAllocator mallocator;


      //! Open/Read interceptor
      Interceptor interceptor;
  };

}

#endif
