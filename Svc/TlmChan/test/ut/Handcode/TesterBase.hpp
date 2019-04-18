// ======================================================================
// \title  TlmChan/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TlmChan component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TlmChan_TESTER_BASE_HPP
#define TlmChan_TESTER_BASE_HPP

#include <Svc/TlmChan/TlmChanComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>

namespace Svc {

  //! \class TlmChanTesterBase
  //! \brief Auto-generated base class for TlmChan component test harness
  //!
  class TlmChanTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TlmChanTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect TlmRecv to to_TlmRecv[portNum]
      //!
      void connect_to_TlmRecv(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::InputTlmPort *const TlmRecv //!< The port
      );

      //! Connect TlmGet to to_TlmGet[portNum]
      //!
      void connect_to_TlmGet(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::InputTlmPort *const TlmGet //!< The port
      );

      //! Connect Run to to_Run[portNum]
      //!
      void connect_to_Run(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Svc::InputSchedPort *const Run //!< The port
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from PktSend
      //!
      //! \return from_PktSend[portNum]
      //!
      Fw::InputComPort* get_from_PktSend(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TlmChanTesterBase
      //!
      TlmChanTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TlmChanTesterBase
      //!
      virtual ~TlmChanTesterBase(void);

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
              const U32 maxSize //!< The maximum history size
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
              T entry //!< The item
          ) {
            FW_ASSERT(this->numEntries < this->maxSize);
            entries[this->numEntries++] = entry;
          }

          //! Get an item at an index
          //!
          //! \return The item at index i
          //!
          T at(
              const U32 i //!< The index
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

      //! Handler prototype for from_PktSend
      //!
      virtual void from_PktSend_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      ) = 0;

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

      //! Push an entry on the history for from_PktSend
      void pushFromPortEntry_PktSend(
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      );

      //! A history entry for from_PktSend
      //!
      typedef struct {
        Fw::ComBuffer data;
        U32 context;
      } FromPortEntry_PktSend;

      //! The history for from_PktSend
      //!
      History<FromPortEntry_PktSend> 
        *fromPortHistory_PktSend;

    protected:

      // ----------------------------------------------------------------------
      // Handler base functions for from ports
      // ----------------------------------------------------------------------

      //! Handler base function for from_PktSend
      //!
      void from_PktSend_handlerBase(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      );

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to TlmRecv
      //!
      void invoke_to_TlmRecv(
          const NATIVE_INT_TYPE portNum, //!< The port number
          FwChanIdType id, //!< Telemetry Channel ID
          Fw::Time &timeTag, //!< Time Tag
          Fw::TlmBuffer &val //!< Buffer containing serialized telemetry value
      );

      //! Invoke the to port connected to TlmGet
      //!
      void invoke_to_TlmGet(
          const NATIVE_INT_TYPE portNum, //!< The port number
          FwChanIdType id, //!< Telemetry Channel ID
          Fw::Time &timeTag, //!< Time Tag
          Fw::TlmBuffer &val //!< Buffer containing serialized telemetry value
      );

      //! Invoke the to port connected to Run
      //!
      void invoke_to_Run(
          const NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_TlmRecv ports
      //!
      //! \return The number of to_TlmRecv ports
      //!
      NATIVE_INT_TYPE getNum_to_TlmRecv(void) const;

      //! Get the number of to_TlmGet ports
      //!
      //! \return The number of to_TlmGet ports
      //!
      NATIVE_INT_TYPE getNum_to_TlmGet(void) const;

      //! Get the number of to_Run ports
      //!
      //! \return The number of to_Run ports
      //!
      NATIVE_INT_TYPE getNum_to_Run(void) const;

      //! Get the number of from_PktSend ports
      //!
      //! \return The number of from_PktSend ports
      //!
      NATIVE_INT_TYPE getNum_from_PktSend(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_TlmRecv[portNum] is connected
      //!
      bool isConnected_to_TlmRecv(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

      //! Check whether port is connected
      //!
      //! Whether to_TlmGet[portNum] is connected
      //!
      bool isConnected_to_TlmGet(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

      //! Check whether port is connected
      //!
      //! Whether to_Run[portNum] is connected
      //!
      bool isConnected_to_Run(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to TlmRecv
      //!
      Fw::OutputTlmPort m_to_TlmRecv[1];

      //! To port connected to TlmGet
      //!
      Fw::OutputTlmPort m_to_TlmGet[1];

      //! To port connected to Run
      //!
      Svc::OutputSchedPort m_to_Run[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to PktSend
      //!
      Fw::InputComPort m_from_PktSend[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_PktSend
      //!
      static void from_PktSend_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      );

  };

} // end namespace Svc

#endif
