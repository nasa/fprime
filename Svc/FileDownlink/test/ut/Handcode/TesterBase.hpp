// ======================================================================
// \title  FileDownlink/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for FileDownlink component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FileDownlink_TESTER_BASE_HPP
#define FileDownlink_TESTER_BASE_HPP

#include <Svc/FileDownlink/FileDownlinkComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class FileDownlinkTesterBase
  //! \brief Auto-generated base class for FileDownlink component test harness
  //!
  class FileDownlinkTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object FileDownlinkTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect cmdIn to to_cmdIn[portNum]
      //!
      void connect_to_cmdIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const cmdIn /*!< The port*/
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

      //! Get the port that receives input from bufferGetCaller
      //!
      //! \return from_bufferGetCaller[portNum]
      //!
      Fw::InputBufferGetPort* get_from_bufferGetCaller(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

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

      //! Get the port that receives input from cmdResponseOut
      //!
      //! \return from_cmdResponseOut[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_cmdResponseOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdRegOut
      //!
      //! \return from_cmdRegOut[portNum]
      //!
      Fw::InputCmdRegPort* get_from_cmdRegOut(
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

      //! Construct object FileDownlinkTesterBase
      //!
      FileDownlinkTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object FileDownlinkTesterBase
      //!
      virtual ~FileDownlinkTesterBase(void);

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

      //! Handler prototype for from_bufferGetCaller
      //!
      virtual Fw::Buffer from_bufferGetCaller_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      ) = 0;

      //! Handler base function for from_bufferGetCaller
      //!
      Fw::Buffer from_bufferGetCaller_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

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

      //! Push an entry on the history for from_bufferGetCaller
      void pushFromPortEntry_bufferGetCaller(
          U32 size 
      );

      //! A history entry for from_bufferGetCaller
      //!
      typedef struct {
        U32 size;
      } FromPortEntry_bufferGetCaller;

      //! The history for from_bufferGetCaller
      //!
      History<FromPortEntry_bufferGetCaller> 
        *fromPortHistory_bufferGetCaller;

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

      //! Get the number of from_bufferGetCaller ports
      //!
      //! \return The number of from_bufferGetCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_bufferGetCaller(void) const;

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of to_cmdIn ports
      //!
      //! \return The number of to_cmdIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdIn(void) const;

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

      //! Get the number of from_cmdResponseOut ports
      //!
      //! \return The number of from_cmdResponseOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdResponseOut(void) const;

      //! Get the number of from_cmdRegOut ports
      //!
      //! \return The number of from_cmdRegOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdRegOut(void) const;

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
      //! Whether to_cmdIn[portNum] is connected
      //!
      bool isConnected_to_cmdIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_pingIn[portNum] is connected
      //!
      bool isConnected_to_pingIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:
    
      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args); 

      //! Send a FileDownlink_SendFile command
      //!
      void sendCmd_FileDownlink_SendFile(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& sourceFileName, /*!< The name of the on-board file to send*/
          const Fw::CmdStringArg& destFileName /*!< The name of the destination file on the ground*/
      );

      //! Send a FileDownlink_Cancel command
      //!
      void sendCmd_FileDownlink_Cancel(
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
      // Event: FileDownlink_FileOpenError
      // ----------------------------------------------------------------------

      //! Handle event FileDownlink_FileOpenError
      //!
      virtual void logIn_WARNING_HI_FileDownlink_FileOpenError(
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileDownlink_FileOpenError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_FileDownlink_FileOpenError;

      //! The history of FileDownlink_FileOpenError events
      //!
      History<EventEntry_FileDownlink_FileOpenError> 
        *eventHistory_FileDownlink_FileOpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_FileReadError
      // ----------------------------------------------------------------------

      //! Handle event FileDownlink_FileReadError
      //!
      virtual void logIn_WARNING_HI_FileDownlink_FileReadError(
          Fw::LogStringArg& fileName /*!< The name of the file*/
      );

      //! A history entry for event FileDownlink_FileReadError
      //!
      typedef struct {
        Fw::LogStringArg fileName;
      } EventEntry_FileDownlink_FileReadError;

      //! The history of FileDownlink_FileReadError events
      //!
      History<EventEntry_FileDownlink_FileReadError> 
        *eventHistory_FileDownlink_FileReadError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_FileSent
      // ----------------------------------------------------------------------

      //! Handle event FileDownlink_FileSent
      //!
      virtual void logIn_ACTIVITY_HI_FileDownlink_FileSent(
          Fw::LogStringArg& sourceFileName, /*!< The source file name*/
          Fw::LogStringArg& destFileName /*!< The destination file name*/
      );

      //! A history entry for event FileDownlink_FileSent
      //!
      typedef struct {
        Fw::LogStringArg sourceFileName;
        Fw::LogStringArg destFileName;
      } EventEntry_FileDownlink_FileSent;

      //! The history of FileDownlink_FileSent events
      //!
      History<EventEntry_FileDownlink_FileSent> 
        *eventHistory_FileDownlink_FileSent;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_DownlinkCanceled
      // ----------------------------------------------------------------------

      //! Handle event FileDownlink_DownlinkCanceled
      //!
      virtual void logIn_ACTIVITY_HI_FileDownlink_DownlinkCanceled(
          Fw::LogStringArg& sourceFileName, /*!< The source file name*/
          Fw::LogStringArg& destFileName /*!< The destination file name*/
      );

      //! A history entry for event FileDownlink_DownlinkCanceled
      //!
      typedef struct {
        Fw::LogStringArg sourceFileName;
        Fw::LogStringArg destFileName;
      } EventEntry_FileDownlink_DownlinkCanceled;

      //! The history of FileDownlink_DownlinkCanceled events
      //!
      History<EventEntry_FileDownlink_DownlinkCanceled> 
        *eventHistory_FileDownlink_DownlinkCanceled;

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
      // Channel: FileDownlink_FilesSent
      // ----------------------------------------------------------------------

      //! Handle channel FileDownlink_FilesSent
      //!
      virtual void tlmInput_FileDownlink_FilesSent(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileDownlink_FilesSent
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileDownlink_FilesSent;

      //! The history of FileDownlink_FilesSent values
      //!
      History<TlmEntry_FileDownlink_FilesSent> 
        *tlmHistory_FileDownlink_FilesSent;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileDownlink_PacketsSent
      // ----------------------------------------------------------------------

      //! Handle channel FileDownlink_PacketsSent
      //!
      virtual void tlmInput_FileDownlink_PacketsSent(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileDownlink_PacketsSent
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileDownlink_PacketsSent;

      //! The history of FileDownlink_PacketsSent values
      //!
      History<TlmEntry_FileDownlink_PacketsSent> 
        *tlmHistory_FileDownlink_PacketsSent;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileDownlink_Warnings
      // ----------------------------------------------------------------------

      //! Handle channel FileDownlink_Warnings
      //!
      virtual void tlmInput_FileDownlink_Warnings(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel FileDownlink_Warnings
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_FileDownlink_Warnings;

      //! The history of FileDownlink_Warnings values
      //!
      History<TlmEntry_FileDownlink_Warnings> 
        *tlmHistory_FileDownlink_Warnings;

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

      //! To port connected to cmdIn
      //!
      Fw::OutputCmdPort m_to_cmdIn[1];

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to bufferGetCaller
      //!
      Fw::InputBufferGetPort m_from_bufferGetCaller[1];

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to bufferSendOut
      //!
      Fw::InputBufferSendPort m_from_bufferSendOut[1];

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

      //! From port connected to cmdResponseOut
      //!
      Fw::InputCmdResponsePort m_from_cmdResponseOut[1];

      //! From port connected to cmdRegOut
      //!
      Fw::InputCmdRegPort m_from_cmdRegOut[1];

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

      //! Static function for port from_bufferGetCaller
      //!
      static Fw::Buffer from_bufferGetCaller_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

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

      //! Static function for port from_cmdResponseOut
      //!
      static void from_cmdResponseOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Static function for port from_cmdRegOut
      //!
      static void from_cmdRegOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
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
