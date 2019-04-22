// ====================================================================== 
// \title  MathReceiverImpl.hpp
// \author tcanham
// \brief  hpp file for MathReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef MathReceiver_HPP
#define MathReceiver_HPP

#include "Ref/MathReceiver/MathReceiverComponentAc.hpp"

namespace Ref {

  class MathReceiverComponentImpl :
    public MathReceiverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object MathReceiver
      //!
      MathReceiverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object MathReceiver
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object MathReceiver
      //!
      ~MathReceiverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for mathIn
      //!
      void mathIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 val1, 
          F32 val2, 
          MathOperation operation /*!< operation argument*/
      );

      //! Handler implementation for SchedIn
      //!
      void SchedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for MR_SET_FACTOR1 command handler
      //! Set operation multiplication factor1
      void MR_SET_FACTOR1_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          F32 val /*!< The first factor*/
      );

      //! Implementation for MR_CLEAR_EVENT_THROTTLE command handler
      //! Clear the event throttle
      void MR_CLEAR_EVENT_THROTTLE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      // stored factor1
      F32 m_factor1;
      // number of times factor1 has been written
      U32 m_factor1s;

      void parameterUpdated(
          FwPrmIdType id /*!< The parameter ID*/
      );

    };

} // end namespace Ref

#endif
