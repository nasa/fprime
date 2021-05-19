// ======================================================================
// \title  Example/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for Example component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_TESTER_BASE_HPP
#define Example_TESTER_BASE_HPP

#include <Autocoders/Python/templates/ExampleComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace ExampleComponents {

  //! \class ExampleTesterBase
  //! \brief Auto-generated base class for Example component test harness
  //!
  class ExampleTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ExampleTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect exampleInput to to_exampleInput[portNum]
      //!
      void connect_to_exampleInput(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          AnotherExample::InputExamplePort *const exampleInput /*!< The port*/
      );

      //! Connect anotherInput to to_anotherInput[portNum]
      //!
      void connect_to_anotherInput(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          SomeOtherNamespace::InputAnotherPort *const anotherInput /*!< The port*/
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

      //! Get the port that receives input from exampleOutput
      //!
      //! \return from_exampleOutput[portNum]
      //!
      AnotherExample::InputExamplePort* get_from_exampleOutput(
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

      //! Get the port that receives input from ParamGet
      //!
      //! \return from_ParamGet[portNum]
      //!
      Fw::InputPrmGetPort* get_from_ParamGet(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from ParamSet
      //!
      //! \return from_ParamSet[portNum]
      //!
      Fw::InputPrmSetPort* get_from_ParamSet(
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

      //! Construct object ExampleTesterBase
      //!
      ExampleTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ExampleTesterBase
      //!
      virtual ~ExampleTesterBase(void);

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

      //! Handler prototype for from_exampleOutput
      //!
      virtual void from_exampleOutput_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg1, /*!< A built-in type argument*/
          ANameSpace::mytype arg2, /*!< A user-defined type argument*/
          U8 arg3, /*!< The third argument*/
          Example3::ExampleSerializable arg4, /*!< The third argument*/
          AnotherExample::SomeEnum arg5 /*!< The ENUM argument*/
      ) = 0;

      //! Handler base function for from_exampleOutput
      //!
      void from_exampleOutput_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg1, /*!< A built-in type argument*/
          ANameSpace::mytype arg2, /*!< A user-defined type argument*/
          U8 arg3, /*!< The third argument*/
          Example3::ExampleSerializable arg4, /*!< The third argument*/
          AnotherExample::SomeEnum arg5 /*!< The ENUM argument*/
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

      //! Push an entry on the history for from_exampleOutput
      void pushFromPortEntry_exampleOutput(
          I32 arg1, /*!< A built-in type argument*/
          ANameSpace::mytype arg2, /*!< A user-defined type argument*/
          U8 arg3, /*!< The third argument*/
          Example3::ExampleSerializable arg4, /*!< The third argument*/
          AnotherExample::SomeEnum arg5 /*!< The ENUM argument*/
      );

      //! A history entry for from_exampleOutput
      //!
      typedef struct {
        I32 arg1;
        ANameSpace::mytype arg2;
        U8 arg3;
        Example3::ExampleSerializable arg4;
        AnotherExample::SomeEnum arg5;
      } FromPortEntry_exampleOutput;

      //! The history for from_exampleOutput
      //!
      History<FromPortEntry_exampleOutput>
        *fromPortHistory_exampleOutput;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to exampleInput
      //!
      void invoke_to_exampleInput(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg1, /*!< A built-in type argument*/
          ANameSpace::mytype arg2, /*!< A user-defined type argument*/
          U8 arg3, /*!< The third argument*/
          Example3::ExampleSerializable arg4, /*!< The third argument*/
          AnotherExample::SomeEnum arg5 /*!< The ENUM argument*/
      );

      //! Invoke the to port connected to anotherInput
      //!
      SomeOtherNamespace::AnotherEnum invoke_to_anotherInput(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg1, /*!< A built-in type argument*/
          F64 arg2, /*!< A float argument*/
          SomeOtherNamespace::SomeEnum arg3 /*!< The ENUM argument*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_exampleInput ports
      //!
      //! \return The number of to_exampleInput ports
      //!
      NATIVE_INT_TYPE getNum_to_exampleInput(void) const;

      //! Get the number of to_anotherInput ports
      //!
      //! \return The number of to_anotherInput ports
      //!
      NATIVE_INT_TYPE getNum_to_anotherInput(void) const;

      //! Get the number of from_exampleOutput ports
      //!
      //! \return The number of from_exampleOutput ports
      //!
      NATIVE_INT_TYPE getNum_from_exampleOutput(void) const;

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

      //! Get the number of from_ParamGet ports
      //!
      //! \return The number of from_ParamGet ports
      //!
      NATIVE_INT_TYPE getNum_from_ParamGet(void) const;

      //! Get the number of from_ParamSet ports
      //!
      //! \return The number of from_ParamSet ports
      //!
      NATIVE_INT_TYPE getNum_from_ParamSet(void) const;

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
      //! Whether to_exampleInput[portNum] is connected
      //!
      bool isConnected_to_exampleInput(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_anotherInput[portNum] is connected
      //!
      bool isConnected_to_anotherInput(
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

      //! Send a TEST_CMD_1 command
      //!
      void sendCmd_TEST_CMD_1(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          I32 arg1, /*!< The I32 command argument*/
          ExampleComponentBase::CmdEnum arg2, /*!< The ENUM argument*/
          const Fw::CmdStringArg& arg3 /*!< The string argument*/
      );

      //! Send a TEST_CMD_2 command
      //!
      void sendCmd_TEST_CMD_2(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          I32 arg1, /*!< The I32 command argument*/
          F32 arg2 /*!< A float argument*/
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
      // Event: SomeEvent
      // ----------------------------------------------------------------------

      //! Handle event SomeEvent
      //!
      virtual void logIn_ACTIVITY_HI_SomeEvent(
          I32 arg1, /*!< The I32 event argument*/
          F32 arg2, /*!< The F32 event argument*/
          Example4::Example2 arg3, /*!< The Example4 event argument*/
          Fw::LogStringArg& arg4, /*!< The string event argument*/
          ExampleComponentBase::SomeEventEnum arg5 /*!< The enum event argument*/
      );

      //! A history entry for event SomeEvent
      //!
      typedef struct {
        I32 arg1;
        F32 arg2;
        Example4::Example2 arg3;
        Fw::LogStringArg arg4;
        ExampleComponentBase::SomeEventEnum arg5;
      } EventEntry_SomeEvent;

      //! The history of SomeEvent events
      //!
      History<EventEntry_SomeEvent>
        *eventHistory_SomeEvent;

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
      // Channel: somechan
      // ----------------------------------------------------------------------

      //! Handle channel somechan
      //!
      virtual void tlmInput_somechan(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel somechan
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_somechan;

      //! The history of somechan values
      //!
      History<TlmEntry_somechan>
        *tlmHistory_somechan;

    protected:

      // ----------------------------------------------------------------------
      // Channel: anotherchan
      // ----------------------------------------------------------------------

      //! Handle channel anotherchan
      //!
      virtual void tlmInput_anotherchan(
          const Fw::Time& timeTag, /*!< The time*/
          const Example4::Example2& val /*!< The channel value*/
      );

      //! A telemetry entry for channel anotherchan
      //!
      typedef struct {
        Fw::Time timeTag;
        Example4::Example2 arg;
      } TlmEntry_anotherchan;

      //! The history of anotherchan values
      //!
      History<TlmEntry_anotherchan>
        *tlmHistory_anotherchan;

    protected:

      // ----------------------------------------------------------------------
      // Channel: stringchan
      // ----------------------------------------------------------------------

      //! Handle channel stringchan
      //!
      virtual void tlmInput_stringchan(
          const Fw::Time& timeTag, /*!< The time*/
          const Fw::TlmString& val /*!< The channel value*/
      );

      //! A telemetry entry for channel stringchan
      //!
      typedef struct {
        Fw::Time timeTag;
        Fw::TlmString arg;
      } TlmEntry_stringchan;

      //! The history of stringchan values
      //!
      History<TlmEntry_stringchan>
        *tlmHistory_stringchan;

    protected:

      // ----------------------------------------------------------------------
      // Channel: enumchan
      // ----------------------------------------------------------------------

      //! Handle channel enumchan
      //!
      virtual void tlmInput_enumchan(
          const Fw::Time& timeTag, /*!< The time*/
          const ExampleComponentBase::SomeTlmEnum& val /*!< The channel value*/
      );

      //! A telemetry entry for channel enumchan
      //!
      typedef struct {
        Fw::Time timeTag;
        ExampleComponentBase::SomeTlmEnum arg;
      } TlmEntry_enumchan;

      //! The history of enumchan values
      //!
      History<TlmEntry_enumchan>
        *tlmHistory_enumchan;

    protected:

      // ----------------------------------------------------------------------
      // Test time
      // ----------------------------------------------------------------------

      //! Set the test time for events and telemetry
      //!
      void setTestTime(
          const Fw::Time& timeTag /*!< The time*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Parameter: someparam
      // ----------------------------------------------------------------------

      void paramSet_someparam(
          const U32& val, /*!< The parameter value*/
          Fw::ParamValid valid /*!< The parameter valid flag*/
      );

      void paramSend_someparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void paramSave_someparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Parameter: anotherparam
      // ----------------------------------------------------------------------

      void paramSet_anotherparam(
          const Example4::Example2& val, /*!< The parameter value*/
          Fw::ParamValid valid /*!< The parameter valid flag*/
      );

      void paramSend_anotherparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void paramSave_anotherparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Parameter: stringparam
      // ----------------------------------------------------------------------

      void paramSet_stringparam(
          const Fw::ParamString& val, /*!< The parameter value*/
          Fw::ParamValid valid /*!< The parameter valid flag*/
      );

      void paramSend_stringparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void paramSave_stringparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Parameter: enumparam
      // ----------------------------------------------------------------------

      void paramSet_enumparam(
          const ExampleComponentBase::SomeParamEnum& val, /*!< The parameter value*/
          Fw::ParamValid valid /*!< The parameter valid flag*/
      );

      void paramSend_enumparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void paramSave_enumparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to exampleInput
      //!
      AnotherExample::OutputExamplePort m_to_exampleInput[6];

      //! To port connected to anotherInput
      //!
      SomeOtherNamespace::OutputAnotherPort m_to_anotherInput[2];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to exampleOutput
      //!
      AnotherExample::InputExamplePort m_from_exampleOutput[4];

      //! From port connected to CmdStatus
      //!
      Fw::InputCmdResponsePort m_from_CmdStatus[1];

      //! From port connected to CmdReg
      //!
      Fw::InputCmdRegPort m_from_CmdReg[1];

      //! From port connected to ParamGet
      //!
      Fw::InputPrmGetPort m_from_ParamGet[1];

      //! From port connected to ParamSet
      //!
      Fw::InputPrmSetPort m_from_ParamSet[1];

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

      //! Static function for port from_exampleOutput
      //!
      static void from_exampleOutput_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg1, /*!< A built-in type argument*/
          ANameSpace::mytype arg2, /*!< A user-defined type argument*/
          U8 arg3, /*!< The third argument*/
          Example3::ExampleSerializable arg4, /*!< The third argument*/
          AnotherExample::SomeEnum arg5 /*!< The ENUM argument*/
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

      //! Static function for port from_ParamGet
      //!
      static Fw::ParamValid from_ParamGet_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

      //! Static function for port from_ParamSet
      //!
      static void from_ParamSet_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
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

    private:

      // ----------------------------------------------------------------------
      // Parameter validity flags
      // ----------------------------------------------------------------------

      //! True if parameter someparam was successfully received
      //!
      Fw::ParamValid m_param_someparam_valid;

      //! True if parameter anotherparam was successfully received
      //!
      Fw::ParamValid m_param_anotherparam_valid;

      //! True if parameter stringparam was successfully received
      //!
      Fw::ParamValid m_param_stringparam_valid;

      //! True if parameter enumparam was successfully received
      //!
      Fw::ParamValid m_param_enumparam_valid;

    private:

      // ----------------------------------------------------------------------
      // Parameter variables
      // ----------------------------------------------------------------------

      //! Parameter someparam
      //!
      U32 m_param_someparam;

      //! Parameter anotherparam
      //!
      Example4::Example2 m_param_anotherparam;

      //! Parameter stringparam
      //!
      Fw::ParamString m_param_stringparam;

      //! Parameter enumparam
      //!
      ExampleComponentBase::SomeParamEnum m_param_enumparam;

  };

} // end namespace ExampleComponents

#endif
