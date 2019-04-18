// ======================================================================
// \title  LinuxGpioDriver/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxGpioDriver component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxGpioDriver_TESTER_BASE_HPP
#define LinuxGpioDriver_TESTER_BASE_HPP

#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Drv {

  //! \class LinuxGpioDriverTesterBase
  //! \brief Auto-generated base class for LinuxGpioDriver component test harness
  //!
  class LinuxGpioDriverTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object LinuxGpioDriverTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect gpioWrite to to_gpioWrite[portNum]
      //!
      void connect_to_gpioWrite(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Drv::InputGpioWritePort *const gpioWrite /*!< The port*/
      );

      //! Connect gpioRead to to_gpioRead[portNum]
      //!
      void connect_to_gpioRead(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Drv::InputGpioReadPort *const gpioRead /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

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

      //! Get the port that receives input from Time
      //!
      //! \return from_Time[portNum]
      //!
      Fw::InputTimePort* get_from_Time(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from intOut
      //!
      //! \return from_intOut[portNum]
      //!
      Svc::InputCyclePort* get_from_intOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxGpioDriverTesterBase
      //!
      LinuxGpioDriverTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxGpioDriverTesterBase
      //!
      virtual ~LinuxGpioDriverTesterBase(void);

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

      //! Handler prototype for from_intOut
      //!
      virtual void from_intOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
      ) = 0;

      //! Handler base function for from_intOut
      //!
      void from_intOut_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
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

      //! Push an entry on the history for from_intOut
      void pushFromPortEntry_intOut(
          Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
      );

      //! A history entry for from_intOut
      //!
      typedef struct {
        Svc::TimerVal cycleStart;
      } FromPortEntry_intOut;

      //! The history for from_intOut
      //!
      History<FromPortEntry_intOut> 
        *fromPortHistory_intOut;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to gpioWrite
      //!
      void invoke_to_gpioWrite(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool state 
      );

      //! Invoke the to port connected to gpioRead
      //!
      void invoke_to_gpioRead(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool &state 
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_gpioWrite ports
      //!
      //! \return The number of to_gpioWrite ports
      //!
      NATIVE_INT_TYPE getNum_to_gpioWrite(void) const;

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

      //! Get the number of to_gpioRead ports
      //!
      //! \return The number of to_gpioRead ports
      //!
      NATIVE_INT_TYPE getNum_to_gpioRead(void) const;

      //! Get the number of from_Time ports
      //!
      //! \return The number of from_Time ports
      //!
      NATIVE_INT_TYPE getNum_from_Time(void) const;

      //! Get the number of from_intOut ports
      //!
      //! \return The number of from_intOut ports
      //!
      NATIVE_INT_TYPE getNum_from_intOut(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_gpioWrite[portNum] is connected
      //!
      bool isConnected_to_gpioWrite(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_gpioRead[portNum] is connected
      //!
      bool isConnected_to_gpioRead(
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
      // Event: GP_OpenError
      // ----------------------------------------------------------------------

      //! Handle event GP_OpenError
      //!
      virtual void logIn_WARNING_HI_GP_OpenError(
          I32 gpio, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event GP_OpenError
      //!
      typedef struct {
        I32 gpio;
        I32 error;
      } EventEntry_GP_OpenError;

      //! The history of GP_OpenError events
      //!
      History<EventEntry_GP_OpenError> 
        *eventHistory_GP_OpenError;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_ConfigError
      // ----------------------------------------------------------------------

      //! Handle event GP_ConfigError
      //!
      virtual void logIn_WARNING_HI_GP_ConfigError(
          I32 gpio, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event GP_ConfigError
      //!
      typedef struct {
        I32 gpio;
        I32 error;
      } EventEntry_GP_ConfigError;

      //! The history of GP_ConfigError events
      //!
      History<EventEntry_GP_ConfigError> 
        *eventHistory_GP_ConfigError;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_WriteError
      // ----------------------------------------------------------------------

      //! Handle event GP_WriteError
      //!
      virtual void logIn_WARNING_HI_GP_WriteError(
          I32 gpio, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event GP_WriteError
      //!
      typedef struct {
        I32 gpio;
        I32 error;
      } EventEntry_GP_WriteError;

      //! The history of GP_WriteError events
      //!
      History<EventEntry_GP_WriteError> 
        *eventHistory_GP_WriteError;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_ReadError
      // ----------------------------------------------------------------------

      //! Handle event GP_ReadError
      //!
      virtual void logIn_WARNING_HI_GP_ReadError(
          I32 gpio, /*!< The device*/
          I32 error /*!< The error code*/
      );

      //! A history entry for event GP_ReadError
      //!
      typedef struct {
        I32 gpio;
        I32 error;
      } EventEntry_GP_ReadError;

      //! The history of GP_ReadError events
      //!
      History<EventEntry_GP_ReadError> 
        *eventHistory_GP_ReadError;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_PortOpened
      // ----------------------------------------------------------------------

      //! Handle event GP_PortOpened
      //!
      virtual void logIn_ACTIVITY_HI_GP_PortOpened(
          I32 gpio /*!< The device*/
      );

      //! A history entry for event GP_PortOpened
      //!
      typedef struct {
        I32 gpio;
      } EventEntry_GP_PortOpened;

      //! The history of GP_PortOpened events
      //!
      History<EventEntry_GP_PortOpened> 
        *eventHistory_GP_PortOpened;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_IntStartError
      // ----------------------------------------------------------------------

      //! Handle event GP_IntStartError
      //!
      virtual void logIn_WARNING_HI_GP_IntStartError(
          I32 gpio /*!< The device*/
      );

      //! A history entry for event GP_IntStartError
      //!
      typedef struct {
        I32 gpio;
      } EventEntry_GP_IntStartError;

      //! The history of GP_IntStartError events
      //!
      History<EventEntry_GP_IntStartError> 
        *eventHistory_GP_IntStartError;

    protected:

      // ----------------------------------------------------------------------
      // Event: GP_IntWaitError
      // ----------------------------------------------------------------------

      //! Handle event GP_IntWaitError
      //!
      virtual void logIn_WARNING_HI_GP_IntWaitError(
          I32 gpio /*!< The device*/
      );

      //! A history entry for event GP_IntWaitError
      //!
      typedef struct {
        I32 gpio;
      } EventEntry_GP_IntWaitError;

      //! The history of GP_IntWaitError events
      //!
      History<EventEntry_GP_IntWaitError> 
        *eventHistory_GP_IntWaitError;

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

      //! To port connected to gpioWrite
      //!
      Drv::OutputGpioWritePort m_to_gpioWrite[1];

      //! To port connected to gpioRead
      //!
      Drv::OutputGpioReadPort m_to_gpioRead[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to Log
      //!
      Fw::InputLogPort m_from_Log[1];

#if FW_ENABLE_TEXT_LOGGING == 1
      //! From port connected to LogText
      //!
      Fw::InputLogTextPort m_from_LogText[1];
#endif

      //! From port connected to Time
      //!
      Fw::InputTimePort m_from_Time[1];

      //! From port connected to intOut
      //!
      Svc::InputCyclePort m_from_intOut[2];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

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

      //! Static function for port from_Time
      //!
      static void from_Time_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

      //! Static function for port from_intOut
      //!
      static void from_intOut_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
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
