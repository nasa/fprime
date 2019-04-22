// ======================================================================
// \title  CommandTester/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for CommandTester component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CommandTester_TESTER_BASE_HPP
#define CommandTester_TESTER_BASE_HPP

#include <Autocoders/Python/test/command_tester/CommandTestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Cmd {

  //! \class CommandTesterTesterBase
  //! \brief Auto-generated base class for CommandTester component test harness
  //!
  class CommandTesterTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object CommandTesterTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect cmdStatusPort to to_cmdStatusPort[portNum]
      //!
      void connect_to_cmdStatusPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdResponsePort *const cmdStatusPort /*!< The port*/
      );

      //! Connect cmdRegPort to to_cmdRegPort[portNum]
      //!
      void connect_to_cmdRegPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdRegPort *const cmdRegPort /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from cmdSendPort
      //!
      //! \return from_cmdSendPort[portNum]
      //!
      Fw::InputCmdPort* get_from_cmdSendPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CommandTesterTesterBase
      //!
      CommandTesterTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CommandTesterTesterBase
      //!
      virtual ~CommandTesterTesterBase(void);

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

      //! Handler prototype for from_cmdSendPort
      //!
      virtual void from_cmdSendPort_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      ) = 0;

      //! Handler base function for from_cmdSendPort
      //!
      void from_cmdSendPort_handlerBase(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
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

      //! Push an entry on the history for from_cmdSendPort
      void pushFromPortEntry_cmdSendPort(
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      );

      //! A history entry for from_cmdSendPort
      //!
      typedef struct {
        FwOpcodeType opCode;
        U32 cmdSeq;
        Fw::CmdArgBuffer args;
      } FromPortEntry_cmdSendPort;

      //! The history for from_cmdSendPort
      //!
      History<FromPortEntry_cmdSendPort>
        *fromPortHistory_cmdSendPort;

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to cmdStatusPort
      //!
      void invoke_to_cmdStatusPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Invoke the to port connected to cmdRegPort
      //!
      void invoke_to_cmdRegPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of from_cmdSendPort ports
      //!
      //! \return The number of from_cmdSendPort ports
      //!
      NATIVE_INT_TYPE getNum_from_cmdSendPort(void) const;

      //! Get the number of to_cmdStatusPort ports
      //!
      //! \return The number of to_cmdStatusPort ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdStatusPort(void) const;

      //! Get the number of to_cmdRegPort ports
      //!
      //! \return The number of to_cmdRegPort ports
      //!
      NATIVE_INT_TYPE getNum_to_cmdRegPort(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_cmdStatusPort[portNum] is connected
      //!
      bool isConnected_to_cmdStatusPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_cmdRegPort[portNum] is connected
      //!
      bool isConnected_to_cmdRegPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to cmdStatusPort
      //!
      Fw::OutputCmdResponsePort m_to_cmdStatusPort[1];

      //! To port connected to cmdRegPort
      //!
      Fw::OutputCmdRegPort m_to_cmdRegPort[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to cmdSendPort
      //!
      Fw::InputCmdPort m_from_cmdSendPort[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_cmdSendPort
      //!
      static void from_cmdSendPort_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CmdArgBuffer &args /*!< Buffer containing arguments*/
      );

  };

} // end namespace Cmd

#endif
