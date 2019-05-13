// ======================================================================
// \title  TestPrm/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TestPrm component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestPrm_TESTER_BASE_HPP
#define TestPrm_TESTER_BASE_HPP

#include <Autocoders/Python/test/param_string/TestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Prm {

  //! \class TestPrmTesterBase
  //! \brief Auto-generated base class for TestPrm component test harness
  //!
  class TestPrmTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TestPrmTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect aport to to_aport[portNum]
      //!
      void connect_to_aport(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Another::InputTestPort *const aport /*!< The port*/
      );

      //! Connect CmdDisp to to_CmdDisp[portNum]
      //!
      void connect_to_CmdDisp(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputCmdPort *const CmdDisp /*!< The port*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for 'from' ports
      // Connect these input ports to the output ports under test
      // ----------------------------------------------------------------------

      //! Get the port that receives input from CmdStatus
      //!
      //! \return from_CmdStatus[portNum]
      //!
      Fw::InputCmdResponsePort* get_from_CmdStatus(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from CmdReg
      //!
      //! \return from_CmdReg[portNum]
      //!
      Fw::InputCmdRegPort* get_from_CmdReg(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from ParamGet
      //!
      //! \return from_ParamGet[portNum]
      //!
      Fw::InputPrmGetPort* get_from_ParamGet(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Get the port that receives input from ParamSet
      //!
      //! \return from_ParamSet[portNum]
      //!
      Fw::InputPrmSetPort* get_from_ParamSet(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestPrmTesterBase
      //!
      TestPrmTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestPrmTesterBase
      //!
      virtual ~TestPrmTesterBase(void);

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

      //! Invoke the to port connected to aport
      //!
      void invoke_to_aport(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          I32 arg4, /*!< The first argument*/
          F32 arg5, /*!< The second argument*/
          U8 arg6 /*!< The third argument*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_aport ports
      //!
      //! \return The number of to_aport ports
      //!
      NATIVE_INT_TYPE getNum_to_aport(void) const;

      //! Get the number of to_CmdDisp ports
      //!
      //! \return The number of to_CmdDisp ports
      //!
      NATIVE_INT_TYPE getNum_to_CmdDisp(void) const;

      //! Get the number of from_CmdStatus ports
      //!
      //! \return The number of from_CmdStatus ports
      //!
      NATIVE_INT_TYPE getNum_from_CmdStatus(void) const;

      //! Get the number of from_CmdReg ports
      //!
      //! \return The number of from_CmdReg ports
      //!
      NATIVE_INT_TYPE getNum_from_CmdReg(void) const;

      //! Get the number of from_ParamGet ports
      //!
      //! \return The number of from_ParamGet ports
      //!
      NATIVE_INT_TYPE getNum_from_ParamGet(void) const;

      //! Get the number of from_ParamSet ports
      //!
      //! \return The number of from_ParamSet ports
      //!
      NATIVE_INT_TYPE getNum_from_ParamSet(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_aport[portNum] is connected
      //!
      bool isConnected_to_aport(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_CmdDisp[portNum] is connected
      //!
      bool isConnected_to_CmdDisp(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      // ----------------------------------------------------------------------
      // Functions for sending commands
      // ----------------------------------------------------------------------

    protected:

      // send command buffers directly - used for intentional command encoding errors
      void sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args);

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
      // Parameter: stringparam
      // ----------------------------------------------------------------------

      void paramSet_stringparam(
          const Fw::ParamString& val, /*!< The parameter value*/
          Fw::ParamValid valid /*!< The parameter valid flag*/
      );

      void paramSend_stringparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void paramSave_stringparam(
          const NATIVE_INT_TYPE instance, /*!< The instance number*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to aport
      //!
      Another::OutputTestPort m_to_aport[1];

      //! To port connected to CmdDisp
      //!
      Fw::OutputCmdPort m_to_CmdDisp[1];

    private:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      //! From port connected to CmdStatus
      //!
      Fw::InputCmdResponsePort m_from_CmdStatus[1];

      //! From port connected to CmdReg
      //!
      Fw::InputCmdRegPort m_from_CmdReg[1];

      //! From port connected to ParamGet
      //!
      Fw::InputPrmGetPort m_from_ParamGet[1];

      //! From port connected to ParamSet
      //!
      Fw::InputPrmSetPort m_from_ParamSet[1];

    private:

      // ----------------------------------------------------------------------
      // Static functions for output ports
      // ----------------------------------------------------------------------

      //! Static function for port from_CmdStatus
      //!
      static void from_CmdStatus_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< Command Op Code*/
          U32 cmdSeq, /*!< Command Sequence*/
          Fw::CommandResponse response /*!< The command response argument*/
      );

      //! Static function for port from_CmdReg
      //!
      static void from_CmdReg_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< Command Op Code*/
      );

      //! Static function for port from_ParamGet
      //!
      static Fw::ParamValid from_ParamGet_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

      //! Static function for port from_ParamSet
      //!
      static void from_ParamSet_static(
          Fw::PassiveComponentBase *const callComp, /*!< The component instance*/
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

    private:

      // ----------------------------------------------------------------------
      // Parameter validity flags
      // ----------------------------------------------------------------------

      //! True if parameter stringparam was successfully received
      //!
      Fw::ParamValid m_param_stringparam_valid;

    private:

      // ----------------------------------------------------------------------
      // Parameter variables
      // ----------------------------------------------------------------------

      //! Parameter stringparam
      //!
      Fw::ParamString m_param_stringparam;

  };

} // end namespace Prm

#endif
