// ======================================================================
// \title  LinuxSerialDriver/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxSerialDriver component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxSerialDriver_TESTER_BASE_HPP
#define LinuxSerialDriver_TESTER_BASE_HPP

#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Drv {

  //! \class LinuxSerialDriverTesterBase
  //! \brief Auto-generated base class for LinuxSerialDriver component test harness
  //!
  class LinuxSerialDriverTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object LinuxSerialDriverTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect readBufferSend to to_readBufferSend[portNum]
      //!
      void connect_to_readBufferSend(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputBufferSendPort *const readBufferSend /*!< The port*/
      );

      //! Connect serialSend to to_serialSend[portNum]
      //!
      void connect_to_serialSend(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Drv::InputSerialWritePort *const serialSend /*!< The port*/
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

      //! Get the port that receives input from serialRecv
      //!
      //! \return from_serialRecv[portNum]
      //!
      Drv::InputSerialReadPort* get_from_serialRecv(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from Time
      //!
      //! \return from_Time[portNum]
      //!
      Fw::InputTimePort* get_from_Time(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxSerialDriverTesterBase
      //!
      LinuxSerialDriverTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxSerialDriverTesterBase
      //!
      virtual ~LinuxSerialDriverTesterBase(void);

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

      //! Handler prototype for from_serialRecv
      //!
      virtual void from_serialRecv_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          SerialReadStatus &status /*!< Status of read*/
      ) = 0;

      //! Handler base function for from_serialRecv
      //!
      void from_serialRecv_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          SerialReadStatus &status /*!< Status of read*/
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

      //! Push an entry on the history for from_serialRecv
      void pushFromPortEntry_serialRecv(
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          SerialReadStatus &status /*!< Status of read*/
      );

      //! A history entry for from_serialRecv
      //!
      typedef struct {
        Fw::Buffer serBuffer;
        SerialReadStatus status;
      } FromPortEntry_serialRecv;

      //! The history for from_serialRecv
      //!
      History<FromPortEntry_serialRecv> 
        *fromPortHistory_serialRecv;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to readBufferSend
      //!
      void invoke_to_readBufferSend(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Invoke the to port connected to serialSend
      //!
      void invoke_to_serialSend(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer 
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_Tlm ports
      //!
      //! \return The number of from_Tlm ports
      //!
      NATIVE_INT_TYPE getNum_from_Tlm(void) const;

      //! Get the number of from_Log ports
      //!
      //! \return The number of from_Log ports
      //!
      NATIVE_INT_TYPE getNum_from_Log(void) const;

      //! Get the number of to_readBufferSend ports
      //!
      //! \return The number of to_readBufferSend ports
      //!
      NATIVE_INT_TYPE getNum_to_readBufferSend(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_LogText ports
      //!
      //! \return The number of from_LogText ports
      //!
      NATIVE_INT_TYPE getNum_from_LogText(void) const;
#endif

      //! Get the number of from_serialRecv ports
      //!
      //! \return The number of from_serialRecv ports
      //!
      NATIVE_INT_TYPE getNum_from_serialRecv(void) const;

      //! Get the number of from_Time ports
      //!
      //! \return The number of from_Time ports
      //!
      NATIVE_INT_TYPE getNum_from_Time(void) const;

      //! Get the number of to_serialSend ports
      //!
      //! \return The number of to_serialSend ports
      //!
      NATIVE_INT_TYPE getNum_to_serialSend(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_readBufferSend[portNum] is connected
      //!
      bool isConnected_to_readBufferSend(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_serialSend[portNum] is connected
      //!
      bool isConnected_to_serialSend(
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
      // Event: DR_OpenError
      // ----------------------------------------------------------------------

      //! Handle event DR_OpenError
      //!
      virtual void logIn_WARNING_HI_DR_OpenError(
          I32 device, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event DR_OpenError
      //!
      typedef struct {
        I32 device;
        I32 error;
      } EventEntry_DR_OpenError;

      //! The history of DR_OpenError events
      //!
      History<EventEntry_DR_OpenError> 
        *eventHistory_DR_OpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_ConfigError
      // ----------------------------------------------------------------------

      //! Handle event DR_ConfigError
      //!
      virtual void logIn_WARNING_HI_DR_ConfigError(
          I32 device, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event DR_ConfigError
      //!
      typedef struct {
        I32 device;
        I32 error;
      } EventEntry_DR_ConfigError;

      //! The history of DR_ConfigError events
      //!
      History<EventEntry_DR_ConfigError> 
        *eventHistory_DR_ConfigError;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_WriteError
      // ----------------------------------------------------------------------

      //! Handle event DR_WriteError
      //!
      virtual void logIn_WARNING_HI_DR_WriteError(
          I32 device, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event DR_WriteError
      //!
      typedef struct {
        I32 device;
        I32 error;
      } EventEntry_DR_WriteError;

      //! The history of DR_WriteError events
      //!
      History<EventEntry_DR_WriteError> 
        *eventHistory_DR_WriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_ReadError
      // ----------------------------------------------------------------------

      //! Handle event DR_ReadError
      //!
      virtual void logIn_WARNING_HI_DR_ReadError(
          I32 device, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event DR_ReadError
      //!
      typedef struct {
        I32 device;
        I32 error;
      } EventEntry_DR_ReadError;

      //! The history of DR_ReadError events
      //!
      History<EventEntry_DR_ReadError> 
        *eventHistory_DR_ReadError;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_PortOpened
      // ----------------------------------------------------------------------

      //! Handle event DR_PortOpened
      //!
      virtual void logIn_ACTIVITY_HI_DR_PortOpened(
          I32 device /*!< The device*/
      );

      //! A history entry for event DR_PortOpened
      //!
      typedef struct {
        I32 device;
      } EventEntry_DR_PortOpened;

      //! The history of DR_PortOpened events
      //!
      History<EventEntry_DR_PortOpened> 
        *eventHistory_DR_PortOpened;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_NoBuffers
      // ----------------------------------------------------------------------

      //! Handle event DR_NoBuffers
      //!
      virtual void logIn_WARNING_HI_DR_NoBuffers(
          I32 device /*!< The device*/
      );

      //! A history entry for event DR_NoBuffers
      //!
      typedef struct {
        I32 device;
      } EventEntry_DR_NoBuffers;

      //! The history of DR_NoBuffers events
      //!
      History<EventEntry_DR_NoBuffers> 
        *eventHistory_DR_NoBuffers;

    protected:

      // ----------------------------------------------------------------------
      // Event: DR_BufferTooSmall
      // ----------------------------------------------------------------------

      //! Handle event DR_BufferTooSmall
      //!
      virtual void logIn_WARNING_HI_DR_BufferTooSmall(
          I32 device, /*!< The device*/
          U32 size, /*!< The provided buffer size*/
          U32 needed /*!< The buffer size needed*/
      );

      //! A history entry for event DR_BufferTooSmall
      //!
      typedef struct {
        I32 device;
        U32 size;
        U32 needed;
      } EventEntry_DR_BufferTooSmall;

      //! The history of DR_BufferTooSmall events
      //!
      History<EventEntry_DR_BufferTooSmall> 
        *eventHistory_DR_BufferTooSmall;

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
      // Channel: DR_BytesSent
      // ----------------------------------------------------------------------

      //! Handle channel DR_BytesSent
      //!
      virtual void tlmInput_DR_BytesSent(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel DR_BytesSent
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_DR_BytesSent;

      //! The history of DR_BytesSent values
      //!
      History<TlmEntry_DR_BytesSent> 
        *tlmHistory_DR_BytesSent;

    protected:

      // ----------------------------------------------------------------------
      // Channel: DR_BytesRecv
      // ----------------------------------------------------------------------

      //! Handle channel DR_BytesRecv
      //!
      virtual void tlmInput_DR_BytesRecv(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel DR_BytesRecv
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_DR_BytesRecv;

      //! The history of DR_BytesRecv values
      //!
      History<TlmEntry_DR_BytesRecv> 
        *tlmHistory_DR_BytesRecv;

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

      //! To port connected to readBufferSend
      //!
      Fw::OutputBufferSendPort m_to_readBufferSend[1];

      //! To port connected to serialSend
      //!
      Drv::OutputSerialWritePort m_to_serialSend[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to Tlm
      //!
      Fw::InputTlmPort m_from_Tlm[1];

      //! From port connected to Log
      //!
      Fw::InputLogPort m_from_Log[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to LogText
      //!
      Fw::InputLogTextPort m_from_LogText[1];
#endif

      //! From port connected to serialRecv
      //!
      Drv::InputSerialReadPort m_from_serialRecv[1];

      //! From port connected to Time
      //!
      Fw::InputTimePort m_from_Time[1];

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

      //! Static function for port from_serialRecv
      //!
      static void from_serialRecv_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          SerialReadStatus &status /*!< Status of read*/
      );

      //! Static function for port from_Time
      //!
      static void from_Time_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

    private:

      // ----------------------------------------------------------------------
      // Test time
      // ----------------------------------------------------------------------

      //! Test time stamp
      //!
      Fw::Time m_testTime;

  };

} // end namespace Drv

#endif
