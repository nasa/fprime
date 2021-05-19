// ======================================================================
// \title  Health/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for Health component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Health_TESTER_BASE_HPP
#define Health_TESTER_BASE_HPP

#include <Svc/Health/HealthComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class HealthTesterBase
  //! \brief Auto-generated base class for Health component test harness
  //!
  class HealthTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object HealthTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect PingReturn to to_PingReturn[portNum]
      //!
      void connect_to_PingReturn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputPingPort *const PingReturn /*!< The port*/
      );

      //! Connect Run to to_Run[portNum]
      //!
      void connect_to_Run(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputSchedPort *const Run /*!< The port*/
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

      //! Get the port that receives input from PingSend
      //!
      //! \return from_PingSend[portNum]
      //!
      Svc::InputPingPort* get_from_PingSend(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from WdogStroke
      //!
      //! \return from_WdogStroke[portNum]
      //!
      Svc::InputWatchDogPort* get_from_WdogStroke(
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

      //! Construct object HealthTesterBase
      //!
      HealthTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object HealthTesterBase
      //!
      virtual ~HealthTesterBase(void);

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

      //! Handler prototype for from_PingSend
      //!
      virtual void from_PingSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      ) = 0;

      //! Handler base function for from_PingSend
      //!
      void from_PingSend_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Handler prototype for from_WdogStroke
      //!
      virtual void from_WdogStroke_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 code /*!< Watchdog stroke code*/
      ) = 0;

      //! Handler base function for from_WdogStroke
      //!
      void from_WdogStroke_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 code /*!< Watchdog stroke code*/
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

      //! Push an entry on the history for from_PingSend
      void pushFromPortEntry_PingSend(
          U32 key /*!< Value to return to pinger*/
      );

      //! A history entry for from_PingSend
      //!
      typedef struct {
        U32 key;
      } FromPortEntry_PingSend;

      //! The history for from_PingSend
      //!
      History<FromPortEntry_PingSend> 
        *fromPortHistory_PingSend;

      //! Push an entry on the history for from_WdogStroke
      void pushFromPortEntry_WdogStroke(
          U32 code /*!< Watchdog stroke code*/
      );

      //! A history entry for from_WdogStroke
      //!
      typedef struct {
        U32 code;
      } FromPortEntry_WdogStroke;

      //! The history for from_WdogStroke
      //!
      History<FromPortEntry_WdogStroke> 
        *fromPortHistory_WdogStroke;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to PingReturn
      //!
      void invoke_to_PingReturn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Invoke the to port connected to Run
      //!
      void invoke_to_Run(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_PingSend ports
      //!
      //! \return The number of from_PingSend ports
      //!
      NATIVE_INT_TYPE getNum_from_PingSend(void) const;

      //! Get the number of to_PingReturn ports
      //!
      //! \return The number of to_PingReturn ports
      //!
      NATIVE_INT_TYPE getNum_to_PingReturn(void) const;

      //! Get the number of to_Run ports
      //!
      //! \return The number of to_Run ports
      //!
      NATIVE_INT_TYPE getNum_to_Run(void) const;

      //! Get the number of from_WdogStroke ports
      //!
      //! \return The number of from_WdogStroke ports
      //!
      NATIVE_INT_TYPE getNum_from_WdogStroke(void) const;

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
      //! Whether to_PingReturn[portNum] is connected
      //!
      bool isConnected_to_PingReturn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_Run[portNum] is connected
      //!
      bool isConnected_to_Run(
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

      //! Send a HLTH_ENABLE command
      //!
      void sendCmd_HLTH_ENABLE(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          HealthComponentBase::HealthEnabled enable /*!< whether or not health checks are enabled*/
      );

      //! Send a HLTH_PING_ENABLE command
      //!
      void sendCmd_HLTH_PING_ENABLE(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& entry, /*!< The entry to enable/disable*/
          HealthComponentBase::PingEnabled enable /*!< whether or not a port is pinged*/
      );

      //! Send a HLTH_CHNG_PING command
      //!
      void sendCmd_HLTH_CHNG_PING(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& entry, /*!< The entry to modify*/
          U32 warningValue, /*!< Ping warning threshold*/
          U32 fatalValue /*!< Ping fatal threshold*/
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
      // Event: HLTH_PING_WARN
      // ----------------------------------------------------------------------

      //! Handle event HLTH_PING_WARN
      //!
      virtual void logIn_WARNING_HI_HLTH_PING_WARN(
          Fw::LogStringArg& entry /*!< The entry passing the warning level*/
      );

      //! A history entry for event HLTH_PING_WARN
      //!
      typedef struct {
        Fw::LogStringArg entry;
      } EventEntry_HLTH_PING_WARN;

      //! The history of HLTH_PING_WARN events
      //!
      History<EventEntry_HLTH_PING_WARN> 
        *eventHistory_HLTH_PING_WARN;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_LATE
      // ----------------------------------------------------------------------

      //! Handle event HLTH_PING_LATE
      //!
      virtual void logIn_FATAL_HLTH_PING_LATE(
          Fw::LogStringArg& entry /*!< The entry passing the warning level*/
      );

      //! A history entry for event HLTH_PING_LATE
      //!
      typedef struct {
        Fw::LogStringArg entry;
      } EventEntry_HLTH_PING_LATE;

      //! The history of HLTH_PING_LATE events
      //!
      History<EventEntry_HLTH_PING_LATE> 
        *eventHistory_HLTH_PING_LATE;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_WRONG_KEY
      // ----------------------------------------------------------------------

      //! Handle event HLTH_PING_WRONG_KEY
      //!
      virtual void logIn_FATAL_HLTH_PING_WRONG_KEY(
          Fw::LogStringArg& entry, /*!< The entry passing the warning level*/
          U32 badKey /*!< The incorrect key value*/
      );

      //! A history entry for event HLTH_PING_WRONG_KEY
      //!
      typedef struct {
        Fw::LogStringArg entry;
        U32 badKey;
      } EventEntry_HLTH_PING_WRONG_KEY;

      //! The history of HLTH_PING_WRONG_KEY events
      //!
      History<EventEntry_HLTH_PING_WRONG_KEY> 
        *eventHistory_HLTH_PING_WRONG_KEY;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_ENABLE
      // ----------------------------------------------------------------------

      //! Handle event HLTH_CHECK_ENABLE
      //!
      virtual void logIn_ACTIVITY_HI_HLTH_CHECK_ENABLE(
          HealthComponentBase::HealthIsEnabled enabled /*!< If health checking is enabled*/
      );

      //! A history entry for event HLTH_CHECK_ENABLE
      //!
      typedef struct {
        HealthComponentBase::HealthIsEnabled enabled;
      } EventEntry_HLTH_CHECK_ENABLE;

      //! The history of HLTH_CHECK_ENABLE events
      //!
      History<EventEntry_HLTH_CHECK_ENABLE> 
        *eventHistory_HLTH_CHECK_ENABLE;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_PING
      // ----------------------------------------------------------------------

      //! Handle event HLTH_CHECK_PING
      //!
      virtual void logIn_ACTIVITY_HI_HLTH_CHECK_PING(
          HealthComponentBase::HealthPingIsEnabled enabled, /*!< If health pinging is enabled for a particular entry*/
          Fw::LogStringArg& entry /*!< The entry passing the warning level*/
      );

      //! A history entry for event HLTH_CHECK_PING
      //!
      typedef struct {
        HealthComponentBase::HealthPingIsEnabled enabled;
        Fw::LogStringArg entry;
      } EventEntry_HLTH_CHECK_PING;

      //! The history of HLTH_CHECK_PING events
      //!
      History<EventEntry_HLTH_CHECK_PING> 
        *eventHistory_HLTH_CHECK_PING;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_LOOKUP_ERROR
      // ----------------------------------------------------------------------

      //! Handle event HLTH_CHECK_LOOKUP_ERROR
      //!
      virtual void logIn_WARNING_LO_HLTH_CHECK_LOOKUP_ERROR(
          Fw::LogStringArg& entry /*!< The entry passing the warning level*/
      );

      //! A history entry for event HLTH_CHECK_LOOKUP_ERROR
      //!
      typedef struct {
        Fw::LogStringArg entry;
      } EventEntry_HLTH_CHECK_LOOKUP_ERROR;

      //! The history of HLTH_CHECK_LOOKUP_ERROR events
      //!
      History<EventEntry_HLTH_CHECK_LOOKUP_ERROR> 
        *eventHistory_HLTH_CHECK_LOOKUP_ERROR;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_UPDATED
      // ----------------------------------------------------------------------

      //! Handle event HLTH_PING_UPDATED
      //!
      virtual void logIn_ACTIVITY_HI_HLTH_PING_UPDATED(
          Fw::LogStringArg& entry, /*!< The entry changed*/
          U32 warn, /*!< The new warning value*/
          U32 fatal /*!< The new FATAL value*/
      );

      //! A history entry for event HLTH_PING_UPDATED
      //!
      typedef struct {
        Fw::LogStringArg entry;
        U32 warn;
        U32 fatal;
      } EventEntry_HLTH_PING_UPDATED;

      //! The history of HLTH_PING_UPDATED events
      //!
      History<EventEntry_HLTH_PING_UPDATED> 
        *eventHistory_HLTH_PING_UPDATED;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_INVALID_VALUES
      // ----------------------------------------------------------------------

      //! Handle event HLTH_PING_INVALID_VALUES
      //!
      virtual void logIn_WARNING_HI_HLTH_PING_INVALID_VALUES(
          Fw::LogStringArg& entry, /*!< The entry changed*/
          U32 warn, /*!< The new warning value*/
          U32 fatal /*!< The new FATAL value*/
      );

      //! A history entry for event HLTH_PING_INVALID_VALUES
      //!
      typedef struct {
        Fw::LogStringArg entry;
        U32 warn;
        U32 fatal;
      } EventEntry_HLTH_PING_INVALID_VALUES;

      //! The history of HLTH_PING_INVALID_VALUES events
      //!
      History<EventEntry_HLTH_PING_INVALID_VALUES> 
        *eventHistory_HLTH_PING_INVALID_VALUES;

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
      // Channel: PingLateWarnings
      // ----------------------------------------------------------------------

      //! Handle channel PingLateWarnings
      //!
      virtual void tlmInput_PingLateWarnings(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel PingLateWarnings
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_PingLateWarnings;

      //! The history of PingLateWarnings values
      //!
      History<TlmEntry_PingLateWarnings> 
        *tlmHistory_PingLateWarnings;

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

      //! To port connected to PingReturn
      //!
      Svc::OutputPingPort m_to_PingReturn[50];

      //! To port connected to Run
      //!
      Svc::OutputSchedPort m_to_Run[1];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to PingSend
      //!
      Svc::InputPingPort m_from_PingSend[50];

      //! From port connected to WdogStroke
      //!
      Svc::InputWatchDogPort m_from_WdogStroke[1];

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

      //! Static function for port from_PingSend
      //!
      static void from_PingSend_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Static function for port from_WdogStroke
      //!
      static void from_WdogStroke_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 code /*!< Watchdog stroke code*/
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
