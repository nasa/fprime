// ======================================================================
// \title  TestSerial/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TestSerial component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestSerial_TESTER_BASE_HPP
#define TestSerial_TESTER_BASE_HPP

#include <Autocoders/Python/test/serial_passive/TestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace TestComponents {

  //! \class TestSerialTesterBase
  //! \brief Auto-generated base class for TestSerial component test harness
  //!
  class TestSerialTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TestSerialTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect SerialInSync to to_SerialInSync[portNum]
      //!
      void connect_to_SerialInSync(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputSerializePort *const SerialInSync /*!< The port*/
      );

      //! Connect SerialInGuarded to to_SerialInGuarded[portNum]
      //!
      void connect_to_SerialInGuarded(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputSerializePort *const SerialInGuarded /*!< The port*/
      );

      //! Connect SerialInAsync to to_SerialInAsync[portNum]
      //!
      void connect_to_SerialInAsync(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputSerializePort *const SerialInAsync /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from SerialOut
      //!
      //! \return from_SerialOut[portNum]
      //!
      Fw::InputSerializePort* get_from_SerialOut(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestSerialTesterBase
      //!
      TestSerialTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestSerialTesterBase
      //!
      virtual ~TestSerialTesterBase(void);

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
      // Handler prototypes for serial from ports
      // ----------------------------------------------------------------------

      //! Handler prototype for from_SerialOut
      //!
      virtual void from_SerialOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      ) = 0;

      //! Handler base class function prototype for from_SerialOut
      //!
      void from_SerialOut_handlerBase (
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );


    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to SerialInSync
      //!
      void invoke_to_SerialInSync(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer
      );

      //! Invoke the to port connected to SerialInGuarded
      //!
      void invoke_to_SerialInGuarded(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer
      );

      //! Invoke the to port connected to SerialInAsync
      //!
      void invoke_to_SerialInAsync(
          NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_SerialInSync ports
      //!
      //! \return The number of to_SerialInSync ports
      //!
      NATIVE_INT_TYPE getNum_to_SerialInSync(void) const;

      //! Get the number of to_SerialInGuarded ports
      //!
      //! \return The number of to_SerialInGuarded ports
      //!
      NATIVE_INT_TYPE getNum_to_SerialInGuarded(void) const;

      //! Get the number of to_SerialInAsync ports
      //!
      //! \return The number of to_SerialInAsync ports
      //!
      NATIVE_INT_TYPE getNum_to_SerialInAsync(void) const;

      //! Get the number of from_SerialOut ports
      //!
      //! \return The number of from_SerialOut ports
      //!
      NATIVE_INT_TYPE getNum_from_SerialOut(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_SerialInSync[portNum] is connected
      //!
      bool isConnected_to_SerialInSync(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_SerialInGuarded[portNum] is connected
      //!
      bool isConnected_to_SerialInGuarded(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_SerialInAsync[portNum] is connected
      //!
      bool isConnected_to_SerialInAsync(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to SerialInSync
      //!
      Fw::OutputSerializePort m_to_SerialInSync[2];

      //! To port connected to SerialInGuarded
      //!
      Fw::OutputSerializePort m_to_SerialInGuarded[2];

      //! To port connected to SerialInAsync
      //!
      Fw::OutputSerializePort m_to_SerialInAsync[2];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to SerialOut
      //!
      Fw::InputSerializePort m_from_SerialOut[2];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_SerialOut
      //!
      static void from_SerialOut_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::SerializeBufferBase& Buffer //!< serialized data buffer
      );

  };

} // end namespace TestComponents

#endif
