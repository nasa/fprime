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

#include <Autocoders/Python/test/port_loopback/ExampleComponentAc.hpp>
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
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect exampleInput to to_exampleInput[portNum]
      //!
      void connect_to_exampleInput(
          const NATIVE_INT_TYPE portNum, //!< The port number
          AnotherExample::InputExamplePort *const exampleInput //!< The port
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
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ExampleTesterBase
      //!
      ExampleTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
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

      //! Handler prototype for from_exampleOutput
      //!
      virtual void from_exampleOutput_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6
      ) = 0;

      //! Handler base function for from_exampleOutput
      //!
      void from_exampleOutput_handlerBase(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6
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
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6
      );

      //! A history entry for from_exampleOutput
      //!
      typedef struct {
        I32 arg1;
        AnotherExample::SomeEnum arg2;
        AnotherExample::arg6String arg6;
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
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6
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

      //! Get the number of from_exampleOutput ports
      //!
      //! \return The number of from_exampleOutput ports
      //!
      NATIVE_INT_TYPE getNum_from_exampleOutput(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_exampleInput[portNum] is connected
      //!
      bool isConnected_to_exampleInput(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to exampleInput
      //!
      AnotherExample::OutputExamplePort m_to_exampleInput[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to exampleOutput
      //!
      AnotherExample::InputExamplePort m_from_exampleOutput[3];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_exampleOutput
      //!
      static void from_exampleOutput_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6
      );

  };

} // end namespace ExampleComponents

#endif
