// ======================================================================
// \title  CmdSequencer/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for CmdSequencer component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CmdSequencer_TESTER_BASE_HPP
#define CmdSequencer_TESTER_BASE_HPP

#include <Svc/CmdSequencer/CmdSequencerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class CmdSequencerTesterBase
  //! \brief Auto-generated base class for CmdSequencer component test harness
  //!
  class CmdSequencerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object CmdSequencerTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect pingIn to to_pingIn[portNum]
      //!
      void connect_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputPingPort *const pingIn /*!< The port*/
      );

      //! Connect cmdResponseIn to to_cmdResponseIn[portNum]
      //!
      void connect_to_cmdResponseIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdResponsePort *const cmdResponseIn /*!< The port*/
      );

      //! Connect cmdIn to to_cmdIn[portNum]
      //!
      void connect_to_cmdIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const cmdIn /*!< The port*/
      );

      //! Connect schedIn to to_schedIn[portNum]
      //!
      void connect_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputSchedPort *const schedIn /*!< The port*/
      );

      //! Connect seqRunIn to to_seqRunIn[portNum]
      //!
      void connect_to_seqRunIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputCmdSeqInPort *const seqRunIn /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from cmdRegOut
      //!
      //! \return from_cmdRegOut[portNum]
      //!
      Fw::InputCmdRegPort* get_from_cmdRegOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the port that receives input from LogText
      //!
      //! \return from_LogText[portNum]
      //!
      Fw::InputLogTextPort* get_from_LogText(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );
#endif

      //! Get the port that receives input from tlmOut
      //!
      //! \return from_tlmOut[portNum]
      //!
      Fw::InputTlmPort* get_from_tlmOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdResponseOut
      //!
      //! \return from_cmdResponseOut[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_cmdResponseOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from timeCaller
      //!
      //! \return from_timeCaller[portNum]
      //!
      Fw::InputTimePort* get_from_timeCaller(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from comCmdOut
      //!
      //! \return from_comCmdOut[portNum]
      //!
      Fw::InputComPort* get_from_comCmdOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from pingOut
      //!
      //! \return from_pingOut[portNum]
      //!
      Svc::InputPingPort* get_from_pingOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from logOut
      //!
      //! \return from_logOut[portNum]
      //!
      Fw::InputLogPort* get_from_logOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from seqDone
      //!
      //! \return from_seqDone[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_seqDone(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CmdSequencerTesterBase
      //!
      CmdSequencerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CmdSequencerTesterBase
      //!
      virtual ~CmdSequencerTesterBase(void);

      // ----------------------------------------------------------------------
      // Test history
      // ----------------------------------------------------------------------

    protected:

      //! \class History
      //! \brief A history of port inputs
      //!
      template <typename T> class History {

        public:

          //! Create a History
          //!
          History(
              const U32 maxSize /*!< The maximum history size*/
          ) : 
              numEntries(0), 
              maxSize(maxSize) 
          { 
            this->entries = new T[maxSize];
          }

          //! Destroy a History
          //!
          ~History() {
            delete[] this->entries;
          }

          //! Clear the history
          //!
          void clear() { this->numEntries = 0; }

          //! Push an item onto the history
          //!
          void push_back(
              T entry /*!< The item*/
          ) {
            FW_ASSERT(this->numEntries < this->maxSize);
            entries[this->numEntries++] = entry;
          }

          //! Get an item at an index
          //!
          //! \return The item at index i
          //!
          T at(
              const U32 i /*!< The index*/
          ) const {
            FW_ASSERT(i < this->numEntries);
            return entries[i];
          }

          //! Get the number of entries in the history
          //!
          //! \return The number of entries in the history
          //!
          U32 size(void) const { return this->numEntries; }

        private:

          //! The number of entries in the history
          //!
          U32 numEntries;

          //! The maximum history size
          //!
          const U32 maxSize;

          //! The entries
          //!
          T *entries;

      };

      //! Clear all history
      //!
      void clearHistory(void);

    protected:

      // ----------------------------------------------------------------------
      // Handler prototypes for typed from ports
      // ----------------------------------------------------------------------

      //! Handler prototype for from_comCmdOut
      //!
      virtual void from_comCmdOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      ) = 0;

      //! Handler base function for from_comCmdOut
      //!
      void from_comCmdOut_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler prototype for from_pingOut
      //!
      virtual void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      ) = 0;

      //! Handler base function for from_pingOut
      //!
      void from_pingOut_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Handler prototype for from_seqDone
      //!
      virtual void from_seqDone_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      ) = 0;

      //! Handler base function for from_seqDone
      //!
      void from_seqDone_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Histories for typed from ports
      // ----------------------------------------------------------------------

      //! Clear from port history
      //!
      void clearFromPortHistory(void);

      //! The total number of from port entries
      //!
      U32 fromPortHistorySize;

      //! Push an entry on the history for from_comCmdOut
      void pushFromPortEntry_comCmdOut(
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! A history entry for from_comCmdOut
      //!
      typedef struct {
        Fw::ComBuffer data;
        U32 context;
      } FromPortEntry_comCmdOut;

      //! The history for from_comCmdOut
      //!
      History<FromPortEntry_comCmdOut> 
        *fromPortHistory_comCmdOut;

      //! Push an entry on the history for from_pingOut
      void pushFromPortEntry_pingOut(
          U32 key /*!< Value to return to pinger*/
      );

      //! A history entry for from_pingOut
      //!
      typedef struct {
        U32 key;
      } FromPortEntry_pingOut;

      //! The history for from_pingOut
      //!
      History<FromPortEntry_pingOut> 
        *fromPortHistory_pingOut;

      //! Push an entry on the history for from_seqDone
      void pushFromPortEntry_seqDone(
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! A history entry for from_seqDone
      //!
      typedef struct {
        FwOpcodeType opCode;
        U32 cmdSeq;
        Fw::CommandResponse response;
      } FromPortEntry_seqDone;

      //! The history for from_seqDone
      //!
      History<FromPortEntry_seqDone> 
        *fromPortHistory_seqDone;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to pingIn
      //!
      void invoke_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Invoke the to port connected to cmdResponseIn
      //!
      void invoke_to_cmdResponseIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Invoke the to port connected to schedIn
      //!
      void invoke_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Invoke the to port connected to seqRunIn
      //!
      void invoke_to_seqRunIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::EightyCharString &filename /*!< The sequence file*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_cmdRegOut ports
      //!
      //! \return The number of from_cmdRegOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdRegOut(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_LogText ports
      //!
      //! \return The number of from_LogText ports
      //!
      NATIVE_INT_TYPE getNum_from_LogText(void) const;
#endif

      //! Get the number of to_pingIn ports
      //!
      //! \return The number of to_pingIn ports
      //!
      NATIVE_INT_TYPE getNum_to_pingIn(void) const;

      //! Get the number of from_tlmOut ports
      //!
      //! \return The number of from_tlmOut ports
      //!
      NATIVE_INT_TYPE getNum_from_tlmOut(void) const;

      //! Get the number of from_cmdResponseOut ports
      //!
      //! \return The number of from_cmdResponseOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdResponseOut(void) const;

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of from_comCmdOut ports
      //!
      //! \return The number of from_comCmdOut ports
      //!
      NATIVE_INT_TYPE getNum_from_comCmdOut(void) const;

      //! Get the number of from_pingOut ports
      //!
      //! \return The number of from_pingOut ports
      //!
      NATIVE_INT_TYPE getNum_from_pingOut(void) const;

      //! Get the number of to_cmdResponseIn ports
      //!
      //! \return The number of to_cmdResponseIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdResponseIn(void) const;

      //! Get the number of to_cmdIn ports
      //!
      //! \return The number of to_cmdIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdIn(void) const;

      //! Get the number of to_schedIn ports
      //!
      //! \return The number of to_schedIn ports
      //!
      NATIVE_INT_TYPE getNum_to_schedIn(void) const;

      //! Get the number of to_seqRunIn ports
      //!
      //! \return The number of to_seqRunIn ports
      //!
      NATIVE_INT_TYPE getNum_to_seqRunIn(void) const;

      //! Get the number of from_logOut ports
      //!
      //! \return The number of from_logOut ports
      //!
      NATIVE_INT_TYPE getNum_from_logOut(void) const;

      //! Get the number of from_seqDone ports
      //!
      //! \return The number of from_seqDone ports
      //!
      NATIVE_INT_TYPE getNum_from_seqDone(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_pingIn[portNum] is connected
      //!
      bool isConnected_to_pingIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_cmdResponseIn[portNum] is connected
      //!
      bool isConnected_to_cmdResponseIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_cmdIn[portNum] is connected
      //!
      bool isConnected_to_cmdIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_schedIn[portNum] is connected
      //!
      bool isConnected_to_schedIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_seqRunIn[portNum] is connected
      //!
      bool isConnected_to_seqRunIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:
    
      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args); 

      //! Send a CS_RUN command
      //!
      void sendCmd_CS_RUN(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& fileName /*!< The name of the sequence file*/
      );

      //! Send a CS_VALIDATE command
      //!
      void sendCmd_CS_VALIDATE(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& fileName /*!< The name of the sequence file*/
      );

      //! Send a CS_CANCEL command
      //!
      void sendCmd_CS_CANCEL(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a CS_START command
      //!
      void sendCmd_CS_START(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a CS_STEP command
      //!
      void sendCmd_CS_STEP(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a CS_AUTO command
      //!
      void sendCmd_CS_AUTO(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a CS_MANUAL command
      //!
      void sendCmd_CS_MANUAL(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Command response handling
      // ----------------------------------------------------------------------

      //! Handle a command response
      //!
      virtual void cmdResponseIn(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CommandResponse response /*!< The command response*/
      );

      //! A type representing a command response
      //!
      typedef struct {
        FwOpcodeType opCode;
        U32 cmdSeq;
        Fw::CommandResponse response;
      } CmdResponse;

      //! The command response history
      //!
      History<CmdResponse> *cmdResponseHistory;

    protected:

      // ----------------------------------------------------------------------
      // Event dispatch
      // ----------------------------------------------------------------------

      //! Dispatch an event
      //!
      void dispatchEvents(
          const FwEventIdType id, /*!< The event ID*/
          Fw::Time& timeTag, /*!< The time*/
          const Fw::LogSeverity severity, /*!< The severity*/
          Fw::LogBuffer& args /*!< The serialized arguments*/
      );

      //! Clear event history
      //!
      void clearEvents(void);

      //! The total number of events seen
      //!
      U32 eventsSize;

#if FW_ENABLE_TEXT_LOGGING

    protected:

      // ----------------------------------------------------------------------
      // Text events
      // ----------------------------------------------------------------------

      //! Handle a text event
      //!
      virtual void textLogIn(
          const FwEventIdType id, /*!< The event ID*/
          Fw::Time& timeTag, /*!< The time*/
          const Fw::TextLogSeverity severity, /*!< The severity*/
          const Fw::TextLogString& text /*!< The event string*/
      );

      //! A history entry for the text log
      //!
      typedef struct {
        U32 id;
        Fw::Time timeTag;
        Fw::TextLogSeverity severity;
        Fw::TextLogString text;
      } TextLogEntry;

      //! The history of text log events
      //!
      History<TextLogEntry> *textLogHistory;

      //! Print a text log history entry
      //!
      static void printTextLogHistoryEntry(
          const TextLogEntry& e,
          FILE* file
      );

      //! Print the text log history
      //!
      void printTextLogHistory(FILE *const file);

#endif

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceLoaded
      // ----------------------------------------------------------------------

      //! Handle event CS_SequenceLoaded
      //!
      virtual void logIn_ACTIVITY_LO_CS_SequenceLoaded(
          Fw::LogStringArg& fileName /*!< The name of the sequence file*/
      );

      //! A history entry for event CS_SequenceLoaded
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_CS_SequenceLoaded;

      //! The history of CS_SequenceLoaded events
      //!
      History<EventEntry_CS_SequenceLoaded> 
        *eventHistory_CS_SequenceLoaded;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceCanceled
      // ----------------------------------------------------------------------

      //! Handle event CS_SequenceCanceled
      //!
      virtual void logIn_ACTIVITY_HI_CS_SequenceCanceled(
          Fw::LogStringArg& fileName /*!< The name of the sequence file*/
      );

      //! A history entry for event CS_SequenceCanceled
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_CS_SequenceCanceled;

      //! The history of CS_SequenceCanceled events
      //!
      History<EventEntry_CS_SequenceCanceled> 
        *eventHistory_CS_SequenceCanceled;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileReadError
      // ----------------------------------------------------------------------

      //! Handle event CS_FileReadError
      //!
      virtual void logIn_WARNING_HI_CS_FileReadError(
          Fw::LogStringArg& fileName /*!< The name of the sequence file*/
      );

      //! A history entry for event CS_FileReadError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_CS_FileReadError;

      //! The history of CS_FileReadError events
      //!
      History<EventEntry_CS_FileReadError> 
        *eventHistory_CS_FileReadError;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileInvalid
      // ----------------------------------------------------------------------

      //! Handle event CS_FileInvalid
      //!
      virtual void logIn_WARNING_HI_CS_FileInvalid(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          CmdSequencerComponentBase::FileReadStage stage, /*!< The read stage*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event CS_FileInvalid
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        CmdSequencerComponentBase::FileReadStage stage;
        I32 error;
      } EventEntry_CS_FileInvalid;

      //! The history of CS_FileInvalid events
      //!
      History<EventEntry_CS_FileInvalid> 
        *eventHistory_CS_FileInvalid;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_RecordInvalid
      // ----------------------------------------------------------------------

      //! Handle event CS_RecordInvalid
      //!
      virtual void logIn_WARNING_HI_CS_RecordInvalid(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U32 recordNumber, /*!< The record number*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event CS_RecordInvalid
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 recordNumber;
        I32 error;
      } EventEntry_CS_RecordInvalid;

      //! The history of CS_RecordInvalid events
      //!
      History<EventEntry_CS_RecordInvalid> 
        *eventHistory_CS_RecordInvalid;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileSizeError
      // ----------------------------------------------------------------------

      //! Handle event CS_FileSizeError
      //!
      virtual void logIn_WARNING_HI_CS_FileSizeError(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U32 size /*!< Inavalid size*/
      );

      //! A history entry for event CS_FileSizeError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 size;
      } EventEntry_CS_FileSizeError;

      //! The history of CS_FileSizeError events
      //!
      History<EventEntry_CS_FileSizeError> 
        *eventHistory_CS_FileSizeError;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileNotFound
      // ----------------------------------------------------------------------

      //! Handle event CS_FileNotFound
      //!
      virtual void logIn_WARNING_HI_CS_FileNotFound(
          Fw::LogStringArg& fileName /*!< The sequence file*/
      );

      //! A history entry for event CS_FileNotFound
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_CS_FileNotFound;

      //! The history of CS_FileNotFound events
      //!
      History<EventEntry_CS_FileNotFound> 
        *eventHistory_CS_FileNotFound;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileCrcFailure
      // ----------------------------------------------------------------------

      //! Handle event CS_FileCrcFailure
      //!
      virtual void logIn_WARNING_HI_CS_FileCrcFailure(
          Fw::LogStringArg& fileName, /*!< The sequence file*/
          U32 storedCRC, /*!< The CRC stored in the file*/
          U32 computedCRC /*!< The CRC computed over the file*/
      );

      //! A history entry for event CS_FileCrcFailure
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 storedCRC;
        U32 computedCRC;
      } EventEntry_CS_FileCrcFailure;

      //! The history of CS_FileCrcFailure events
      //!
      History<EventEntry_CS_FileCrcFailure> 
        *eventHistory_CS_FileCrcFailure;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CommandComplete
      // ----------------------------------------------------------------------

      //! Handle event CS_CommandComplete
      //!
      virtual void logIn_ACTIVITY_LO_CS_CommandComplete(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U32 recordNumber, /*!< The record number of the command*/
          U32 opCode /*!< The command opcode*/
      );

      //! A history entry for event CS_CommandComplete
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 recordNumber;
        U32 opCode;
      } EventEntry_CS_CommandComplete;

      //! The history of CS_CommandComplete events
      //!
      History<EventEntry_CS_CommandComplete> 
        *eventHistory_CS_CommandComplete;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceComplete
      // ----------------------------------------------------------------------

      //! Handle event CS_SequenceComplete
      //!
      virtual void logIn_ACTIVITY_HI_CS_SequenceComplete(
          Fw::LogStringArg& fileName /*!< The name of the sequence file*/
      );

      //! A history entry for event CS_SequenceComplete
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_CS_SequenceComplete;

      //! The history of CS_SequenceComplete events
      //!
      History<EventEntry_CS_SequenceComplete> 
        *eventHistory_CS_SequenceComplete;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CommandError
      // ----------------------------------------------------------------------

      //! Handle event CS_CommandError
      //!
      virtual void logIn_WARNING_HI_CS_CommandError(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U32 recordNumber, /*!< The record number*/
          U32 opCode, /*!< The opcode*/
          U32 errorStatus /*!< The error status*/
      );

      //! A history entry for event CS_CommandError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 recordNumber;
        U32 opCode;
        U32 errorStatus;
      } EventEntry_CS_CommandError;

      //! The history of CS_CommandError events
      //!
      History<EventEntry_CS_CommandError> 
        *eventHistory_CS_CommandError;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_InvalidMode
      // ----------------------------------------------------------------------

      //! Handle event CS_InvalidMode
      //!
      virtual void logIn_WARNING_HI_CS_InvalidMode(
          void
      );

      //! Size of history for event CS_InvalidMode
      //!
      U32 eventsSize_CS_InvalidMode;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_RecordMismatch
      // ----------------------------------------------------------------------

      //! Handle event CS_RecordMismatch
      //!
      virtual void logIn_WARNING_HI_CS_RecordMismatch(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U32 header_records, /*!< The number of records in the header*/
          U32 extra_bytes /*!< The number of bytes beyond last record*/
      );

      //! A history entry for event CS_RecordMismatch
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 header_records;
        U32 extra_bytes;
      } EventEntry_CS_RecordMismatch;

      //! The history of CS_RecordMismatch events
      //!
      History<EventEntry_CS_RecordMismatch> 
        *eventHistory_CS_RecordMismatch;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_TimeBaseMismatch
      // ----------------------------------------------------------------------

      //! Handle event CS_TimeBaseMismatch
      //!
      virtual void logIn_WARNING_HI_CS_TimeBaseMismatch(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U16 time_base, /*!< The current time*/
          U16 seq_time_base /*!< The sequence time base*/
      );

      //! A history entry for event CS_TimeBaseMismatch
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U16 time_base;
        U16 seq_time_base;
      } EventEntry_CS_TimeBaseMismatch;

      //! The history of CS_TimeBaseMismatch events
      //!
      History<EventEntry_CS_TimeBaseMismatch> 
        *eventHistory_CS_TimeBaseMismatch;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_TimeContextMismatch
      // ----------------------------------------------------------------------

      //! Handle event CS_TimeContextMismatch
      //!
      virtual void logIn_WARNING_HI_CS_TimeContextMismatch(
          Fw::LogStringArg& fileName, /*!< The name of the sequence file*/
          U8 currTimeBase, /*!< The current time base*/
          U8 seqTimeBase /*!< The sequence time base*/
      );

      //! A history entry for event CS_TimeContextMismatch
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U8 currTimeBase;
        U8 seqTimeBase;
      } EventEntry_CS_TimeContextMismatch;

      //! The history of CS_TimeContextMismatch events
      //!
      History<EventEntry_CS_TimeContextMismatch> 
        *eventHistory_CS_TimeContextMismatch;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_PortSequenceStarted
      // ----------------------------------------------------------------------

      //! Handle event CS_PortSequenceStarted
      //!
      virtual void logIn_ACTIVITY_HI_CS_PortSequenceStarted(
          Fw::LogStringArg& filename /*!< The sequence file*/
      );

      //! A history entry for event CS_PortSequenceStarted
      //!
      typedef struct {
        Fw::LogStringArg filename;
      } EventEntry_CS_PortSequenceStarted;

      //! The history of CS_PortSequenceStarted events
      //!
      History<EventEntry_CS_PortSequenceStarted> 
        *eventHistory_CS_PortSequenceStarted;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_UnexpectedCompletion
      // ----------------------------------------------------------------------

      //! Handle event CS_UnexpectedCompletion
      //!
      virtual void logIn_WARNING_HI_CS_UnexpectedCompletion(
          U32 opcode /*!< The reported opcode*/
      );

      //! A history entry for event CS_UnexpectedCompletion
      //!
      typedef struct {
        U32 opcode;
      } EventEntry_CS_UnexpectedCompletion;

      //! The history of CS_UnexpectedCompletion events
      //!
      History<EventEntry_CS_UnexpectedCompletion> 
        *eventHistory_CS_UnexpectedCompletion;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_ModeSwitched
      // ----------------------------------------------------------------------

      //! Handle event CS_ModeSwitched
      //!
      virtual void logIn_ACTIVITY_HI_CS_ModeSwitched(
          CmdSequencerComponentBase::SeqMode mode /*!< The new mode*/
      );

      //! A history entry for event CS_ModeSwitched
      //!
      typedef struct {
        CmdSequencerComponentBase::SeqMode mode;
      } EventEntry_CS_ModeSwitched;

      //! The history of CS_ModeSwitched events
      //!
      History<EventEntry_CS_ModeSwitched> 
        *eventHistory_CS_ModeSwitched;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_NoSequenceActive
      // ----------------------------------------------------------------------

      //! Handle event CS_NoSequenceActive
      //!
      virtual void logIn_WARNING_LO_CS_NoSequenceActive(
          void
      );

      //! Size of history for event CS_NoSequenceActive
      //!
      U32 eventsSize_CS_NoSequenceActive;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceValid
      // ----------------------------------------------------------------------

      //! Handle event CS_SequenceValid
      //!
      virtual void logIn_ACTIVITY_HI_CS_SequenceValid(
          Fw::LogStringArg& filename /*!< The sequence file*/
      );

      //! A history entry for event CS_SequenceValid
      //!
      typedef struct {
        Fw::LogStringArg filename;
      } EventEntry_CS_SequenceValid;

      //! The history of CS_SequenceValid events
      //!
      History<EventEntry_CS_SequenceValid> 
        *eventHistory_CS_SequenceValid;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceTimeout
      // ----------------------------------------------------------------------

      //! Handle event CS_SequenceTimeout
      //!
      virtual void logIn_WARNING_HI_CS_SequenceTimeout(
          Fw::LogStringArg& filename, /*!< The sequence file*/
          U32 command /*!< The command that timed out*/
      );

      //! A history entry for event CS_SequenceTimeout
      //!
      typedef struct {
        Fw::LogStringArg filename;
        U32 command;
      } EventEntry_CS_SequenceTimeout;

      //! The history of CS_SequenceTimeout events
      //!
      History<EventEntry_CS_SequenceTimeout> 
        *eventHistory_CS_SequenceTimeout;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CmdStepped
      // ----------------------------------------------------------------------

      //! Handle event CS_CmdStepped
      //!
      virtual void logIn_ACTIVITY_HI_CS_CmdStepped(
          Fw::LogStringArg& filename, /*!< The sequence file*/
          U32 command /*!< The command that was stepped*/
      );

      //! A history entry for event CS_CmdStepped
      //!
      typedef struct {
        Fw::LogStringArg filename;
        U32 command;
      } EventEntry_CS_CmdStepped;

      //! The history of CS_CmdStepped events
      //!
      History<EventEntry_CS_CmdStepped> 
        *eventHistory_CS_CmdStepped;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CmdStarted
      // ----------------------------------------------------------------------

      //! Handle event CS_CmdStarted
      //!
      virtual void logIn_ACTIVITY_HI_CS_CmdStarted(
          Fw::LogStringArg& filename /*!< The sequence file*/
      );

      //! A history entry for event CS_CmdStarted
      //!
      typedef struct {
        Fw::LogStringArg filename;
      } EventEntry_CS_CmdStarted;

      //! The history of CS_CmdStarted events
      //!
      History<EventEntry_CS_CmdStarted> 
        *eventHistory_CS_CmdStarted;

    protected:

      // ----------------------------------------------------------------------
      // Telemetry dispatch
      // ----------------------------------------------------------------------

      //! Dispatch telemetry
      //!
      void dispatchTlm(
          const FwChanIdType id, /*!< The channel ID*/
          const Fw::Time& timeTag, /*!< The time*/
          Fw::TlmBuffer& val /*!< The channel value*/
      );

      //! Clear telemetry history
      //!
      void clearTlm(void);

      //! The total number of telemetry inputs seen
      //!
      U32 tlmSize;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_LoadCommands
      // ----------------------------------------------------------------------

      //! Handle channel CS_LoadCommands
      //!
      virtual void tlmInput_CS_LoadCommands(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CS_LoadCommands
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CS_LoadCommands;

      //! The history of CS_LoadCommands values
      //!
      History<TlmEntry_CS_LoadCommands> 
        *tlmHistory_CS_LoadCommands;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_CancelCommands
      // ----------------------------------------------------------------------

      //! Handle channel CS_CancelCommands
      //!
      virtual void tlmInput_CS_CancelCommands(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CS_CancelCommands
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CS_CancelCommands;

      //! The history of CS_CancelCommands values
      //!
      History<TlmEntry_CS_CancelCommands> 
        *tlmHistory_CS_CancelCommands;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_Errors
      // ----------------------------------------------------------------------

      //! Handle channel CS_Errors
      //!
      virtual void tlmInput_CS_Errors(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CS_Errors
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CS_Errors;

      //! The history of CS_Errors values
      //!
      History<TlmEntry_CS_Errors> 
        *tlmHistory_CS_Errors;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_CommandsExecuted
      // ----------------------------------------------------------------------

      //! Handle channel CS_CommandsExecuted
      //!
      virtual void tlmInput_CS_CommandsExecuted(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CS_CommandsExecuted
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CS_CommandsExecuted;

      //! The history of CS_CommandsExecuted values
      //!
      History<TlmEntry_CS_CommandsExecuted> 
        *tlmHistory_CS_CommandsExecuted;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_SequencesCompleted
      // ----------------------------------------------------------------------

      //! Handle channel CS_SequencesCompleted
      //!
      virtual void tlmInput_CS_SequencesCompleted(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CS_SequencesCompleted
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CS_SequencesCompleted;

      //! The history of CS_SequencesCompleted values
      //!
      History<TlmEntry_CS_SequencesCompleted> 
        *tlmHistory_CS_SequencesCompleted;

    protected:

      // ----------------------------------------------------------------------
      // Test time
      // ----------------------------------------------------------------------

      //! Set the test time for events and telemetry
      //!
      void setTestTime(
          const Fw::Time& timeTag /*!< The time*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

      //! To port connected to cmdResponseIn
      //!
      Fw::OutputCmdResponsePort m_to_cmdResponseIn[1];

      //! To port connected to cmdIn
      //!
      Fw::OutputCmdPort m_to_cmdIn[1];

      //! To port connected to schedIn
      //!
      Svc::OutputSchedPort m_to_schedIn[1];

      //! To port connected to seqRunIn
      //!
      Svc::OutputCmdSeqInPort m_to_seqRunIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to cmdRegOut
      //!
      Fw::InputCmdRegPort m_from_cmdRegOut[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to LogText
      //!
      Fw::InputLogTextPort m_from_LogText[1];
#endif

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

      //! From port connected to cmdResponseOut
      //!
      Fw::InputCmdResponsePort m_from_cmdResponseOut[1];

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to comCmdOut
      //!
      Fw::InputComPort m_from_comCmdOut[1];

      //! From port connected to pingOut
      //!
      Svc::InputPingPort m_from_pingOut[1];

      //! From port connected to logOut
      //!
      Fw::InputLogPort m_from_logOut[1];

      //! From port connected to seqDone
      //!
      Fw::InputCmdResponsePort m_from_seqDone[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_cmdRegOut
      //!
      static void from_cmdRegOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Static function for port from_LogText
      //!
      static void from_LogText_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TextLogSeverity severity, /*!< The severity argument*/
          Fw::TextLogString &text /*!< Text of log message*/
      );
#endif

      //! Static function for port from_tlmOut
      //!
      static void from_tlmOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwChanIdType id, /*!< Telemetry Channel ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
      );

      //! Static function for port from_cmdResponseOut
      //!
      static void from_cmdResponseOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Static function for port from_timeCaller
      //!
      static void from_timeCaller_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

      //! Static function for port from_comCmdOut
      //!
      static void from_comCmdOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Static function for port from_pingOut
      //!
      static void from_pingOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Static function for port from_logOut
      //!
      static void from_logOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
      );

      //! Static function for port from_seqDone
      //!
      static void from_seqDone_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

    private:

      // ----------------------------------------------------------------------
      // Test time
      // ----------------------------------------------------------------------

      //! Test time stamp
      //!
      Fw::Time m_testTime;

  };

} // end namespace Svc

#endif
