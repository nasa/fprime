// ======================================================================
// \title  UdpReceiver/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for UdpReceiver component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpReceiver_TESTER_BASE_HPP
#define UdpReceiver_TESTER_BASE_HPP

#include <Svc/UdpReceiver/UdpReceiverComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class UdpReceiverTesterBase
  //! \brief Auto-generated base class for UdpReceiver component test harness
  //!
  class UdpReceiverTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object UdpReceiverTesterBase
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

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from PortsOut
      //!
      //! \return from_PortsOut[portNum]
      //!
      Fw::InputSerializePort* get_from_PortsOut(
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

      //! Construct object UdpReceiverTesterBase
      //!
      UdpReceiverTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object UdpReceiverTesterBase
      //!
      virtual ~UdpReceiverTesterBase(void);

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
      // Handler prototypes for serial from ports
      // ----------------------------------------------------------------------

      //! Handler prototype for from_PortsOut
      //!
      virtual void from_PortsOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      ) = 0;

      //! Handler base class function prototype for from_PortsOut
      //!
      void from_PortsOut_handlerBase (
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );


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

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_Sched ports
      //!
      //! \return The number of to_Sched ports
      //!
      NATIVE_INT_TYPE getNum_to_Sched(void) const;

      //! Get the number of from_PortsOut ports
      //!
      //! \return The number of from_PortsOut ports
      //!
      NATIVE_INT_TYPE getNum_from_PortsOut(void) const;

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
      // Event: UR_PortOpened
      // ----------------------------------------------------------------------

      //! Handle event UR_PortOpened
      //!
      virtual void logIn_ACTIVITY_HI_UR_PortOpened(
          U32 port 
      );

      //! A history entry for event UR_PortOpened
      //!
      typedef struct {
        U32 port;
      } EventEntry_UR_PortOpened;

      //! The history of UR_PortOpened events
      //!
      History<EventEntry_UR_PortOpened> 
        *eventHistory_UR_PortOpened;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_SocketError
      // ----------------------------------------------------------------------

      //! Handle event UR_SocketError
      //!
      virtual void logIn_WARNING_HI_UR_SocketError(
          Fw::LogStringArg& error 
      );

      //! A history entry for event UR_SocketError
      //!
      typedef struct {
        Fw::LogStringArg error;
      } EventEntry_UR_SocketError;

      //! The history of UR_SocketError events
      //!
      History<EventEntry_UR_SocketError> 
        *eventHistory_UR_SocketError;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_BindError
      // ----------------------------------------------------------------------

      //! Handle event UR_BindError
      //!
      virtual void logIn_WARNING_HI_UR_BindError(
          Fw::LogStringArg& error 
      );

      //! A history entry for event UR_BindError
      //!
      typedef struct {
        Fw::LogStringArg error;
      } EventEntry_UR_BindError;

      //! The history of UR_BindError events
      //!
      History<EventEntry_UR_BindError> 
        *eventHistory_UR_BindError;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_RecvError
      // ----------------------------------------------------------------------

      //! Handle event UR_RecvError
      //!
      virtual void logIn_WARNING_HI_UR_RecvError(
          Fw::LogStringArg& error 
      );

      //! A history entry for event UR_RecvError
      //!
      typedef struct {
        Fw::LogStringArg error;
      } EventEntry_UR_RecvError;

      //! The history of UR_RecvError events
      //!
      History<EventEntry_UR_RecvError> 
        *eventHistory_UR_RecvError;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_DecodeError
      // ----------------------------------------------------------------------

      //! Handle event UR_DecodeError
      //!
      virtual void logIn_WARNING_HI_UR_DecodeError(
          UdpReceiverComponentBase::DecodeStage stage, 
          I32 error 
      );

      //! A history entry for event UR_DecodeError
      //!
      typedef struct {
        UdpReceiverComponentBase::DecodeStage stage;
        I32 error;
      } EventEntry_UR_DecodeError;

      //! The history of UR_DecodeError events
      //!
      History<EventEntry_UR_DecodeError> 
        *eventHistory_UR_DecodeError;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_DroppedPacket
      // ----------------------------------------------------------------------

      //! Handle event UR_DroppedPacket
      //!
      virtual void logIn_WARNING_HI_UR_DroppedPacket(
          U32 diff 
      );

      //! A history entry for event UR_DroppedPacket
      //!
      typedef struct {
        U32 diff;
      } EventEntry_UR_DroppedPacket;

      //! The history of UR_DroppedPacket events
      //!
      History<EventEntry_UR_DroppedPacket> 
        *eventHistory_UR_DroppedPacket;

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
      // Channel: UR_PacketsReceived
      // ----------------------------------------------------------------------

      //! Handle channel UR_PacketsReceived
      //!
      virtual void tlmInput_UR_PacketsReceived(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel UR_PacketsReceived
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_UR_PacketsReceived;

      //! The history of UR_PacketsReceived values
      //!
      History<TlmEntry_UR_PacketsReceived> 
        *tlmHistory_UR_PacketsReceived;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_BytesReceived
      // ----------------------------------------------------------------------

      //! Handle channel UR_BytesReceived
      //!
      virtual void tlmInput_UR_BytesReceived(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel UR_BytesReceived
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_UR_BytesReceived;

      //! The history of UR_BytesReceived values
      //!
      History<TlmEntry_UR_BytesReceived> 
        *tlmHistory_UR_BytesReceived;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_PacketsDropped
      // ----------------------------------------------------------------------

      //! Handle channel UR_PacketsDropped
      //!
      virtual void tlmInput_UR_PacketsDropped(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel UR_PacketsDropped
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_UR_PacketsDropped;

      //! The history of UR_PacketsDropped values
      //!
      History<TlmEntry_UR_PacketsDropped> 
        *tlmHistory_UR_PacketsDropped;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_DecodeErrors
      // ----------------------------------------------------------------------

      //! Handle channel UR_DecodeErrors
      //!
      virtual void tlmInput_UR_DecodeErrors(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel UR_DecodeErrors
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_UR_DecodeErrors;

      //! The history of UR_DecodeErrors values
      //!
      History<TlmEntry_UR_DecodeErrors> 
        *tlmHistory_UR_DecodeErrors;

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

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to PortsOut
      //!
      Fw::InputSerializePort m_from_PortsOut[10];

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

      //! Static function for port from_PortsOut
      //!
      static void from_PortsOut_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer //!< serialized data buffer
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
