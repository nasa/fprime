// ======================================================================
// \title  CmdSequencerImpl.hpp
// \author Bocchino/Canham
// \brief  hpp file for CmdSequencer component implementation class
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_CmdSequencerImpl_HPP
#define Svc_CmdSequencerImpl_HPP

#include "Fw/Com/ComBuffer.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Os/File.hpp"
#include "Os/ValidateFile.hpp"
#include "Svc/CmdSequencer/CmdSequencerComponentAc.hpp"

namespace Svc {

  class CmdSequencerComponentImpl :
    public CmdSequencerComponentBase
  {

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private enumerations
      // ----------------------------------------------------------------------

      //! The run mode
      enum RunMode {
        STOPPED, RUNNING
      };

      //! The step mode
      enum StepMode {
        AUTO, MANUAL
      };

    public:

      // ----------------------------------------------------------------------
      // Public classes
      // ----------------------------------------------------------------------

      //! \class Sequence
      //! \brief A sequence with unspecified binary format
      class Sequence {

        public:

          //! \class Events
          //! \brief Sequence event reporting
          class Events {

            public:

              //! Construct an Events object
              Events(
                  Sequence& sequence //!< The enclosing sequence
              );

            public:

              //! File CRC failure
              void fileCRCFailure(
                  const U32 storedCRC, //!< The CRC stored in the file
                  const U32 computedCRC //!< The CRC computed over the file
              );

              //! File invalid
              void fileInvalid(
                  const CmdSequencer_FileReadStage::t stage, //!< The file read stage
                  const I32 error //!< The error
              );

              //! File not found
              void fileNotFound();

              //! File read error
              void fileReadError();

              //! File size error
              void fileSizeError(
                  const U32 size //!< The size
              );

              //! Record invalid
              void recordInvalid(
                  const U32 recordNumber, //!< The record number
                  const I32 error //!< The error
              );

              //! Record mismatch
              void recordMismatch(
                  const U32 numRecords, //!< The number of records in the header
                  const U32 extraBytes //!< The number of bytes beyond last record
              );

              //! Time base mismatch
              void timeBaseMismatch(
                  const TimeBase currTimeBase, //!< The current time base
                  const TimeBase seqTimeBase //!< The sequence file time base
              );

              //! Time context mismatch
              void timeContextMismatch(
                  const FwTimeContextStoreType currTimeContext, //!< The current time context
                  const FwTimeContextStoreType seqTimeContext //!< The sequence file time context
              );

            PRIVATE:

              //! The enclosing component
              Sequence& m_sequence;

          };

        public:

          //! Construct a Sequence object
          Sequence(
              CmdSequencerComponentImpl& component //!< The enclosing component
          );

          //! Destroy a Sequence object
          virtual ~Sequence();

        public:

          //! \class Header
          //! \brief A sequence header
          class Header {

            public:

              enum Constants {
                //! Serialized size of header
                SERIALIZED_SIZE =
                  sizeof(U32) +
                  sizeof(U32) +
                  sizeof(FwTimeBaseStoreType) +
                  sizeof(FwTimeContextStoreType)
              };

            public:

              //! Construct a Header object
              Header();

            public:

              //! Validate the time field of the sequence header
              //! \return Success or failure
              bool validateTime(
                  CmdSequencerComponentImpl& component //!< Component for time and events
              );

            public:

              //! The file size
              U32 m_fileSize;

              //! The number of records in the sequence
              U32 m_numRecords;

              //! The time base of the sequence
              TimeBase m_timeBase;

              //! The context of the sequence
              FwTimeContextStoreType m_timeContext;

          };

        public:

          //! \class Record
          //! \brief A sequence record
          class Record {

            public:

              enum Descriptor {
                ABSOLUTE, //!< Absolute time
                RELATIVE, //!< Relative time
                END_OF_SEQUENCE //!< end of sequence
              };

            public:

              //! Construct a Record object
              Record() :
                m_descriptor(END_OF_SEQUENCE)
              {

              }

            public:

              //! The descriptor
              Descriptor m_descriptor;

              //! The time tag. NOTE: timeBase and context not filled in
              Fw::Time m_timeTag;

              //! The command
              Fw::ComBuffer m_command;

          };

        public:

          //! Give the sequence representation a memory buffer
          void allocateBuffer(
              NATIVE_INT_TYPE identifier, //!< The identifier
              Fw::MemAllocator& allocator, //!< The allocator
              NATIVE_UINT_TYPE bytes //!< The number of bytes
          );

          //! Deallocate the buffer
          void deallocateBuffer(
              Fw::MemAllocator& allocator //!< The allocator
          );

          //! Set the file name. Also sets the log file name.
          void setFileName(const Fw::StringBase& fileName);

          //! Get the file name
          //! \return The file name
          Fw::CmdStringArg& getFileName();

          //! Get the log file name
          //! \return The log file name
          Fw::LogStringArg& getLogFileName();

          //! Get the sequence header
          const Header& getHeader() const;

          //! Load a sequence file
          //! \return Success or failure
          virtual bool loadFile(
              const Fw::StringBase& fileName //!< The file name
          ) = 0;

          //! Query whether the sequence has any more records
          //! \return Yes or no
          virtual bool hasMoreRecords() const = 0;

          //! Get the next record in the sequence
          //! Asserts on failure
          virtual void nextRecord(
              Record& record //!< The returned record
          ) = 0;

          //! Reset the sequence to the beginning.
          //! After calling this, hasMoreRecords should return true,
          //! unless the sequence has no records
          virtual void reset() = 0;

          //! Clear the sequence records.
          //! After calling this, hasMoreRecords should return false
          virtual void clear() = 0;

        PROTECTED:

          //! The enclosing component
          CmdSequencerComponentImpl& m_component;

          //! Event reporting
          Events m_events;

          //! The sequence file name
          Fw::CmdStringArg m_fileName;

          //! Copy of file name for events
          Fw::LogStringArg m_logFileName;

          //! Serialize buffer to hold the binary sequence data
          Fw::ExternalSerializeBuffer m_buffer;

          //! The allocator ID
          NATIVE_INT_TYPE m_allocatorId;

          //! The sequence header
          Header m_header;

      };

      //! \class FPrimeSequence
      //! \brief A sequence that uses the F Prime binary format
      class FPrimeSequence :
        public Sequence
      {

        PRIVATE:

          enum Constants {
            INITIAL_COMPUTED_VALUE = 0xFFFFFFFFU
          };

        public:

          //! \class CRC
          //! \brief Container for computed and stored CRC values
          struct CRC {

            //! Construct a CRC
            CRC();

            //! Initialize computed CRC
            void init();

            //! Update computed CRC
            void update(
                const BYTE* buffer, //!< The buffer
                NATIVE_UINT_TYPE bufferSize //!< The buffer size
            );

            //! Finalize computed CRC
            void finalize();

            //! Computed CRC
            U32 m_computed;

            //! Stored CRC
            U32 m_stored;

          };

        public:

          //! Construct an FPrimeSequence
          FPrimeSequence(
              CmdSequencerComponentImpl& component //!< The enclosing component
          );

        public:

          //! Load a sequence file
          //! \return Success or failure
          bool loadFile(
              const Fw::StringBase& fileName //!< The file name
          );

          //! Query whether the sequence has any more records
          //! \return Yes or no
          bool hasMoreRecords() const;

          //! Get the next record in the sequence.
          //! Asserts on failure
          void nextRecord(
              Record& record //!< The returned record
          );

          //! Reset the sequence to the beginning.
          //! After calling this, hasMoreRecords should return true, unless
          //! the sequence has no records.
          void reset();

          //! Clear the sequence records.
          //! After calling this, hasMoreRecords should return false.
          void clear();

        PRIVATE:

          //! Read a sequence file
          //! \return Success or failure
          bool readFile();

          //! Read an open sequence file
          //! \return Success or failure
          bool readOpenFile();

          //! Read a binary sequence header from the sequence file
          //! into the buffer
          //! \return Success or failure
          bool readHeader();

          //! Deserialize the binary sequence header from the buffer
          //! \return Success or failure
          bool deserializeHeader();

          //! Read records and CRC into buffer
          //! \return Success or failure
          bool readRecordsAndCRC();

          //! Extract CRC from record data
          //! \return Success or failure
          bool extractCRC();

          //! Validate the CRC
          //! \return Success or failure
          bool validateCRC();

          //! Deserialize a record from a buffer
          //! \return Serialize status
          Fw::SerializeStatus deserializeRecord(
              Record& record //!< The record
          );

          //! Deserialize a record descriptor
          //! \return Serialize status
          Fw::SerializeStatus deserializeDescriptor(
              Record::Descriptor& descriptor //!< The descriptor
          );

          //! Deserialize a time tag
          //! \return Serialize status
          Fw::SerializeStatus deserializeTimeTag(
              Fw::Time& timeTag //!< The time tag
          );

          //! Deserialize the record size
          //! \return Serialize status
          Fw::SerializeStatus deserializeRecordSize(
              U32& recordSize //!< The record size
          );

          //! Copy the serialized command into a com buffer
          //! \return Serialize status
          Fw::SerializeStatus copyCommand(
              Fw::ComBuffer& comBuffer, //!< The com buffer
              const U32 recordSize //!< The record size
          );

          //! Validate the sequence records in the buffer
          //! \return Success or failure
          bool validateRecords();

        PRIVATE:

          //! The CRC values
          CRC m_crc;

          //! The sequence file
          Os::File m_sequenceFile;

      };

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private classes
      // ----------------------------------------------------------------------

      //! \class Timer
      //! \brief A class representing a timer
      class Timer {

        PRIVATE:

          //! The timer state
          typedef enum {
            SET, CLEAR
          } State;

        public:

          //! Construct a Timer object
          Timer() :
            m_state(CLEAR)
          {

          }

          //! Set the expiration time
          void set(
              Fw::Time time //!< The time
          ) {
            this->m_state = SET;
            this->expirationTime = time;
          }

          //! Clear the timer
          void clear() {
            this->m_state = CLEAR;
          }

          //! Determine whether the timer is expired at a given time
          //! \return Yes or no
          bool isExpiredAt(
              Fw::Time time //!< The time
          ) {
            if (this->m_state == CLEAR) {
              return false;
            } else if (
                Fw::Time::compare(this->expirationTime, time) == Fw::Time::GT
            ) {
              return false;
            }
            return true;
          }

        PRIVATE:

          //! The timer state
          State m_state;

          //! The expiration time
          Fw::Time expirationTime;

      };


    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct a CmdSequencer
      CmdSequencerComponentImpl(
          const char* compName //!< The component name
      );

      //! Initialize a CmdSequencer
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! (Optional) Set a timeout.
      //! Sequence will quit if a command takes longer than the number of
      //! seconds in the timeout value.
      void setTimeout(
          const NATIVE_UINT_TYPE seconds //!< The number of seconds
      );

      //! (Optional) Set the sequence format.
      //! CmdSequencer will use the sequence object you pass in
      //! to load and run sequences. By default, it uses an FPrimeSequence
      //! object.
      void setSequenceFormat(
          Sequence& sequence //!< The sequence object
      );

      //! Give the sequence a memory buffer.
      //! Call this after constructor and init, and after setting
      //! the sequence format, but before task is spawned.
      void allocateBuffer(
          const NATIVE_INT_TYPE identifier, //!< The identifier
          Fw::MemAllocator& allocator, //!< The allocator
          const NATIVE_UINT_TYPE bytes //!< The number of bytes
      );

      //! (Optional) Load a sequence to run later.
      //! When you call this function, the event ports must be connected.
      void loadSequence(
          const Fw::StringBase& fileName //!< The file name
      );

      //! Return allocated buffer. Call during shutdown.
      void deallocateBuffer(
          Fw::MemAllocator& allocator //!< The allocator
      );

      //! Destroy a CmdDispatcherComponentBase
      ~CmdSequencerComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for input ports
      // ----------------------------------------------------------------------

      //! Handler for input port cmdResponseIn
      void cmdResponseIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          FwOpcodeType opcode, //!< The command opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdResponse& response //!< The command response
      );

      //! Handler for input port schedIn
      void schedIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE order //!< The call order
      );

      //! Handler for input port seqRunIn
      void seqRunIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::String &filename //!< The sequence file
      );

      //! Handler for ping port
      void pingIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

      //! Handler implementation for seqCancelIn
      //!
      void seqCancelIn_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Handler for command CS_AUTO
      //! Set the run mode to AUTO.
      void CS_AUTO_cmdHandler(
          FwOpcodeType opcode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command CS_CANCEL
      //! Validate a command sequence file
      void CS_CANCEL_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command CS_MANUAL
      //! Set the run mode to MANUAL.
      void CS_MANUAL_cmdHandler(
          FwOpcodeType opcode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command CS_RUN
      void CS_RUN_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& fileName, //!< The file name
          Svc::CmdSequencer_BlockState block /*!< Return command status when complete or not*/
      );

      //! Handler for command CS_START
      //! Start running a command sequence
      void CS_START_cmdHandler(
          FwOpcodeType opcode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command CS_STEP
      //! Perform one step in a command sequence.
      //! Valid only if SequenceRunner is in MANUAL run mode.
      void CS_STEP_cmdHandler(
          FwOpcodeType opcode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command CS_VALIDATE
      //! Run a command sequence file
      void CS_VALIDATE_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& fileName //!< The name of the sequence file
      );

      //! Implementation for CS_JOIN command handler
      //! Wait for sequences that are running to finish.
			//! Allow user to run multiple seq files in SEQ_NO_BLOCK mode
			//! then wait for them to finish before allowing more seq run request.
      void CS_JOIN_WAIT_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private helper methods
      // ----------------------------------------------------------------------

      //! Load a sequence file
      //! \return Success or failure
      bool loadFile(
          const Fw::StringBase& fileName //!< The file name
      );

      //! Perform a Cancel command
      void performCmd_Cancel();

      //! Perform a Step command
      void performCmd_Step();

      //! Perform a Step command with a relative time
      void performCmd_Step_RELATIVE(
          Fw::Time& currentTime //!< The time
      );

      //! Perform a Step command with an absolute time
      void performCmd_Step_ABSOLUTE(
          Fw::Time& currentTime //!< The time
      );

      //! Record a completed command
      void commandComplete(
          const U32 opCode //!< The opcode
      );

      //! Record a sequence complete event
      void sequenceComplete();

      //! Record an error
      void error();

      //! Record an error in executing a sequence command
      void commandError(
          const U32 number, //!< The command number
          const U32 opCode, //!< The command opcode
          const U32 error //!< The error code
      );

      //! Require a run mode
      //! \return Whether we are in the correct mode
      bool requireRunMode(
          RunMode mode //!< The required mode
      );

      //! Set command timeout timer
      void setCmdTimeout(
          const Fw::Time &currentTime //!< The current time
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The F Prime sequence
      FPrimeSequence m_FPrimeSequence;

      //! The abstract sequence
      Sequence *m_sequence;

      //! The number of Load commands executed
      U32 m_loadCmdCount;

      //! The number of Cancel commands executed
      U32 m_cancelCmdCount;

      //! The number of errors
      U32 m_errorCount;

      //! The run mode
      RunMode m_runMode;

      //! The step mode
      StepMode m_stepMode;

      //! The sequence record currently being processed
      Sequence::Record m_record;

      //! The command time timer
      Timer m_cmdTimer;

      //! The number of commands executed in this sequence
      U32 m_executedCount;

      //! The total number of commands executed across all sequences
      U32 m_totalExecutedCount;

      //! The total number of sequences completed
      U32 m_sequencesCompletedCount;

      //! timeout value
      NATIVE_UINT_TYPE m_timeout;

      //! timeout timer
      Timer m_cmdTimeoutTimer;

      //! Block mode for command status
      Svc::CmdSequencer_BlockState::t m_blockState;
      FwOpcodeType m_opCode;
      U32 m_cmdSeq;
      bool m_join_waiting;
  };

}

#endif
