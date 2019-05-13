// ======================================================================
// \title  ActiveLogger/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for ActiveLogger component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ActiveLogger_TESTER_BASE_HPP
#define ActiveLogger_TESTER_BASE_HPP

#include <Svc/ActiveLogger/ActiveLoggerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class ActiveLoggerTesterBase
  //! \brief Auto-generated base class for ActiveLogger component test harness
  //!
  class ActiveLoggerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ActiveLoggerTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect LogRecv to to_LogRecv[portNum]
      //!
      void connect_to_LogRecv(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputLogPort *const LogRecv /*!< The port*/
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

      //! Get the port that receives input from PktSend
      //!
      //! \return from_PktSend[portNum]
      //!
      Fw::InputComPort* get_from_PktSend(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from FatalAnnounce
      //!
      //! \return from_FatalAnnounce[portNum]
      //!
      Svc::InputFatalEventPort* get_from_FatalAnnounce(
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

      //! Construct object ActiveLoggerTesterBase
      //!
      ActiveLoggerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ActiveLoggerTesterBase
      //!
      virtual ~ActiveLoggerTesterBase(void);

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

      //! Handler prototype for from_PktSend
      //!
      virtual void from_PktSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      ) = 0;

      //! Handler base function for from_PktSend
      //!
      void from_PktSend_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler prototype for from_FatalAnnounce
      //!
      virtual void from_FatalAnnounce_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType Id /*!< The ID of the FATAL event*/
      ) = 0;

      //! Handler base function for from_FatalAnnounce
      //!
      void from_FatalAnnounce_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType Id /*!< The ID of the FATAL event*/
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

      //! Push an entry on the history for from_PktSend
      void pushFromPortEntry_PktSend(
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! A history entry for from_PktSend
      //!
      typedef struct {
        Fw::ComBuffer data;
        U32 context;
      } FromPortEntry_PktSend;

      //! The history for from_PktSend
      //!
      History<FromPortEntry_PktSend> 
        *fromPortHistory_PktSend;

      //! Push an entry on the history for from_FatalAnnounce
      void pushFromPortEntry_FatalAnnounce(
          FwEventIdType Id /*!< The ID of the FATAL event*/
      );

      //! A history entry for from_FatalAnnounce
      //!
      typedef struct {
        FwEventIdType Id;
      } FromPortEntry_FatalAnnounce;

      //! The history for from_FatalAnnounce
      //!
      History<FromPortEntry_FatalAnnounce> 
        *fromPortHistory_FatalAnnounce;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to LogRecv
      //!
      void invoke_to_LogRecv(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_LogRecv ports
      //!
      //! \return The number of to_LogRecv ports
      //!
      NATIVE_INT_TYPE getNum_to_LogRecv(void) const;

      //! Get the number of from_PktSend ports
      //!
      //! \return The number of from_PktSend ports
      //!
      NATIVE_INT_TYPE getNum_from_PktSend(void) const;

      //! Get the number of from_FatalAnnounce ports
      //!
      //! \return The number of from_FatalAnnounce ports
      //!
      NATIVE_INT_TYPE getNum_from_FatalAnnounce(void) const;

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
      //! Whether to_LogRecv[portNum] is connected
      //!
      bool isConnected_to_LogRecv(
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

      //! Send a ALOG_SET_EVENT_REPORT_FILTER command
      //!
      void sendCmd_ALOG_SET_EVENT_REPORT_FILTER(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          ActiveLoggerComponentBase::InputFilterLevel FilterLevel, /*!< Filter level*/
          ActiveLoggerComponentBase::InputFilterEnabled FilterEnable /*!< Filter state*/
      );

      //! Send a ALOG_SET_EVENT_SEND_FILTER command
      //!
      void sendCmd_ALOG_SET_EVENT_SEND_FILTER(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          ActiveLoggerComponentBase::SendFilterLevel FilterLevel, /*!< Filter level*/
          ActiveLoggerComponentBase::SendFilterEnabled FilterEnable /*!< Severity filter state*/
      );

      //! Send a ALOG_DUMP_EVENT_LOG command
      //!
      void sendCmd_ALOG_DUMP_EVENT_LOG(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& filename 
      );

      //! Send a ALOG_SET_ID_FILTER command
      //!
      void sendCmd_ALOG_SET_ID_FILTER(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 ID, 
          ActiveLoggerComponentBase::IdFilterEnabled IdFilterEnable /*!< ID filter state*/
      );

      //! Send a ALOG_DUMP_FILTER_STATE command
      //!
      void sendCmd_ALOG_DUMP_FILTER_STATE(
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
      // Event: ALOG_FILE_WRITE_ERR
      // ----------------------------------------------------------------------

      //! Handle event ALOG_FILE_WRITE_ERR
      //!
      virtual void logIn_WARNING_HI_ALOG_FILE_WRITE_ERR(
          ActiveLoggerComponentBase::LogWriteError stage, /*!< The write stage*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event ALOG_FILE_WRITE_ERR
      //!
      typedef struct {
        ActiveLoggerComponentBase::LogWriteError stage;
        I32 error;
      } EventEntry_ALOG_FILE_WRITE_ERR;

      //! The history of ALOG_FILE_WRITE_ERR events
      //!
      History<EventEntry_ALOG_FILE_WRITE_ERR> 
        *eventHistory_ALOG_FILE_WRITE_ERR;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_FILE_WRITE_COMPLETE
      // ----------------------------------------------------------------------

      //! Handle event ALOG_FILE_WRITE_COMPLETE
      //!
      virtual void logIn_ACTIVITY_HI_ALOG_FILE_WRITE_COMPLETE(
          U32 records /*!< number of records written*/
      );

      //! A history entry for event ALOG_FILE_WRITE_COMPLETE
      //!
      typedef struct {
        U32 records;
      } EventEntry_ALOG_FILE_WRITE_COMPLETE;

      //! The history of ALOG_FILE_WRITE_COMPLETE events
      //!
      History<EventEntry_ALOG_FILE_WRITE_COMPLETE> 
        *eventHistory_ALOG_FILE_WRITE_COMPLETE;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_SEVERITY_FILTER_STATE
      // ----------------------------------------------------------------------

      //! Handle event ALOG_SEVERITY_FILTER_STATE
      //!
      virtual void logIn_ACTIVITY_LO_ALOG_SEVERITY_FILTER_STATE(
          ActiveLoggerComponentBase::EventFilterState severity, /*!< The severity level*/
          bool recvEnabled, 
          bool sendEnabled 
      );

      //! A history entry for event ALOG_SEVERITY_FILTER_STATE
      //!
      typedef struct {
        ActiveLoggerComponentBase::EventFilterState severity;
        bool recvEnabled;
        bool sendEnabled;
      } EventEntry_ALOG_SEVERITY_FILTER_STATE;

      //! The history of ALOG_SEVERITY_FILTER_STATE events
      //!
      History<EventEntry_ALOG_SEVERITY_FILTER_STATE> 
        *eventHistory_ALOG_SEVERITY_FILTER_STATE;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_ENABLED
      // ----------------------------------------------------------------------

      //! Handle event ALOG_ID_FILTER_ENABLED
      //!
      virtual void logIn_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(
          U32 ID /*!< The ID filtered*/
      );

      //! A history entry for event ALOG_ID_FILTER_ENABLED
      //!
      typedef struct {
        U32 ID;
      } EventEntry_ALOG_ID_FILTER_ENABLED;

      //! The history of ALOG_ID_FILTER_ENABLED events
      //!
      History<EventEntry_ALOG_ID_FILTER_ENABLED> 
        *eventHistory_ALOG_ID_FILTER_ENABLED;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_LIST_FULL
      // ----------------------------------------------------------------------

      //! Handle event ALOG_ID_FILTER_LIST_FULL
      //!
      virtual void logIn_WARNING_LO_ALOG_ID_FILTER_LIST_FULL(
          U32 ID /*!< The ID filtered*/
      );

      //! A history entry for event ALOG_ID_FILTER_LIST_FULL
      //!
      typedef struct {
        U32 ID;
      } EventEntry_ALOG_ID_FILTER_LIST_FULL;

      //! The history of ALOG_ID_FILTER_LIST_FULL events
      //!
      History<EventEntry_ALOG_ID_FILTER_LIST_FULL> 
        *eventHistory_ALOG_ID_FILTER_LIST_FULL;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_REMOVED
      // ----------------------------------------------------------------------

      //! Handle event ALOG_ID_FILTER_REMOVED
      //!
      virtual void logIn_ACTIVITY_HI_ALOG_ID_FILTER_REMOVED(
          U32 ID /*!< The ID removed*/
      );

      //! A history entry for event ALOG_ID_FILTER_REMOVED
      //!
      typedef struct {
        U32 ID;
      } EventEntry_ALOG_ID_FILTER_REMOVED;

      //! The history of ALOG_ID_FILTER_REMOVED events
      //!
      History<EventEntry_ALOG_ID_FILTER_REMOVED> 
        *eventHistory_ALOG_ID_FILTER_REMOVED;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_NOT_FOUND
      // ----------------------------------------------------------------------

      //! Handle event ALOG_ID_FILTER_NOT_FOUND
      //!
      virtual void logIn_WARNING_LO_ALOG_ID_FILTER_NOT_FOUND(
          U32 ID /*!< The ID removed*/
      );

      //! A history entry for event ALOG_ID_FILTER_NOT_FOUND
      //!
      typedef struct {
        U32 ID;
      } EventEntry_ALOG_ID_FILTER_NOT_FOUND;

      //! The history of ALOG_ID_FILTER_NOT_FOUND events
      //!
      History<EventEntry_ALOG_ID_FILTER_NOT_FOUND> 
        *eventHistory_ALOG_ID_FILTER_NOT_FOUND;

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

      //! To port connected to LogRecv
      //!
      Fw::OutputLogPort m_to_LogRecv[1];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to PktSend
      //!
      Fw::InputComPort m_from_PktSend[1];

      //! From port connected to FatalAnnounce
      //!
      Svc::InputFatalEventPort m_from_FatalAnnounce[1];

      //! From port connected to CmdStatus
      //!
      Fw::InputCmdResponsePort m_from_CmdStatus[1];

      //! From port connected to CmdReg
      //!
      Fw::InputCmdRegPort m_from_CmdReg[1];

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

      //! Static function for port from_PktSend
      //!
      static void from_PktSend_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Static function for port from_FatalAnnounce
      //!
      static void from_FatalAnnounce_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType Id /*!< The ID of the FATAL event*/
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
