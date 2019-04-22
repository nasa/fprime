// ======================================================================
// \title  BufferAccumulator/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferAccumulator component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferAccumulator_TESTER_BASE_HPP
#define BufferAccumulator_TESTER_BASE_HPP

#include <Svc/BufferAccumulator/BufferAccumulatorComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class BufferAccumulatorTesterBase
  //! \brief Auto-generated base class for BufferAccumulator component test harness
  //!
  class BufferAccumulatorTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object BufferAccumulatorTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect bufferSendInFill to to_bufferSendInFill[portNum]
      //!
      void connect_to_bufferSendInFill(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputBufferSendPort *const bufferSendInFill /*!< The port*/
      );

      //! Connect bufferSendInReturn to to_bufferSendInReturn[portNum]
      //!
      void connect_to_bufferSendInReturn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputBufferSendPort *const bufferSendInReturn /*!< The port*/
      );

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

      //! Connect schedIn to to_schedIn[portNum]
      //!
      void connect_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::InputSchedPort *const schedIn /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from bufferSendOutDrain
      //!
      //! \return from_bufferSendOutDrain[portNum]
      //!
      Fw::InputBufferSendPort* get_from_bufferSendOutDrain(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from bufferSendOutReturn
      //!
      //! \return from_bufferSendOutReturn[portNum]
      //!
      Fw::InputBufferSendPort* get_from_bufferSendOutReturn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdRegOut
      //!
      //! \return from_cmdRegOut[portNum]
      //!
      Fw::InputCmdRegPort* get_from_cmdRegOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from cmdResponseOut
      //!
      //! \return from_cmdResponseOut[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_cmdResponseOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from eventOut
      //!
      //! \return from_eventOut[portNum]
      //!
      Fw::InputLogPort* get_from_eventOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the port that receives input from eventOutText
      //!
      //! \return from_eventOutText[portNum]
      //!
      Fw::InputLogTextPort* get_from_eventOutText(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );
#endif

      //! Get the port that receives input from pingOut
      //!
      //! \return from_pingOut[portNum]
      //!
      Svc::InputPingPort* get_from_pingOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from timeCaller
      //!
      //! \return from_timeCaller[portNum]
      //!
      Fw::InputTimePort* get_from_timeCaller(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from tlmOut
      //!
      //! \return from_tlmOut[portNum]
      //!
      Fw::InputTlmPort* get_from_tlmOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object BufferAccumulatorTesterBase
      //!
      BufferAccumulatorTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object BufferAccumulatorTesterBase
      //!
      virtual ~BufferAccumulatorTesterBase(void);

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

      //! Handler prototype for from_bufferSendOutDrain
      //!
      virtual void from_bufferSendOutDrain_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      ) = 0;

      //! Handler base function for from_bufferSendOutDrain
      //!
      void from_bufferSendOutDrain_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler prototype for from_bufferSendOutReturn
      //!
      virtual void from_bufferSendOutReturn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      ) = 0;

      //! Handler base function for from_bufferSendOutReturn
      //!
      void from_bufferSendOutReturn_handlerBase(
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

      //! Push an entry on the history for from_bufferSendOutDrain
      void pushFromPortEntry_bufferSendOutDrain(
          Fw::Buffer &fwBuffer 
      );

      //! A history entry for from_bufferSendOutDrain
      //!
      typedef struct {
        Fw::Buffer fwBuffer;
      } FromPortEntry_bufferSendOutDrain;

      //! The history for from_bufferSendOutDrain
      //!
      History<FromPortEntry_bufferSendOutDrain> 
        *fromPortHistory_bufferSendOutDrain;

      //! Push an entry on the history for from_bufferSendOutReturn
      void pushFromPortEntry_bufferSendOutReturn(
          Fw::Buffer &fwBuffer 
      );

      //! A history entry for from_bufferSendOutReturn
      //!
      typedef struct {
        Fw::Buffer fwBuffer;
      } FromPortEntry_bufferSendOutReturn;

      //! The history for from_bufferSendOutReturn
      //!
      History<FromPortEntry_bufferSendOutReturn> 
        *fromPortHistory_bufferSendOutReturn;

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

      //! Invoke the to port connected to bufferSendInFill
      //!
      void invoke_to_bufferSendInFill(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Invoke the to port connected to bufferSendInReturn
      //!
      void invoke_to_bufferSendInReturn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Invoke the to port connected to pingIn
      //!
      void invoke_to_pingIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Invoke the to port connected to schedIn
      //!
      void invoke_to_schedIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_bufferSendInFill ports
      //!
      //! \return The number of to_bufferSendInFill ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferSendInFill(void) const;

      //! Get the number of to_bufferSendInReturn ports
      //!
      //! \return The number of to_bufferSendInReturn ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferSendInReturn(void) const;

      //! Get the number of from_bufferSendOutDrain ports
      //!
      //! \return The number of from_bufferSendOutDrain ports
      //!
      NATIVE_INT_TYPE getNum_from_bufferSendOutDrain(void) const;

      //! Get the number of from_bufferSendOutReturn ports
      //!
      //! \return The number of from_bufferSendOutReturn ports
      //!
      NATIVE_INT_TYPE getNum_from_bufferSendOutReturn(void) const;

      //! Get the number of to_cmdIn ports
      //!
      //! \return The number of to_cmdIn ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdIn(void) const;

      //! Get the number of from_cmdRegOut ports
      //!
      //! \return The number of from_cmdRegOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdRegOut(void) const;

      //! Get the number of from_cmdResponseOut ports
      //!
      //! \return The number of from_cmdResponseOut ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdResponseOut(void) const;

      //! Get the number of from_eventOut ports
      //!
      //! \return The number of from_eventOut ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOut(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_eventOutText ports
      //!
      //! \return The number of from_eventOutText ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOutText(void) const;
#endif

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

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of to_schedIn ports
      //!
      //! \return The number of to_schedIn ports
      //!
      NATIVE_INT_TYPE getNum_to_schedIn(void) const;

      //! Get the number of from_tlmOut ports
      //!
      //! \return The number of from_tlmOut ports
      //!
      NATIVE_INT_TYPE getNum_from_tlmOut(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_bufferSendInFill[portNum] is connected
      //!
      bool isConnected_to_bufferSendInFill(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_bufferSendInReturn[portNum] is connected
      //!
      bool isConnected_to_bufferSendInReturn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

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

      //! Check whether port is connected
      //!
      //! Whether to_schedIn[portNum] is connected
      //!
      bool isConnected_to_schedIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:
    
      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args); 

      //! Send a BA_SetMode command
      //!
      void sendCmd_BA_SetMode(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq, /*!< The command sequence number*/
          BufferAccumulatorComponentBase::OpState mode 
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
      // Event: BA_BufferAccepted
      // ----------------------------------------------------------------------

      //! Handle event BA_BufferAccepted
      //!
      virtual void logIn_ACTIVITY_HI_BA_BufferAccepted(
          void
      );

      //! Size of history for event BA_BufferAccepted
      //!
      U32 eventsSize_BA_BufferAccepted;

    protected:

      // ----------------------------------------------------------------------
      // Event: BA_QueueFull
      // ----------------------------------------------------------------------

      //! Handle event BA_QueueFull
      //!
      virtual void logIn_WARNING_HI_BA_QueueFull(
          void
      );

      //! Size of history for event BA_QueueFull
      //!
      U32 eventsSize_BA_QueueFull;

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
      // Channel: BufferAccumulator_NumQueuedBuffers
      // ----------------------------------------------------------------------

      //! Handle channel BufferAccumulator_NumQueuedBuffers
      //!
      virtual void tlmInput_BufferAccumulator_NumQueuedBuffers(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel BufferAccumulator_NumQueuedBuffers
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_BufferAccumulator_NumQueuedBuffers;

      //! The history of BufferAccumulator_NumQueuedBuffers values
      //!
      History<TlmEntry_BufferAccumulator_NumQueuedBuffers> 
        *tlmHistory_BufferAccumulator_NumQueuedBuffers;

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

      //! To port connected to bufferSendInFill
      //!
      Fw::OutputBufferSendPort m_to_bufferSendInFill[1];

      //! To port connected to bufferSendInReturn
      //!
      Fw::OutputBufferSendPort m_to_bufferSendInReturn[1];

      //! To port connected to cmdIn
      //!
      Fw::OutputCmdPort m_to_cmdIn[1];

      //! To port connected to pingIn
      //!
      Svc::OutputPingPort m_to_pingIn[1];

      //! To port connected to schedIn
      //!
      Svc::OutputSchedPort m_to_schedIn[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to bufferSendOutDrain
      //!
      Fw::InputBufferSendPort m_from_bufferSendOutDrain[1];

      //! From port connected to bufferSendOutReturn
      //!
      Fw::InputBufferSendPort m_from_bufferSendOutReturn[1];

      //! From port connected to cmdRegOut
      //!
      Fw::InputCmdRegPort m_from_cmdRegOut[1];

      //! From port connected to cmdResponseOut
      //!
      Fw::InputCmdResponsePort m_from_cmdResponseOut[1];

      //! From port connected to eventOut
      //!
      Fw::InputLogPort m_from_eventOut[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to eventOutText
      //!
      Fw::InputLogTextPort m_from_eventOutText[1];
#endif

      //! From port connected to pingOut
      //!
      Svc::InputPingPort m_from_pingOut[1];

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_bufferSendOutDrain
      //!
      static void from_bufferSendOutDrain_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Static function for port from_bufferSendOutReturn
      //!
      static void from_bufferSendOutReturn_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Static function for port from_cmdRegOut
      //!
      static void from_cmdRegOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
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

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Static function for port from_eventOutText
      //!
      static void from_eventOutText_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TextLogSeverity severity, /*!< The severity argument*/
          Fw::TextLogString &text /*!< Text of log message*/
      );
#endif

      //! Static function for port from_pingOut
      //!
      static void from_pingOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      //! Static function for port from_timeCaller
      //!
      static void from_timeCaller_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
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
