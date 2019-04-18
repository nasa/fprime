// ======================================================================
// \title  BufferManager/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferManager component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferManager_TESTER_BASE_HPP
#define BufferManager_TESTER_BASE_HPP

#include <Svc/BufferManager/BufferManagerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class BufferManagerTesterBase
  //! \brief Auto-generated base class for BufferManager component test harness
  //!
  class BufferManagerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object BufferManagerTesterBase
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

      //! Connect bufferGetCallee to to_bufferGetCallee[portNum]
      //!
      void connect_to_bufferGetCallee(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputBufferGetPort *const bufferGetCallee /*!< The port*/
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

      //! Get the port that receives input from eventOut
      //!
      //! \return from_eventOut[portNum]
      //!
      Fw::InputLogPort* get_from_eventOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the port that receives input from textEventOut
      //!
      //! \return from_textEventOut[portNum]
      //!
      Fw::InputLogTextPort* get_from_textEventOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );
#endif

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

      //! Construct object BufferManagerTesterBase
      //!
      BufferManagerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object BufferManagerTesterBase
      //!
      virtual ~BufferManagerTesterBase(void);

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

      //! Invoke the to port connected to bufferSendIn
      //!
      void invoke_to_bufferSendIn(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Invoke the to port connected to bufferGetCallee
      //!
      Fw::Buffer invoke_to_bufferGetCallee(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_timeCaller ports
      //!
      //! \return The number of from_timeCaller ports
      //!
      NATIVE_INT_TYPE getNum_from_timeCaller(void) const;

      //! Get the number of from_eventOut ports
      //!
      //! \return The number of from_eventOut ports
      //!
      NATIVE_INT_TYPE getNum_from_eventOut(void) const;

#if FW_ENABLE_TEXT_LOGGING == 1
      //! Get the number of from_textEventOut ports
      //!
      //! \return The number of from_textEventOut ports
      //!
      NATIVE_INT_TYPE getNum_from_textEventOut(void) const;
#endif

      //! Get the number of to_bufferSendIn ports
      //!
      //! \return The number of to_bufferSendIn ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferSendIn(void) const;

      //! Get the number of to_bufferGetCallee ports
      //!
      //! \return The number of to_bufferGetCallee ports
      //!
      NATIVE_INT_TYPE getNum_to_bufferGetCallee(void) const;

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
      //! Whether to_bufferSendIn[portNum] is connected
      //!
      bool isConnected_to_bufferSendIn(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_bufferGetCallee[portNum] is connected
      //!
      bool isConnected_to_bufferGetCallee(
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
      // Event: ClearedErrorState
      // ----------------------------------------------------------------------

      //! Handle event ClearedErrorState
      //!
      virtual void logIn_ACTIVITY_HI_ClearedErrorState(
          void
      );

      //! Size of history for event ClearedErrorState
      //!
      U32 eventsSize_ClearedErrorState;

    protected:

      // ----------------------------------------------------------------------
      // Event: StoreSizeExceeded
      // ----------------------------------------------------------------------

      //! Handle event StoreSizeExceeded
      //!
      virtual void logIn_WARNING_HI_StoreSizeExceeded(
          void
      );

      //! Size of history for event StoreSizeExceeded
      //!
      U32 eventsSize_StoreSizeExceeded;

    protected:

      // ----------------------------------------------------------------------
      // Event: TooManyBuffers
      // ----------------------------------------------------------------------

      //! Handle event TooManyBuffers
      //!
      virtual void logIn_WARNING_HI_TooManyBuffers(
          void
      );

      //! Size of history for event TooManyBuffers
      //!
      U32 eventsSize_TooManyBuffers;

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
      // Channel: BufferManager_NumAllocatedBuffers
      // ----------------------------------------------------------------------

      //! Handle channel BufferManager_NumAllocatedBuffers
      //!
      virtual void tlmInput_BufferManager_NumAllocatedBuffers(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel BufferManager_NumAllocatedBuffers
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_BufferManager_NumAllocatedBuffers;

      //! The history of BufferManager_NumAllocatedBuffers values
      //!
      History<TlmEntry_BufferManager_NumAllocatedBuffers> 
        *tlmHistory_BufferManager_NumAllocatedBuffers;

    protected:

      // ----------------------------------------------------------------------
      // Channel: BufferManager_AllocatedSize
      // ----------------------------------------------------------------------

      //! Handle channel BufferManager_AllocatedSize
      //!
      virtual void tlmInput_BufferManager_AllocatedSize(
          const Fw::Time& timeTag, /*!< The time*/
          const U32& val /*!< The channel value*/
      );

      //! A telemetry entry for channel BufferManager_AllocatedSize
      //!
      typedef struct {
        Fw::Time timeTag;
        U32 arg;
      } TlmEntry_BufferManager_AllocatedSize;

      //! The history of BufferManager_AllocatedSize values
      //!
      History<TlmEntry_BufferManager_AllocatedSize> 
        *tlmHistory_BufferManager_AllocatedSize;

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

      //! To port connected to bufferGetCallee
      //!
      Fw::OutputBufferGetPort m_to_bufferGetCallee[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to timeCaller
      //!
      Fw::InputTimePort m_from_timeCaller[1];

      //! From port connected to eventOut
      //!
      Fw::InputLogPort m_from_eventOut[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to textEventOut
      //!
      Fw::InputLogTextPort m_from_textEventOut[1];
#endif

      //! From port connected to tlmOut
      //!
      Fw::InputTlmPort m_from_tlmOut[1];

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
      //! Static function for port from_textEventOut
      //!
      static void from_textEventOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TextLogSeverity severity, /*!< The severity argument*/
          Fw::TextLogString &text /*!< Text of log message*/
      );
#endif

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
