// ======================================================================
// \title  CommandDispatcher/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for CommandDispatcher component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CommandDispatcher_TESTER_BASE_HPP
#define CommandDispatcher_TESTER_BASE_HPP

#include <Svc/CmdDispatcher/CommandDispatcherComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class CommandDispatcherTesterBase
  //! \brief Auto-generated base class for CommandDispatcher component test harness
  //!
  class CommandDispatcherTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object CommandDispatcherTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect compCmdReg to to_compCmdReg[portNum]
      //!
      void connect_to_compCmdReg(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdRegPort *const compCmdReg /*!< The port*/
      );

      //! Connect compCmdStat to to_compCmdStat[portNum]
      //!
      void connect_to_compCmdStat(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdResponsePort *const compCmdStat /*!< The port*/
      );

      //! Connect seqCmdBuff to to_seqCmdBuff[portNum]
      //!
      void connect_to_seqCmdBuff(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputComPort *const seqCmdBuff /*!< The port*/
      );

      //! Connect pingIn to to_pingIn[portNum]
      //!
      void connect_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputPingPort *const pingIn /*!< The port*/
      );

      //! Connect CmdDisp to to_CmdDisp[portNum]
      //!
      void connect_to_CmdDisp(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const CmdDisp /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from compCmdSend
      //!
      //! \return from_compCmdSend[portNum]
      //!
      Fw::InputCmdPort* get_from_compCmdSend(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from seqCmdStatus
      //!
      //! \return from_seqCmdStatus[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_seqCmdStatus(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from pingOut
      //!
      //! \return from_pingOut[portNum]
      //!
      Svc::InputPingPort* get_from_pingOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from CmdStatus
      //!
      //! \return from_CmdStatus[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_CmdStatus(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from CmdReg
      //!
      //! \return from_CmdReg[portNum]
      //!
      Fw::InputCmdRegPort* get_from_CmdReg(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from Tlm
      //!
      //! \return from_Tlm[portNum]
      //!
      Fw::InputTlmPort* get_from_Tlm(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from Time
      //!
      //! \return from_Time[portNum]
      //!
      Fw::InputTimePort* get_from_Time(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from Log
      //!
      //! \return from_Log[portNum]
      //!
      Fw::InputLogPort* get_from_Log(
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

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CommandDispatcherTesterBase
      //!
      CommandDispatcherTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CommandDispatcherTesterBase
      //!
      virtual ~CommandDispatcherTesterBase(void);

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

      //! Handler prototype for from_compCmdSend
      //!
      virtual void from_compCmdSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      ) = 0;

      //! Handler base function for from_compCmdSend
      //!
      void from_compCmdSend_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      );

      //! Handler prototype for from_seqCmdStatus
      //!
      virtual void from_seqCmdStatus_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      ) = 0;

      //! Handler base function for from_seqCmdStatus
      //!
      void from_seqCmdStatus_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
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

      //! Push an entry on the history for from_compCmdSend
      void pushFromPortEntry_compCmdSend(
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      );

      //! A history entry for from_compCmdSend
      //!
      typedef struct {
        FwOpcodeType opCode;
        U32 cmdSeq;
        Fw::CmdArgBuffer args;
      } FromPortEntry_compCmdSend;

      //! The history for from_compCmdSend
      //!
      History<FromPortEntry_compCmdSend>
        *fromPortHistory_compCmdSend;

      //! Push an entry on the history for from_seqCmdStatus
      void pushFromPortEntry_seqCmdStatus(
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! A history entry for from_seqCmdStatus
      //!
      typedef struct {
        FwOpcodeType opCode;
        U32 cmdSeq;
        Fw::CommandResponse response;
      } FromPortEntry_seqCmdStatus;

      //! The history for from_seqCmdStatus
      //!
      History<FromPortEntry_seqCmdStatus>
        *fromPortHistory_seqCmdStatus;

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

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to compCmdReg
      //!
      void invoke_to_compCmdReg(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
      );

      //! Invoke the to port connected to compCmdStat
      //!
      void invoke_to_compCmdStat(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Invoke the to port connected to seqCmdBuff
      //!
      void invoke_to_seqCmdBuff(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Invoke the to port connected to pingIn
      //!
      void invoke_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_compCmdSend ports
      //!
      //! \return The number of from_compCmdSend ports
      //!
      NATIVE_INT_TYPE getNum_from_compCmdSend(void) const;

      //! Get the number of to_compCmdReg ports
      //!
      //! \return The number of to_compCmdReg ports
      //!
      NATIVE_INT_TYPE getNum_to_compCmdReg(void) const;

      //! Get the number of to_compCmdStat ports
      //!
      //! \return The number of to_compCmdStat ports
      //!
      NATIVE_INT_TYPE getNum_to_compCmdStat(void) const;

      //! Get the number of from_seqCmdStatus ports
      //!
      //! \return The number of from_seqCmdStatus ports
      //!
      NATIVE_INT_TYPE getNum_from_seqCmdStatus(void) const;

      //! Get the number of to_seqCmdBuff ports
      //!
      //! \return The number of to_seqCmdBuff ports
      //!
      NATIVE_INT_TYPE getNum_to_seqCmdBuff(void) const;

      //! Get the number of to_pingIn ports
      //!
      //! \return The number of to_pingIn ports
      //!
      NATIVE_INT_TYPE getNum_to_pingIn(void) const;

      //! Get the number of from_pingOut ports
      //!
      //! \return The number of from_pingOut ports
      //!
      NATIVE_INT_TYPE getNum_from_pingOut(void) const;

      //! Get the number of to_CmdDisp ports
      //!
      //! \return The number of to_CmdDisp ports
      //!
      NATIVE_INT_TYPE getNum_to_CmdDisp(void) const;

      //! Get the number of from_CmdStatus ports
      //!
      //! \return The number of from_CmdStatus ports
      //!
      NATIVE_INT_TYPE getNum_from_CmdStatus(void) const;

      //! Get the number of from_CmdReg ports
      //!
      //! \return The number of from_CmdReg ports
      //!
      NATIVE_INT_TYPE getNum_from_CmdReg(void) const;

      //! Get the number of from_Tlm ports
      //!
      //! \return The number of from_Tlm ports
      //!
      NATIVE_INT_TYPE getNum_from_Tlm(void) const;

      //! Get the number of from_Time ports
      //!
      //! \return The number of from_Time ports
      //!
      NATIVE_INT_TYPE getNum_from_Time(void) const;

      //! Get the number of from_Log ports
      //!
      //! \return The number of from_Log ports
      //!
      NATIVE_INT_TYPE getNum_from_Log(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_LogText ports
      //!
      //! \return The number of from_LogText ports
      //!
      NATIVE_INT_TYPE getNum_from_LogText(void) const;
#endif

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_compCmdReg[portNum] is connected
      //!
      bool isConnected_to_compCmdReg(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_compCmdStat[portNum] is connected
      //!
      bool isConnected_to_compCmdStat(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_seqCmdBuff[portNum] is connected
      //!
      bool isConnected_to_seqCmdBuff(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_pingIn[portNum] is connected
      //!
      bool isConnected_to_pingIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_CmdDisp[portNum] is connected
      //!
      bool isConnected_to_CmdDisp(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:

      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args);

      //! Send a CMD_NO_OP command
      //!
      void sendCmd_CMD_NO_OP(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a CMD_NO_OP_STRING command
      //!
      void sendCmd_CMD_NO_OP_STRING(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& arg1 /*!< The String command argument*/
      );

      //! Send a CMD_TEST_CMD_1 command
      //!
      void sendCmd_CMD_TEST_CMD_1(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          I32 arg1, /*!< The I32 command argument*/
          F32 arg2, /*!< The F32 command argument*/
          U8 arg3 /*!< The U8 command argument*/
      );

      //! Send a CMD_CLEAR_TRACKING command
      //!
      void sendCmd_CMD_CLEAR_TRACKING(
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
      // Event: OpCodeRegistered
      // ----------------------------------------------------------------------

      //! Handle event OpCodeRegistered
      //!
      virtual void logIn_DIAGNOSTIC_OpCodeRegistered(
          U32 Opcode, /*!< The opcode to register*/
          I32 port, /*!< The registration port*/
          I32 slot /*!< The dispatch slot it was placed in*/
      );

      //! A history entry for event OpCodeRegistered
      //!
      typedef struct {
        U32 Opcode;
        I32 port;
        I32 slot;
      } EventEntry_OpCodeRegistered;

      //! The history of OpCodeRegistered events
      //!
      History<EventEntry_OpCodeRegistered>
        *eventHistory_OpCodeRegistered;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeDispatched
      // ----------------------------------------------------------------------

      //! Handle event OpCodeDispatched
      //!
      virtual void logIn_COMMAND_OpCodeDispatched(
          U32 Opcode, /*!< The opcode dispatched*/
          I32 port /*!< The port dispatched to*/
      );

      //! A history entry for event OpCodeDispatched
      //!
      typedef struct {
        U32 Opcode;
        I32 port;
      } EventEntry_OpCodeDispatched;

      //! The history of OpCodeDispatched events
      //!
      History<EventEntry_OpCodeDispatched>
        *eventHistory_OpCodeDispatched;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeCompleted
      // ----------------------------------------------------------------------

      //! Handle event OpCodeCompleted
      //!
      virtual void logIn_COMMAND_OpCodeCompleted(
          U32 Opcode /*!< The I32 command argument*/
      );

      //! A history entry for event OpCodeCompleted
      //!
      typedef struct {
        U32 Opcode;
      } EventEntry_OpCodeCompleted;

      //! The history of OpCodeCompleted events
      //!
      History<EventEntry_OpCodeCompleted>
        *eventHistory_OpCodeCompleted;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeError
      // ----------------------------------------------------------------------

      //! Handle event OpCodeError
      //!
      virtual void logIn_WARNING_HI_OpCodeError(
          U32 Opcode, /*!< The opcode with the error*/
          CommandDispatcherComponentBase::ErrorResponse error /*!< The error value*/
      );

      //! A history entry for event OpCodeError
      //!
      typedef struct {
        U32 Opcode;
        CommandDispatcherComponentBase::ErrorResponse error;
      } EventEntry_OpCodeError;

      //! The history of OpCodeError events
      //!
      History<EventEntry_OpCodeError>
        *eventHistory_OpCodeError;

    protected:

      // ----------------------------------------------------------------------
      // Event: MalformedCommand
      // ----------------------------------------------------------------------

      //! Handle event MalformedCommand
      //!
      virtual void logIn_WARNING_HI_MalformedCommand(
          CommandDispatcherComponentBase::CmdSerError Status /*!< The deserialization error*/
      );

      //! A history entry for event MalformedCommand
      //!
      typedef struct {
        CommandDispatcherComponentBase::CmdSerError Status;
      } EventEntry_MalformedCommand;

      //! The history of MalformedCommand events
      //!
      History<EventEntry_MalformedCommand>
        *eventHistory_MalformedCommand;

    protected:

      // ----------------------------------------------------------------------
      // Event: InvalidCommand
      // ----------------------------------------------------------------------

      //! Handle event InvalidCommand
      //!
      virtual void logIn_WARNING_HI_InvalidCommand(
          U32 Opcode /*!< Invalid opcode*/
      );

      //! A history entry for event InvalidCommand
      //!
      typedef struct {
        U32 Opcode;
      } EventEntry_InvalidCommand;

      //! The history of InvalidCommand events
      //!
      History<EventEntry_InvalidCommand>
        *eventHistory_InvalidCommand;

    protected:

      // ----------------------------------------------------------------------
      // Event: TooManyCommands
      // ----------------------------------------------------------------------

      //! Handle event TooManyCommands
      //!
      virtual void logIn_WARNING_HI_TooManyCommands(
          U32 Opcode /*!< The opcode that overflowed the list*/
      );

      //! A history entry for event TooManyCommands
      //!
      typedef struct {
        U32 Opcode;
      } EventEntry_TooManyCommands;

      //! The history of TooManyCommands events
      //!
      History<EventEntry_TooManyCommands>
        *eventHistory_TooManyCommands;

    protected:

      // ----------------------------------------------------------------------
      // Event: NoOpReceived
      // ----------------------------------------------------------------------

      //! Handle event NoOpReceived
      //!
      virtual void logIn_ACTIVITY_HI_NoOpReceived(
          void
      );

      //! Size of history for event NoOpReceived
      //!
      U32 eventsSize_NoOpReceived;

    protected:

      // ----------------------------------------------------------------------
      // Event: NoOpStringReceived
      // ----------------------------------------------------------------------

      //! Handle event NoOpStringReceived
      //!
      virtual void logIn_ACTIVITY_HI_NoOpStringReceived(
          Fw::LogStringArg& message /*!< The NO-OP string that is generated*/
      );

      //! A history entry for event NoOpStringReceived
      //!
      typedef struct {
        Fw::LogStringArg message;
      } EventEntry_NoOpStringReceived;

      //! The history of NoOpStringReceived events
      //!
      History<EventEntry_NoOpStringReceived>
        *eventHistory_NoOpStringReceived;

    protected:

      // ----------------------------------------------------------------------
      // Event: TestCmd1Args
      // ----------------------------------------------------------------------

      //! Handle event TestCmd1Args
      //!
      virtual void logIn_ACTIVITY_HI_TestCmd1Args(
          I32 arg1, /*!< Arg1*/
          F32 arg2, /*!< Arg2*/
          U8 arg3 /*!< Arg3*/
      );

      //! A history entry for event TestCmd1Args
      //!
      typedef struct {
        I32 arg1;
        F32 arg2;
        U8 arg3;
      } EventEntry_TestCmd1Args;

      //! The history of TestCmd1Args events
      //!
      History<EventEntry_TestCmd1Args>
        *eventHistory_TestCmd1Args;

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
      // Channel: CommandsDispatched
      // ----------------------------------------------------------------------

      //! Handle channel CommandsDispatched
      //!
      virtual void tlmInput_CommandsDispatched(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CommandsDispatched
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CommandsDispatched;

      //! The history of CommandsDispatched values
      //!
      History<TlmEntry_CommandsDispatched>
        *tlmHistory_CommandsDispatched;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CommandErrors
      // ----------------------------------------------------------------------

      //! Handle channel CommandErrors
      //!
      virtual void tlmInput_CommandErrors(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel CommandErrors
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_CommandErrors;

      //! The history of CommandErrors values
      //!
      History<TlmEntry_CommandErrors>
        *tlmHistory_CommandErrors;

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

      //! To port connected to compCmdReg
      //!
      Fw::OutputCmdRegPort m_to_compCmdReg[30];

      //! To port connected to compCmdStat
      //!
      Fw::OutputCmdResponsePort m_to_compCmdStat[1];

      //! To port connected to seqCmdBuff
      //!
      Fw::OutputComPort m_to_seqCmdBuff[5];

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to compCmdSend
      //!
      Fw::InputCmdPort m_from_compCmdSend[30];

      //! From port connected to seqCmdStatus
      //!
      Fw::InputCmdResponsePort m_from_seqCmdStatus[5];

      //! From port connected to pingOut
      //!
      Svc::InputPingPort m_from_pingOut[1];

      //! From port connected to CmdStatus
      //!
      Fw::InputCmdResponsePort m_from_CmdStatus[1];

      //! From port connected to CmdReg
      //!
      Fw::InputCmdRegPort m_from_CmdReg[1];

      //! From port connected to Tlm
      //!
      Fw::InputTlmPort m_from_Tlm[1];

      //! From port connected to Time
      //!
      Fw::InputTimePort m_from_Time[1];

      //! From port connected to Log
      //!
      Fw::InputLogPort m_from_Log[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to LogText
      //!
      Fw::InputLogTextPort m_from_LogText[1];
#endif

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_compCmdSend
      //!
      static void from_compCmdSend_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      );

      //! Static function for port from_seqCmdStatus
      //!
      static void from_seqCmdStatus_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Static function for port from_pingOut
      //!
      static void from_pingOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Static function for port from_CmdStatus
      //!
      static void from_CmdStatus_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Static function for port from_CmdReg
      //!
      static void from_CmdReg_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
      );

      //! Static function for port from_Tlm
      //!
      static void from_Tlm_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwChanIdType id, /*!< Telemetry Channel ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
      );

      //! Static function for port from_Time
      //!
      static void from_Time_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

      //! Static function for port from_Log
      //!
      static void from_Log_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
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
