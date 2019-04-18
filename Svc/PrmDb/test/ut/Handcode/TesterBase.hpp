// ======================================================================
// \title  PrmDb/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for PrmDb component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef PrmDb_TESTER_BASE_HPP
#define PrmDb_TESTER_BASE_HPP

#include <Svc/PrmDb/PrmDbComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class PrmDbTesterBase
  //! \brief Auto-generated base class for PrmDb component test harness
  //!
  class PrmDbTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object PrmDbTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect getPrm to to_getPrm[portNum]
      //!
      void connect_to_getPrm(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputPrmGetPort *const getPrm /*!< The port*/
      );

      //! Connect setPrm to to_setPrm[portNum]
      //!
      void connect_to_setPrm(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputPrmSetPort *const setPrm /*!< The port*/
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

      //! Construct object PrmDbTesterBase
      //!
      PrmDbTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object PrmDbTesterBase
      //!
      virtual ~PrmDbTesterBase(void);

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

      //! Invoke the to port connected to getPrm
      //!
      Fw::ParamValid invoke_to_getPrm(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

      //! Invoke the to port connected to setPrm
      //!
      void invoke_to_setPrm(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_getPrm ports
      //!
      //! \return The number of to_getPrm ports
      //!
      NATIVE_INT_TYPE getNum_to_getPrm(void) const;

      //! Get the number of to_setPrm ports
      //!
      //! \return The number of to_setPrm ports
      //!
      NATIVE_INT_TYPE getNum_to_setPrm(void) const;

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
      //! Whether to_getPrm[portNum] is connected
      //!
      bool isConnected_to_getPrm(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_setPrm[portNum] is connected
      //!
      bool isConnected_to_setPrm(
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

      //! Send a PRM_SAVE_FILE command
      //!
      void sendCmd_PRM_SAVE_FILE(
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
      // Event: PrmIdNotFound
      // ----------------------------------------------------------------------

      //! Handle event PrmIdNotFound
      //!
      virtual void logIn_WARNING_LO_PrmIdNotFound(
          U32 Id /*!< The parameter ID*/
      );

      //! A history entry for event PrmIdNotFound
      //!
      typedef struct {
        U32 Id;
      } EventEntry_PrmIdNotFound;

      //! The history of PrmIdNotFound events
      //!
      History<EventEntry_PrmIdNotFound> 
        *eventHistory_PrmIdNotFound;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmIdUpdated
      // ----------------------------------------------------------------------

      //! Handle event PrmIdUpdated
      //!
      virtual void logIn_ACTIVITY_HI_PrmIdUpdated(
          U32 Id /*!< The parameter ID*/
      );

      //! A history entry for event PrmIdUpdated
      //!
      typedef struct {
        U32 Id;
      } EventEntry_PrmIdUpdated;

      //! The history of PrmIdUpdated events
      //!
      History<EventEntry_PrmIdUpdated> 
        *eventHistory_PrmIdUpdated;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmDbFull
      // ----------------------------------------------------------------------

      //! Handle event PrmDbFull
      //!
      virtual void logIn_FATAL_PrmDbFull(
          U32 Id /*!< The parameter ID*/
      );

      //! A history entry for event PrmDbFull
      //!
      typedef struct {
        U32 Id;
      } EventEntry_PrmDbFull;

      //! The history of PrmDbFull events
      //!
      History<EventEntry_PrmDbFull> 
        *eventHistory_PrmDbFull;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmIdAdded
      // ----------------------------------------------------------------------

      //! Handle event PrmIdAdded
      //!
      virtual void logIn_ACTIVITY_HI_PrmIdAdded(
          U32 Id /*!< The parameter ID*/
      );

      //! A history entry for event PrmIdAdded
      //!
      typedef struct {
        U32 Id;
      } EventEntry_PrmIdAdded;

      //! The history of PrmIdAdded events
      //!
      History<EventEntry_PrmIdAdded> 
        *eventHistory_PrmIdAdded;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmFileWriteError
      // ----------------------------------------------------------------------

      //! Handle event PrmFileWriteError
      //!
      virtual void logIn_WARNING_HI_PrmFileWriteError(
          PrmDbComponentBase::PrmWriteError stage, /*!< The write stage*/
          I32 record, /*!< The record that had the failure*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event PrmFileWriteError
      //!
      typedef struct {
        PrmDbComponentBase::PrmWriteError stage;
        I32 record;
        I32 error;
      } EventEntry_PrmFileWriteError;

      //! The history of PrmFileWriteError events
      //!
      History<EventEntry_PrmFileWriteError> 
        *eventHistory_PrmFileWriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmFileSaveComplete
      // ----------------------------------------------------------------------

      //! Handle event PrmFileSaveComplete
      //!
      virtual void logIn_ACTIVITY_HI_PrmFileSaveComplete(
          U32 records /*!< The number of records saved*/
      );

      //! A history entry for event PrmFileSaveComplete
      //!
      typedef struct {
        U32 records;
      } EventEntry_PrmFileSaveComplete;

      //! The history of PrmFileSaveComplete events
      //!
      History<EventEntry_PrmFileSaveComplete> 
        *eventHistory_PrmFileSaveComplete;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmFileReadError
      // ----------------------------------------------------------------------

      //! Handle event PrmFileReadError
      //!
      virtual void logIn_WARNING_HI_PrmFileReadError(
          PrmDbComponentBase::PrmReadError stage, /*!< The write stage*/
          I32 record, /*!< The record that had the failure*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event PrmFileReadError
      //!
      typedef struct {
        PrmDbComponentBase::PrmReadError stage;
        I32 record;
        I32 error;
      } EventEntry_PrmFileReadError;

      //! The history of PrmFileReadError events
      //!
      History<EventEntry_PrmFileReadError> 
        *eventHistory_PrmFileReadError;

    protected:

      // ----------------------------------------------------------------------
      // Event: PrmFileLoadComplete
      // ----------------------------------------------------------------------

      //! Handle event PrmFileLoadComplete
      //!
      virtual void logIn_ACTIVITY_HI_PrmFileLoadComplete(
          U32 records /*!< The number of records loaded*/
      );

      //! A history entry for event PrmFileLoadComplete
      //!
      typedef struct {
        U32 records;
      } EventEntry_PrmFileLoadComplete;

      //! The history of PrmFileLoadComplete events
      //!
      History<EventEntry_PrmFileLoadComplete> 
        *eventHistory_PrmFileLoadComplete;

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

      //! To port connected to getPrm
      //!
      Fw::OutputPrmGetPort m_to_getPrm[1];

      //! To port connected to setPrm
      //!
      Fw::OutputPrmSetPort m_to_setPrm[1];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

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
