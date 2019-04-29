// ======================================================================
// \title  BufferLogger/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferLogger component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferLogger_TESTER_BASE_HPP
#define BufferLogger_TESTER_BASE_HPP

#include <Svc/BufferLogger/BufferLoggerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class BufferLoggerTesterBase
  //! \brief Auto-generated base class for BufferLogger component test harness
  //!
  class BufferLoggerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object BufferLoggerTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect bufferSendIn to to_bufferSendIn[portNum]
      //!
      void connect_to_bufferSendIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputBufferSendPort *const bufferSendIn /*!< The port*/
      );

      //! Connect cmdIn to to_cmdIn[portNum]
      //!
      void connect_to_cmdIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const cmdIn /*!< The port*/
      );

      //! Connect comIn to to_comIn[portNum]
      //!
      void connect_to_comIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputComPort *const comIn /*!< The port*/
      );

      //! Connect pingIn to to_pingIn[portNum]
      //!
      void connect_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputPingPort *const pingIn /*!< The port*/
      );

      //! Connect schedIn to to_schedIn[portNum]
      //!
      void connect_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputSchedPort *const schedIn /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from bufferSendOut
      //!
      //! \return from_bufferSendOut[portNum]
      //!
      Fw::InputBufferSendPort* get_from_bufferSendOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdRegOut
      //!
      //! \return from_cmdRegOut[portNum]
      //!
      Fw::InputCmdRegPort* get_from_cmdRegOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdResponseOut
      //!
      //! \return from_cmdResponseOut[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_cmdResponseOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from eventOut
      //!
      //! \return from_eventOut[portNum]
      //!
      Fw::InputLogPort* get_from_eventOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the port that receives input from eventOutText
      //!
      //! \return from_eventOutText[portNum]
      //!
      Fw::InputLogTextPort* get_from_eventOutText(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );
#endif

      //! Get the port that receives input from pingOut
      //!
      //! \return from_pingOut[portNum]
      //!
      Svc::InputPingPort* get_from_pingOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from timeCaller
      //!
      //! \return from_timeCaller[portNum]
      //!
      Fw::InputTimePort* get_from_timeCaller(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from tlmOut
      //!
      //! \return from_tlmOut[portNum]
      //!
      Fw::InputTlmPort* get_from_tlmOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object BufferLoggerTesterBase
      //!
      BufferLoggerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object BufferLoggerTesterBase
      //!
      virtual ~BufferLoggerTesterBase(void);

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

      //! Handler prototype for from_bufferSendOut
      //!
      virtual void from_bufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      ) = 0;

      //! Handler base function for from_bufferSendOut
      //!
      void from_bufferSendOut_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
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

      //! Push an entry on the history for from_bufferSendOut
      void pushFromPortEntry_bufferSendOut(
          Fw::Buffer &fwBuffer 
      );

      //! A history entry for from_bufferSendOut
      //!
      typedef struct {
        Fw::Buffer fwBuffer;
      } FromPortEntry_bufferSendOut;

      //! The history for from_bufferSendOut
      //!
      History<FromPortEntry_bufferSendOut> 
        *fromPortHistory_bufferSendOut;

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

      //! Invoke the to port connected to bufferSendIn
      //!
      void invoke_to_bufferSendIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Invoke the to port connected to comIn
      //!
      void invoke_to_comIn(
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

      //! Invoke the to port connected to schedIn
      //!
      void invoke_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_bufferSendIn ports
      //!
      //! \return The number of to_bufferSendIn ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferSendIn(void) const;

      //! Get the number of from_bufferSendOut ports
      //!
      //! \return The number of from_bufferSendOut ports
      //!
      NATIVE_INT_TYPE getNum_from_bufferSendOut(void) const;

      //! Get the number of to_cmdIn ports
      //!
      //! \return The number of to_cmdIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdIn(void) const;

      //! Get the number of from_cmdRegOut ports
      //!
      //! \return The number of from_cmdRegOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdRegOut(void) const;

      //! Get the number of from_cmdResponseOut ports
      //!
      //! \return The number of from_cmdResponseOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdResponseOut(void) const;

      //! Get the number of to_comIn ports
      //!
      //! \return The number of to_comIn ports
      //!
      NATIVE_INT_TYPE getNum_to_comIn(void) const;

      //! Get the number of from_eventOut ports
      //!
      //! \return The number of from_eventOut ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOut(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_eventOutText ports
      //!
      //! \return The number of from_eventOutText ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOutText(void) const;
#endif

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

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of to_schedIn ports
      //!
      //! \return The number of to_schedIn ports
      //!
      NATIVE_INT_TYPE getNum_to_schedIn(void) const;

      //! Get the number of from_tlmOut ports
      //!
      //! \return The number of from_tlmOut ports
      //!
      NATIVE_INT_TYPE getNum_from_tlmOut(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_bufferSendIn[portNum] is connected
      //!
      bool isConnected_to_bufferSendIn(
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
      //! Whether to_comIn[portNum] is connected
      //!
      bool isConnected_to_comIn(
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
      //! Whether to_schedIn[portNum] is connected
      //!
      bool isConnected_to_schedIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:
    
      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args); 

      //! Send a BL_OpenFile command
      //!
      void sendCmd_BL_OpenFile(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& file 
      );

      //! Send a BL_CloseFile command
      //!
      void sendCmd_BL_CloseFile(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Send a BL_SetLogging command
      //!
      void sendCmd_BL_SetLogging(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          BufferLoggerComponentBase::LogState state 
      );

      //! Send a BL_FlushFile command
      //!
      void sendCmd_BL_FlushFile(
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
      // Event: BL_LogFileClosed
      // ----------------------------------------------------------------------

      //! Handle event BL_LogFileClosed
      //!
      virtual void logIn_DIAGNOSTIC_BL_LogFileClosed(
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event BL_LogFileClosed
      //!
      typedef struct {
        Fw::LogStringArg file;
      } EventEntry_BL_LogFileClosed;

      //! The history of BL_LogFileClosed events
      //!
      History<EventEntry_BL_LogFileClosed> 
        *eventHistory_BL_LogFileClosed;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileOpenError
      // ----------------------------------------------------------------------

      //! Handle event BL_LogFileOpenError
      //!
      virtual void logIn_WARNING_HI_BL_LogFileOpenError(
          U32 errornum, /*!< The error number returned from the open operation*/
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event BL_LogFileOpenError
      //!
      typedef struct {
        U32 errornum;
        Fw::LogStringArg file;
      } EventEntry_BL_LogFileOpenError;

      //! The history of BL_LogFileOpenError events
      //!
      History<EventEntry_BL_LogFileOpenError> 
        *eventHistory_BL_LogFileOpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileValidationError
      // ----------------------------------------------------------------------

      //! Handle event BL_LogFileValidationError
      //!
      virtual void logIn_WARNING_HI_BL_LogFileValidationError(
          Fw::LogStringArg& validationFile, /*!< The validation file*/
          U32 status /*!< The Os::Validate::Status return*/
      );

      //! A history entry for event BL_LogFileValidationError
      //!
      typedef struct {
        Fw::LogStringArg validationFile;
        U32 status;
      } EventEntry_BL_LogFileValidationError;

      //! The history of BL_LogFileValidationError events
      //!
      History<EventEntry_BL_LogFileValidationError> 
        *eventHistory_BL_LogFileValidationError;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileWriteError
      // ----------------------------------------------------------------------

      //! Handle event BL_LogFileWriteError
      //!
      virtual void logIn_WARNING_HI_BL_LogFileWriteError(
          U32 errornum, /*!< The error number returned from the write operation*/
          U32 bytesWritten, /*!< The number of bytes successfully written*/
          U32 bytesToWrite, /*!< The number of bytes attempted*/
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event BL_LogFileWriteError
      //!
      typedef struct {
        U32 errornum;
        U32 bytesWritten;
        U32 bytesToWrite;
        Fw::LogStringArg file;
      } EventEntry_BL_LogFileWriteError;

      //! The history of BL_LogFileWriteError events
      //!
      History<EventEntry_BL_LogFileWriteError> 
        *eventHistory_BL_LogFileWriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_Activated
      // ----------------------------------------------------------------------

      //! Handle event BL_Activated
      //!
      virtual void logIn_ACTIVITY_LO_BL_Activated(
          void
      );

      //! Size of history for event BL_Activated
      //!
      U32 eventsSize_BL_Activated;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_Deativated
      // ----------------------------------------------------------------------

      //! Handle event BL_Deativated
      //!
      virtual void logIn_ACTIVITY_LO_BL_Deativated(
          void
      );

      //! Size of history for event BL_Deativated
      //!
      U32 eventsSize_BL_Deativated;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_NoLogFileOpenInitError
      // ----------------------------------------------------------------------

      //! Handle event BL_NoLogFileOpenInitError
      //!
      virtual void logIn_WARNING_HI_BL_NoLogFileOpenInitError(
          void
      );

      //! Size of history for event BL_NoLogFileOpenInitError
      //!
      U32 eventsSize_BL_NoLogFileOpenInitError;

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
      // Channel: BufferLogger_NumLoggedBuffers
      // ----------------------------------------------------------------------

      //! Handle channel BufferLogger_NumLoggedBuffers
      //!
      virtual void tlmInput_BufferLogger_NumLoggedBuffers(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel BufferLogger_NumLoggedBuffers
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_BufferLogger_NumLoggedBuffers;

      //! The history of BufferLogger_NumLoggedBuffers values
      //!
      History<TlmEntry_BufferLogger_NumLoggedBuffers> 
        *tlmHistory_BufferLogger_NumLoggedBuffers;

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

      //! To port connected to bufferSendIn
      //!
      Fw::OutputBufferSendPort m_to_bufferSendIn[1];

      //! To port connected to cmdIn
      //!
      Fw::OutputCmdPort m_to_cmdIn[1];

      //! To port connected to comIn
      //!
      Fw::OutputComPort m_to_comIn[1];

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

      //! To port connected to schedIn
      //!
      Svc::OutputSchedPort m_to_schedIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to bufferSendOut
      //!
      Fw::InputBufferSendPort m_from_bufferSendOut[1];

      //! From port connected to cmdRegOut
      //!
      Fw::InputCmdRegPort m_from_cmdRegOut[1];

      //! From port connected to cmdResponseOut
      //!
      Fw::InputCmdResponsePort m_from_cmdResponseOut[1];

      //! From port connected to eventOut
      //!
      Fw::InputLogPort m_from_eventOut[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to eventOutText
      //!
      Fw::InputLogTextPort m_from_eventOutText[1];
#endif

      //! From port connected to pingOut
      //!
      Svc::InputPingPort m_from_pingOut[1];

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_bufferSendOut
      //!
      static void from_bufferSendOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Static function for port from_cmdRegOut
      //!
      static void from_cmdRegOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
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

      //! Static function for port from_eventOut
      //!
      static void from_eventOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Static function for port from_eventOutText
      //!
      static void from_eventOutText_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TextLogSeverity severity, /*!< The severity argument*/
          Fw::TextLogString &text /*!< Text of log message*/
      );
#endif

      //! Static function for port from_pingOut
      //!
      static void from_pingOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Static function for port from_timeCaller
      //!
      static void from_timeCaller_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

      //! Static function for port from_tlmOut
      //!
      static void from_tlmOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwChanIdType id, /*!< Telemetry Channel ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
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
