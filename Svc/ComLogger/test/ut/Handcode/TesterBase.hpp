// ======================================================================
// \title  ComLogger/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for ComLogger component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ComLogger_TESTER_BASE_HPP
#define ComLogger_TESTER_BASE_HPP

#include <Svc/ComLogger/ComLoggerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class ComLoggerTesterBase
  //! \brief Auto-generated base class for ComLogger component test harness
  //!
  class ComLoggerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ComLoggerTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect comIn to to_comIn[portNum]
      //!
      void connect_to_comIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputComPort *const comIn /*!< The port*/
      );

      //! Connect cmdIn to to_cmdIn[portNum]
      //!
      void connect_to_cmdIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const cmdIn /*!< The port*/
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

      //! Get the port that receives input from cmdRegOut
      //!
      //! \return from_cmdRegOut[portNum]
      //!
      Fw::InputCmdRegPort* get_from_cmdRegOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from logOut
      //!
      //! \return from_logOut[portNum]
      //!
      Fw::InputLogPort* get_from_logOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdResponseOut
      //!
      //! \return from_cmdResponseOut[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_cmdResponseOut(
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

      //! Construct object ComLoggerTesterBase
      //!
      ComLoggerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ComLoggerTesterBase
      //!
      virtual ~ComLoggerTesterBase(void);

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

      //! Invoke the to port connected to comIn
      //!
      void invoke_to_comIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_comIn ports
      //!
      //! \return The number of to_comIn ports
      //!
      NATIVE_INT_TYPE getNum_to_comIn(void) const;

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of from_cmdRegOut ports
      //!
      //! \return The number of from_cmdRegOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdRegOut(void) const;

      //! Get the number of to_cmdIn ports
      //!
      //! \return The number of to_cmdIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdIn(void) const;

      //! Get the number of from_logOut ports
      //!
      //! \return The number of from_logOut ports
      //!
      NATIVE_INT_TYPE getNum_from_logOut(void) const;

      //! Get the number of from_cmdResponseOut ports
      //!
      //! \return The number of from_cmdResponseOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdResponseOut(void) const;

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
      //! Whether to_comIn[portNum] is connected
      //!
      bool isConnected_to_comIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_cmdIn[portNum] is connected
      //!
      bool isConnected_to_cmdIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:
    
      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args); 

      //! Send a CloseFile command
      //!
      void sendCmd_CloseFile(
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
      // Event: FileOpenError
      // ----------------------------------------------------------------------

      //! Handle event FileOpenError
      //!
      virtual void logIn_WARNING_HI_FileOpenError(
          U32 errornum, /*!< The error number returned from open file*/
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event FileOpenError
      //!
      typedef struct {
        U32 errornum;
        Fw::LogStringArg file;
      } EventEntry_FileOpenError;

      //! The history of FileOpenError events
      //!
      History<EventEntry_FileOpenError> 
        *eventHistory_FileOpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileWriteError
      // ----------------------------------------------------------------------

      //! Handle event FileWriteError
      //!
      virtual void logIn_WARNING_HI_FileWriteError(
          U32 errornum, /*!< The error number returned from write file*/
          U32 bytesWritten, /*!< The number of bytes successfully written to file*/
          U32 bytesToWrite, /*!< The number of bytes attempted to write to file*/
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event FileWriteError
      //!
      typedef struct {
        U32 errornum;
        U32 bytesWritten;
        U32 bytesToWrite;
        Fw::LogStringArg file;
      } EventEntry_FileWriteError;

      //! The history of FileWriteError events
      //!
      History<EventEntry_FileWriteError> 
        *eventHistory_FileWriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileValidationError
      // ----------------------------------------------------------------------

      //! Handle event FileValidationError
      //!
      virtual void logIn_WARNING_LO_FileValidationError(
          Fw::LogStringArg& validationFile, /*!< The validation file*/
          Fw::LogStringArg& file, /*!< The file*/
          U32 status /*!< The Os::Validate::Status return*/
      );

      //! A history entry for event FileValidationError
      //!
      typedef struct {
        Fw::LogStringArg validationFile;
        Fw::LogStringArg file;
        U32 status;
      } EventEntry_FileValidationError;

      //! The history of FileValidationError events
      //!
      History<EventEntry_FileValidationError> 
        *eventHistory_FileValidationError;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileClosed
      // ----------------------------------------------------------------------

      //! Handle event FileClosed
      //!
      virtual void logIn_DIAGNOSTIC_FileClosed(
          Fw::LogStringArg& file /*!< The file*/
      );

      //! A history entry for event FileClosed
      //!
      typedef struct {
        Fw::LogStringArg file;
      } EventEntry_FileClosed;

      //! The history of FileClosed events
      //!
      History<EventEntry_FileClosed> 
        *eventHistory_FileClosed;

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

      //! To port connected to comIn
      //!
      Fw::OutputComPort m_to_comIn[1];

      //! To port connected to cmdIn
      //!
      Fw::OutputCmdPort m_to_cmdIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to cmdRegOut
      //!
      Fw::InputCmdRegPort m_from_cmdRegOut[1];

      //! From port connected to logOut
      //!
      Fw::InputLogPort m_from_logOut[1];

      //! From port connected to cmdResponseOut
      //!
      Fw::InputCmdResponsePort m_from_cmdResponseOut[1];

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

      //! Static function for port from_cmdRegOut
      //!
      static void from_cmdRegOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
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

      //! Static function for port from_cmdResponseOut
      //!
      static void from_cmdResponseOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
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
