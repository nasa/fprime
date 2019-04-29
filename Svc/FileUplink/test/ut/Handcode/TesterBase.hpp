// ======================================================================
// \title  FileUplink/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for FileUplink component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FileUplink_TESTER_BASE_HPP
#define FileUplink_TESTER_BASE_HPP

#include <Svc/FileUplink/FileUplinkComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class FileUplinkTesterBase
  //! \brief Auto-generated base class for FileUplink component test harness
  //!
  class FileUplinkTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object FileUplinkTesterBase
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

      //! Connect pingIn to to_pingIn[portNum]
      //!
      void connect_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputPingPort *const pingIn /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from timeCaller
      //!
      //! \return from_timeCaller[portNum]
      //!
      Fw::InputTimePort* get_from_timeCaller(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from bufferSendOut
      //!
      //! \return from_bufferSendOut[portNum]
      //!
      Fw::InputBufferSendPort* get_from_bufferSendOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from tlmOut
      //!
      //! \return from_tlmOut[portNum]
      //!
      Fw::InputTlmPort* get_from_tlmOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from eventOut
      //!
      //! \return from_eventOut[portNum]
      //!
      Fw::InputLogPort* get_from_eventOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from pingOut
      //!
      //! \return from_pingOut[portNum]
      //!
      Svc::InputPingPort* get_from_pingOut(
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

      //! Construct object FileUplinkTesterBase
      //!
      FileUplinkTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object FileUplinkTesterBase
      //!
      virtual ~FileUplinkTesterBase(void);

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

      //! Get the number of to_bufferSendIn ports
      //!
      //! \return The number of to_bufferSendIn ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferSendIn(void) const;

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of from_bufferSendOut ports
      //!
      //! \return The number of from_bufferSendOut ports
      //!
      NATIVE_INT_TYPE getNum_from_bufferSendOut(void) const;

      //! Get the number of from_tlmOut ports
      //!
      //! \return The number of from_tlmOut ports
      //!
      NATIVE_INT_TYPE getNum_from_tlmOut(void) const;

      //! Get the number of from_eventOut ports
      //!
      //! \return The number of from_eventOut ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOut(void) const;

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
      //! Whether to_bufferSendIn[portNum] is connected
      //!
      bool isConnected_to_bufferSendIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_pingIn[portNum] is connected
      //!
      bool isConnected_to_pingIn(
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
      // Event: FileUplink_BadChecksum
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_BadChecksum
      //!
      virtual void logIn_WARNING_HI_FileUplink_BadChecksum(
          Fw::LogStringArg& fileName, /*!< The file name*/
          U32 computed, /*!< The computed value*/
          U32 read /*!< The value read*/
      );

      //! A history entry for event FileUplink_BadChecksum
      //!
      typedef struct {
        Fw::LogStringArg fileName;
        U32 computed;
        U32 read;
      } EventEntry_FileUplink_BadChecksum;

      //! The history of FileUplink_BadChecksum events
      //!
      History<EventEntry_FileUplink_BadChecksum> 
        *eventHistory_FileUplink_BadChecksum;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileOpenError
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_FileOpenError
      //!
      virtual void logIn_WARNING_HI_FileUplink_FileOpenError(
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileUplink_FileOpenError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_FileUplink_FileOpenError;

      //! The history of FileUplink_FileOpenError events
      //!
      History<EventEntry_FileUplink_FileOpenError> 
        *eventHistory_FileUplink_FileOpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileReceived
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_FileReceived
      //!
      virtual void logIn_ACTIVITY_HI_FileUplink_FileReceived(
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileUplink_FileReceived
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_FileUplink_FileReceived;

      //! The history of FileUplink_FileReceived events
      //!
      History<EventEntry_FileUplink_FileReceived> 
        *eventHistory_FileUplink_FileReceived;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileWriteError
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_FileWriteError
      //!
      virtual void logIn_WARNING_HI_FileUplink_FileWriteError(
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileUplink_FileWriteError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_FileUplink_FileWriteError;

      //! The history of FileUplink_FileWriteError events
      //!
      History<EventEntry_FileUplink_FileWriteError> 
        *eventHistory_FileUplink_FileWriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_InvalidReceiveMode
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_InvalidReceiveMode
      //!
      virtual void logIn_WARNING_HI_FileUplink_InvalidReceiveMode(
          U32 packetType, /*!< The type of the packet received*/
          U32 mode /*!< The receive mode*/
      );

      //! A history entry for event FileUplink_InvalidReceiveMode
      //!
      typedef struct {
        U32 packetType;
        U32 mode;
      } EventEntry_FileUplink_InvalidReceiveMode;

      //! The history of FileUplink_InvalidReceiveMode events
      //!
      History<EventEntry_FileUplink_InvalidReceiveMode> 
        *eventHistory_FileUplink_InvalidReceiveMode;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_PacketOutOfBounds
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_PacketOutOfBounds
      //!
      virtual void logIn_WARNING_HI_FileUplink_PacketOutOfBounds(
          U32 packetIndex, /*!< The sequence index of the packet*/
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileUplink_PacketOutOfBounds
      //!
      typedef struct {
        U32 packetIndex;
        Fw::LogStringArg fileName;
      } EventEntry_FileUplink_PacketOutOfBounds;

      //! The history of FileUplink_PacketOutOfBounds events
      //!
      History<EventEntry_FileUplink_PacketOutOfBounds> 
        *eventHistory_FileUplink_PacketOutOfBounds;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_PacketOutOfOrder
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_PacketOutOfOrder
      //!
      virtual void logIn_WARNING_HI_FileUplink_PacketOutOfOrder(
          U32 packetIndex, /*!< The sequence index of the out-of-order packet*/
          U32 lastPacketIndex /*!< The sequence index of the last packet received before the out-of-order packet*/
      );

      //! A history entry for event FileUplink_PacketOutOfOrder
      //!
      typedef struct {
        U32 packetIndex;
        U32 lastPacketIndex;
      } EventEntry_FileUplink_PacketOutOfOrder;

      //! The history of FileUplink_PacketOutOfOrder events
      //!
      History<EventEntry_FileUplink_PacketOutOfOrder> 
        *eventHistory_FileUplink_PacketOutOfOrder;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_UplinkCanceled
      // ----------------------------------------------------------------------

      //! Handle event FileUplink_UplinkCanceled
      //!
      virtual void logIn_ACTIVITY_HI_FileUplink_UplinkCanceled(
          void
      );

      //! Size of history for event FileUplink_UplinkCanceled
      //!
      U32 eventsSize_FileUplink_UplinkCanceled;

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
      // Channel: FileUplink_FilesReceived
      // ----------------------------------------------------------------------

      //! Handle channel FileUplink_FilesReceived
      //!
      virtual void tlmInput_FileUplink_FilesReceived(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileUplink_FilesReceived
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileUplink_FilesReceived;

      //! The history of FileUplink_FilesReceived values
      //!
      History<TlmEntry_FileUplink_FilesReceived> 
        *tlmHistory_FileUplink_FilesReceived;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileUplink_PacketsReceived
      // ----------------------------------------------------------------------

      //! Handle channel FileUplink_PacketsReceived
      //!
      virtual void tlmInput_FileUplink_PacketsReceived(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileUplink_PacketsReceived
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileUplink_PacketsReceived;

      //! The history of FileUplink_PacketsReceived values
      //!
      History<TlmEntry_FileUplink_PacketsReceived> 
        *tlmHistory_FileUplink_PacketsReceived;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileUplink_Warnings
      // ----------------------------------------------------------------------

      //! Handle channel FileUplink_Warnings
      //!
      virtual void tlmInput_FileUplink_Warnings(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileUplink_Warnings
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileUplink_Warnings;

      //! The history of FileUplink_Warnings values
      //!
      History<TlmEntry_FileUplink_Warnings> 
        *tlmHistory_FileUplink_Warnings;

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

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to bufferSendOut
      //!
      Fw::InputBufferSendPort m_from_bufferSendOut[1];

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

      //! From port connected to eventOut
      //!
      Fw::InputLogPort m_from_eventOut[1];

      //! From port connected to pingOut
      //!
      Svc::InputPingPort m_from_pingOut[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to LogText
      //!
      Fw::InputLogTextPort m_from_LogText[1];
#endif

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_timeCaller
      //!
      static void from_timeCaller_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

      //! Static function for port from_bufferSendOut
      //!
      static void from_bufferSendOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
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

      //! Static function for port from_pingOut
      //!
      static void from_pingOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
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
