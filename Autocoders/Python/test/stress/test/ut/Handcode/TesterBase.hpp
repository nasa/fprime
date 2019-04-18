// ======================================================================
// \title  TestPort/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TestPort component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestPort_TESTER_BASE_HPP
#define TestPort_TESTER_BASE_HPP

#include <Autocoders/Python/test/stress/TestPtComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace StressTest {

  //! \class TestPortTesterBase
  //! \brief Auto-generated base class for TestPort component test harness
  //!
  class TestPortTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TestPortTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from aport
      //!
      //! \return from_aport[portNum]
      //!
      Another::InputTestPort* get_from_aport(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

      //! Get the port that receives input from aport2
      //!
      //! \return from_aport2[portNum]
      //!
      Somewhere::InputTest2Port* get_from_aport2(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestPortTesterBase
      //!
      TestPortTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TestPortTesterBase
      //!
      virtual ~TestPortTesterBase(void);

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

      //! Handler prototype for from_aport
      //!
      virtual void from_aport_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          U8 arg6 //!< The third argument
      ) = 0;

      //! Handler base function for from_aport
      //!
      void from_aport_handlerBase(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          U8 arg6 //!< The third argument
      );

      //! Handler prototype for from_aport2
      //!
      virtual void from_aport2_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          Ref::Gnc::Quaternion arg6 //!< The third argument
      ) = 0;

      //! Handler base function for from_aport2
      //!
      void from_aport2_handlerBase(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          Ref::Gnc::Quaternion arg6 //!< The third argument
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

      //! Push an entry on the history for from_aport
      void pushFromPortEntry_aport(
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          U8 arg6 //!< The third argument
      );

      //! A history entry for from_aport
      //!
      typedef struct {
        I32 arg4;
        F32 arg5;
        U8 arg6;
      } FromPortEntry_aport;

      //! The history for from_aport
      //!
      History<FromPortEntry_aport>
        *fromPortHistory_aport;

      //! Push an entry on the history for from_aport2
      void pushFromPortEntry_aport2(
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          Ref::Gnc::Quaternion arg6 //!< The third argument
      );

      //! A history entry for from_aport2
      //!
      typedef struct {
        I32 arg4;
        F32 arg5;
        Ref::Gnc::Quaternion arg6;
      } FromPortEntry_aport2;

      //! The history for from_aport2
      //!
      History<FromPortEntry_aport2>
        *fromPortHistory_aport2;

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_aport ports
      //!
      //! \return The number of from_aport ports
      //!
      NATIVE_INT_TYPE getNum_from_aport(void) const;

      //! Get the number of from_aport2 ports
      //!
      //! \return The number of from_aport2 ports
      //!
      NATIVE_INT_TYPE getNum_from_aport2(void) const;

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to aport
      //!
      Another::InputTestPort m_from_aport[1];

      //! From port connected to aport2
      //!
      Somewhere::InputTest2Port m_from_aport2[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_aport
      //!
      static void from_aport_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          U8 arg6 //!< The third argument
      );

      //! Static function for port from_aport2
      //!
      static void from_aport2_static(
          Fw::PassiveComponentBase *const callComp, //!< The component instance
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg4, //!< The first argument
          F32 arg5, //!< The second argument
          Ref::Gnc::Quaternion arg6 //!< The third argument
      );

  };

} // end namespace StressTest

#endif
