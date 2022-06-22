// ======================================================================
// \title  SignalGen.hpp
// \author mstarch
// \brief  hpp file for SignalGen component implementation class
// ======================================================================

#ifndef SignalGen_HPP
#define SignalGen_HPP

#include "Ref/SignalGen/SignalGenComponentAc.hpp"

namespace Ref {

  class SignalGen :
    public SignalGenComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SignalGen
      //!
      SignalGen(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object SignalGen
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SignalGen
      //!
      ~SignalGen();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< 
      The call order
      */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for SignalGen_Settings command handler
      //! Signal Generator Settings
      void SignalGen_Settings_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 Frequency, 
          F32 Amplitude, 
          F32 Phase, 
          Ref::SignalType SigType 
      );

      //! Implementation for SignalGen_Toggle command handler
      //! Toggle Signal Generator On/Off.
      void SignalGen_Toggle_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for SignalGen_Skip command handler
      //! Skip next sample
      void SignalGen_Skip_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );


    };

} // end namespace Ref

#endif
