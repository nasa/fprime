// ======================================================================
// \title  TestComponent/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TestComponent component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestComponent_TESTER_BASE_HPP
#define TestComponent_TESTER_BASE_HPP

#include <Autocoders/Python/test/string_port/TestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace ExampleComponents {

  //! \class TestComponentTesterBase
  //! \brief Auto-generated base class for TestComponent component test harness
  //!
  class TestComponentTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TestComponentTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect testIn to to_testIn[portNum]
      //!
      void connect_to_testIn(
          const NATIVE_INT_TYPE portNum, //!< The port number
          AnotherExample::InputTestPort *const testIn //!< The port
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from testOut
      //!
      //! \return from_testOut[portNum]
      //!
      AnotherExample::InputTestPort* get_from_testOut(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestComponentTesterBase
      //!
      TestComponentTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TestComponentTesterBase
      //!
      virtual ~TestComponentTesterBase(void);

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

      //! Handler prototype for from_testOut
      //!
      virtual void from_testOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::arg2String &arg2, //!< A string argument
          AnotherExample::arg3Buffer arg3 //!< A buffer argument
      ) = 0;

      //! Handler base function for from_testOut
      //!
      void from_testOut_handlerBase(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::arg2String &arg2, //!< A string argument
          AnotherExample::arg3Buffer arg3 //!< A buffer argument
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

      //! Push an entry on the history for from_testOut
      void pushFromPortEntry_testOut(
          I32 arg1, //!< A built-in type argument
          AnotherExample::arg2String &arg2, //!< A string argument
          AnotherExample::arg3Buffer arg3 //!< A buffer argument
      );

      //! A history entry for from_testOut
      //!
      typedef struct {
        I32 arg1;
        AnotherExample::arg2String arg2;
        AnotherExample::arg3Buffer arg3;
      } FromPortEntry_testOut;

      //! The history for from_testOut
      //!
      History<FromPortEntry_testOut>
        *fromPortHistory_testOut;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to testIn
      //!
      void invoke_to_testIn(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::arg2String &arg2, //!< A string argument
          AnotherExample::arg3Buffer arg3 //!< A buffer argument
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_testIn ports
      //!
      //! \return The number of to_testIn ports
      //!
      NATIVE_INT_TYPE getNum_to_testIn(void) const;

      //! Get the number of from_testOut ports
      //!
      //! \return The number of from_testOut ports
      //!
      NATIVE_INT_TYPE getNum_from_testOut(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_testIn[portNum] is connected
      //!
      bool isConnected_to_testIn(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to testIn
      //!
      AnotherExample::OutputTestPort m_to_testIn[6];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to testOut
      //!
      AnotherExample::InputTestPort m_from_testOut[4];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_testOut
      //!
      static void from_testOut_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::arg2String &arg2, //!< A string argument
          AnotherExample::arg3Buffer arg3 //!< A buffer argument
      );

  };

} // end namespace ExampleComponents

#endif
