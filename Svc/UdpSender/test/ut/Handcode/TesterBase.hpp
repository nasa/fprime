// ======================================================================
// \title  UdpSender/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for UdpSender component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpSender_TESTER_BASE_HPP
#define UdpSender_TESTER_BASE_HPP

#include <Svc/UdpSender/UdpSenderComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class UdpSenderTesterBase
  //! \brief Auto-generated base class for UdpSender component test harness
  //!
  class UdpSenderTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object UdpSenderTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect Sched to to_Sched[portNum]
      //!
      void connect_to_Sched(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputSchedPort *const Sched /*!< The port*/
      );

      //! Connect PortsIn to to_PortsIn[portNum]
      //!
      void connect_to_PortsIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputSerializePort *const PortsIn /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

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

      //! Construct object UdpSenderTesterBase
      //!
      UdpSenderTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object UdpSenderTesterBase
      //!
      virtual ~UdpSenderTesterBase(void);

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
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to Sched
      //!
      void invoke_to_Sched(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Invoke the to port connected to PortsIn
      //!
      void invoke_to_PortsIn(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_Sched ports
      //!
      //! \return The number of to_Sched ports
      //!
      NATIVE_INT_TYPE getNum_to_Sched(void) const;

      //! Get the number of to_PortsIn ports
      //!
      //! \return The number of to_PortsIn ports
      //!
      NATIVE_INT_TYPE getNum_to_PortsIn(void) const;

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
      //! Whether to_Sched[portNum] is connected
      //!
      bool isConnected_to_Sched(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_PortsIn[portNum] is connected
      //!
      bool isConnected_to_PortsIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

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
      // Event: US_PortOpened
      // ----------------------------------------------------------------------

      //! Handle event US_PortOpened
      //!
      virtual void logIn_ACTIVITY_HI_US_PortOpened(
          Fw::LogStringArg& server, 
          U32 port 
      );

      //! A history entry for event US_PortOpened
      //!
      typedef struct {
        Fw::LogStringArg server;
        U32 port;
      } EventEntry_US_PortOpened;

      //! The history of US_PortOpened events
      //!
      History<EventEntry_US_PortOpened> 
        *eventHistory_US_PortOpened;

    protected:

      // ----------------------------------------------------------------------
      // Event: US_SocketError
      // ----------------------------------------------------------------------

      //! Handle event US_SocketError
      //!
      virtual void logIn_WARNING_HI_US_SocketError(
          Fw::LogStringArg& error 
      );

      //! A history entry for event US_SocketError
      //!
      typedef struct {
        Fw::LogStringArg error;
      } EventEntry_US_SocketError;

      //! The history of US_SocketError events
      //!
      History<EventEntry_US_SocketError> 
        *eventHistory_US_SocketError;

    protected:

      // ----------------------------------------------------------------------
      // Event: US_SendError
      // ----------------------------------------------------------------------

      //! Handle event US_SendError
      //!
      virtual void logIn_WARNING_HI_US_SendError(
          Fw::LogStringArg& error 
      );

      //! A history entry for event US_SendError
      //!
      typedef struct {
        Fw::LogStringArg error;
      } EventEntry_US_SendError;

      //! The history of US_SendError events
      //!
      History<EventEntry_US_SendError> 
        *eventHistory_US_SendError;

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
      // Channel: US_PacketsSent
      // ----------------------------------------------------------------------

      //! Handle channel US_PacketsSent
      //!
      virtual void tlmInput_US_PacketsSent(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel US_PacketsSent
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_US_PacketsSent;

      //! The history of US_PacketsSent values
      //!
      History<TlmEntry_US_PacketsSent> 
        *tlmHistory_US_PacketsSent;

    protected:

      // ----------------------------------------------------------------------
      // Channel: US_BytesSent
      // ----------------------------------------------------------------------

      //! Handle channel US_BytesSent
      //!
      virtual void tlmInput_US_BytesSent(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel US_BytesSent
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_US_BytesSent;

      //! The history of US_BytesSent values
      //!
      History<TlmEntry_US_BytesSent> 
        *tlmHistory_US_BytesSent;

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

      //! To port connected to Sched
      //!
      Svc::OutputSchedPort m_to_Sched[1];

      //! To port connected to PortsIn
      //!
      Fw::OutputSerializePort m_to_PortsIn[10];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

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
