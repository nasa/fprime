// ====================================================================== 
// \title  BufferAccumulator.hpp
// \author bocchino
// \brief  BufferAccumulator interface
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef ASTERIA_BufferAccumulator_HPP
#define ASTERIA_BufferAccumulator_HPP

#include "ASTERIA/Components/BufferAccumulator/BufferAccumulatorComponentAc.hpp"
#include "ASTERIA/Types/Queue/ArrayFIFOBuffer.hpp"

namespace ASTERIA {

  class BufferAccumulator :
    public BufferAccumulatorComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct BufferAccumulator instance
      //!
      BufferAccumulator(
          const char *const compName, //!< The component name
          const U32 maxNumBuffers //!< The maximum number of buffers
      );

      //! Initialize BufferAccumulator instance
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Destroy BufferAccumulator instance
      //!
      ~BufferAccumulator(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for bufferSendInFill
      //!
      void bufferSendInFill_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer buffer 
      );

      //! Handler implementation for bufferSendInReturn
      //!
      void bufferSendInReturn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer buffer 
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for SetMode command handler
      //! Set the mode
      void SetMode_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          BufferAccumulatorMode mode //!< The mode
      );
    PRIVATE:

      // ----------------------------------------------------------------------
      // Private helper methods 
      // ----------------------------------------------------------------------

      //! Send a stored buffer
      void sendStoredBuffer(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private member variables 
      // ----------------------------------------------------------------------

      //! The mode
      BufferAccumulatorMode mode;

      //! Memory for the buffer array
      Fw::Buffer *const bufferMemory;

      //! An array of buffers for the queue
      VariableSizeArrayBuffer bufferArray;

      //! The FIFO queue of buffers
      ArrayFIFOBuffer bufferQueue;

      //! Whether to send a buffer to the downstream client
      bool send;

      //! The number of QueueFull warnings sent since the last successful enqueue operation
      U32 numWarnings;

  };

}

#endif
