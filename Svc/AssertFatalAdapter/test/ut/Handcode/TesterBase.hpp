// ======================================================================
// \title  AssertFatalAdapter/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for AssertFatalAdapter component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef AssertFatalAdapter_TESTER_BASE_HPP
#define AssertFatalAdapter_TESTER_BASE_HPP

#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class AssertFatalAdapterTesterBase
  //! \brief Auto-generated base class for AssertFatalAdapter component test harness
  //!
  class AssertFatalAdapterTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object AssertFatalAdapterTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

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

      //! Construct object AssertFatalAdapterTesterBase
      //!
      AssertFatalAdapterTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object AssertFatalAdapterTesterBase
      //!
      virtual ~AssertFatalAdapterTesterBase(void);

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

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

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
      // Event: AF_ASSERT_0
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_0
      //!
      virtual void logIn_FATAL_AF_ASSERT_0(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line /*!< Line number of the assert*/
      );

      //! A history entry for event AF_ASSERT_0
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
      } EventEntry_AF_ASSERT_0;

      //! The history of AF_ASSERT_0 events
      //!
      History<EventEntry_AF_ASSERT_0> 
        *eventHistory_AF_ASSERT_0;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_1
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_1
      //!
      virtual void logIn_FATAL_AF_ASSERT_1(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1 /*!< First assert argument*/
      );

      //! A history entry for event AF_ASSERT_1
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
      } EventEntry_AF_ASSERT_1;

      //! The history of AF_ASSERT_1 events
      //!
      History<EventEntry_AF_ASSERT_1> 
        *eventHistory_AF_ASSERT_1;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_2
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_2
      //!
      virtual void logIn_FATAL_AF_ASSERT_2(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1, /*!< First assert argument*/
          U32 arg2 /*!< Second assert argument*/
      );

      //! A history entry for event AF_ASSERT_2
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
        U32 arg2;
      } EventEntry_AF_ASSERT_2;

      //! The history of AF_ASSERT_2 events
      //!
      History<EventEntry_AF_ASSERT_2> 
        *eventHistory_AF_ASSERT_2;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_3
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_3
      //!
      virtual void logIn_FATAL_AF_ASSERT_3(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1, /*!< First assert argument*/
          U32 arg2, /*!< Second assert argument*/
          U32 arg3 /*!< Third assert argument*/
      );

      //! A history entry for event AF_ASSERT_3
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
        U32 arg2;
        U32 arg3;
      } EventEntry_AF_ASSERT_3;

      //! The history of AF_ASSERT_3 events
      //!
      History<EventEntry_AF_ASSERT_3> 
        *eventHistory_AF_ASSERT_3;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_4
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_4
      //!
      virtual void logIn_FATAL_AF_ASSERT_4(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1, /*!< First assert argument*/
          U32 arg2, /*!< Second assert argument*/
          U32 arg3, /*!< Third assert argument*/
          U32 arg4 /*!< Fourth assert argument*/
      );

      //! A history entry for event AF_ASSERT_4
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
        U32 arg2;
        U32 arg3;
        U32 arg4;
      } EventEntry_AF_ASSERT_4;

      //! The history of AF_ASSERT_4 events
      //!
      History<EventEntry_AF_ASSERT_4> 
        *eventHistory_AF_ASSERT_4;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_5
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_5
      //!
      virtual void logIn_FATAL_AF_ASSERT_5(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1, /*!< First assert argument*/
          U32 arg2, /*!< Second assert argument*/
          U32 arg3, /*!< Third assert argument*/
          U32 arg4, /*!< Fourth assert argument*/
          U32 arg5 /*!< Fifth assert argument*/
      );

      //! A history entry for event AF_ASSERT_5
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
        U32 arg2;
        U32 arg3;
        U32 arg4;
        U32 arg5;
      } EventEntry_AF_ASSERT_5;

      //! The history of AF_ASSERT_5 events
      //!
      History<EventEntry_AF_ASSERT_5> 
        *eventHistory_AF_ASSERT_5;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_6
      // ----------------------------------------------------------------------

      //! Handle event AF_ASSERT_6
      //!
      virtual void logIn_FATAL_AF_ASSERT_6(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 arg1, /*!< First assert argument*/
          U32 arg2, /*!< Second assert argument*/
          U32 arg3, /*!< Third assert argument*/
          U32 arg4, /*!< Fourth assert argument*/
          U32 arg5, /*!< Fifth assert argument*/
          U32 arg6 /*!< Sixth assert argument*/
      );

      //! A history entry for event AF_ASSERT_6
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 arg1;
        U32 arg2;
        U32 arg3;
        U32 arg4;
        U32 arg5;
        U32 arg6;
      } EventEntry_AF_ASSERT_6;

      //! The history of AF_ASSERT_6 events
      //!
      History<EventEntry_AF_ASSERT_6> 
        *eventHistory_AF_ASSERT_6;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_UNEXPECTED_ASSERT
      // ----------------------------------------------------------------------

      //! Handle event AF_UNEXPECTED_ASSERT
      //!
      virtual void logIn_FATAL_AF_UNEXPECTED_ASSERT(
          Fw::LogStringArg& file, /*!< The source file of the assert*/
          U32 line, /*!< Line number of the assert*/
          U32 numArgs /*!< Number of unexpected arguments*/
      );

      //! A history entry for event AF_UNEXPECTED_ASSERT
      //!
      typedef struct {
        Fw::LogStringArg file;
        U32 line;
        U32 numArgs;
      } EventEntry_AF_UNEXPECTED_ASSERT;

      //! The history of AF_UNEXPECTED_ASSERT events
      //!
      History<EventEntry_AF_UNEXPECTED_ASSERT> 
        *eventHistory_AF_UNEXPECTED_ASSERT;

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
      // From ports
      // ----------------------------------------------------------------------

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
