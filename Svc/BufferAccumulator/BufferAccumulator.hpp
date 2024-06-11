// ======================================================================
// \title  BufferAccumulator.hpp
// \author bocchino
// \brief  BufferAccumulator interface
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_BufferAccumulator_HPP
#define Svc_BufferAccumulator_HPP

#include <Fw/Types/MemAllocator.hpp>

#include "Os/Queue.hpp"
#include "Svc/BufferAccumulator/BufferAccumulatorComponentAc.hpp"

namespace Svc {

    class BufferAccumulator : public BufferAccumulatorComponentBase {
      PRIVATE:

        // ----------------------------------------------------------------------
        // Types
        // ----------------------------------------------------------------------

        //! A BufferLogger file
        class ArrayFIFOBuffer {
            public:
                //! Construct an ArrayFIFOBuffer object
                ArrayFIFOBuffer();

                //! Destroy an ArrayFIFOBuffer File object
                ~ArrayFIFOBuffer();

                void init(Fw::Buffer* const elements,  //!< The array elements
                        NATIVE_UINT_TYPE capacity    //!< The capacity
                        );

                //! Enqueue an index.
                //! Fails if the queue is full.
                //! \return Whether the operation succeeded
                bool enqueue(const Fw::Buffer& e  //!< The element to enqueue
                        );

                //! Dequeue an index.
                //! Fails if the queue is empty.
                bool dequeue(Fw::Buffer& e  //!< The dequeued element
                        );

                //! Get the size of the queue
                //! \return The size
                U32 getSize() const;

                //! Get the capacity of the queue
                //! \return The capacity
                U32 getCapacity() const;

      PRIVATE:

                // ----------------------------------------------------------------------
                // Private member variables
                // ----------------------------------------------------------------------

                //! The memory for the elements
                Fw::Buffer* m_elements;

                //! The capacity of the queue
                NATIVE_UINT_TYPE m_capacity;

                //! The enqueue index
                NATIVE_UINT_TYPE m_enqueueIndex;

                //! The dequeue index
                NATIVE_UINT_TYPE m_dequeueIndex;

                //! The size of the queue
                NATIVE_UINT_TYPE m_size;
        };  // class ArrayFIFOBuffer

        public:
        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct BufferAccumulator instance
        //!
        BufferAccumulator(
                const char* const compName /*!< The component name*/
                );

        //! Initialize BufferAccumulator instance
        //!
        void init(const NATIVE_INT_TYPE queueDepth,   //!< The queue depth
                const NATIVE_INT_TYPE instance = 0  //!< The instance number
                );

        //! Destroy BufferAccumulator instance
        //!
        ~BufferAccumulator();

        // ----------------------------------------------------------------------
        // Public methods
        // ----------------------------------------------------------------------

        //! Give the class a memory buffer. Should be called after constructor
        //! and init, but before task is spawned.
        void allocateQueue(
                NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator,
                NATIVE_UINT_TYPE maxNumBuffers  //!< The maximum number of buffers
                );

        //! Return allocated queue. Should be done during shutdown
        void deallocateQueue(Fw::MemAllocator& allocator);

      PRIVATE:

        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for bufferSendInFill
        //!
        void
            bufferSendInFill_handler(
                    const NATIVE_INT_TYPE portNum,  //!< The port number
                    Fw::Buffer& buffer);

        //! Handler implementation for bufferSendInReturn
        //!
        void bufferSendInReturn_handler(
                const NATIVE_INT_TYPE portNum,  //!< The port number
                Fw::Buffer& buffer);

        //! Handler implementation for pingIn
        //!
        void pingIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                            U32 key  //!< Value to return to pinger
                            );

      PRIVATE:

        // ----------------------------------------------------------------------
        // Command handler implementations
        // ----------------------------------------------------------------------

        //! Implementation for SetMode command handler
        //! Set the mode
        void BA_SetMode_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                   const U32 cmdSeq,  //!< The command sequence number
                                   BufferAccumulator_OpState mode  //!< The mode
                                   );

        //! Implementation for BA_DrainBuffers command handler
        //! Drain the commanded number of buffers
        void BA_DrainBuffers_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                        const U32 cmdSeq,          /*!< The command sequence number*/
                                        U32 numToDrain,
                                        BufferAccumulator_BlockMode blockMode
                                        );

      PRIVATE:

        // ----------------------------------------------------------------------
        // Private helper methods
        // ----------------------------------------------------------------------

        //! Send a stored buffer
        void sendStoredBuffer();

      PRIVATE:

        // ----------------------------------------------------------------------
        // Private member variables
        // ----------------------------------------------------------------------

        //! The mode
        BufferAccumulator_OpState m_mode;

        //! Memory for the buffer array
        Fw::Buffer* m_bufferMemory;

        //! The FIFO queue of buffers
        ArrayFIFOBuffer m_bufferQueue;

        //! Whether to send a buffer to the downstream client
        bool m_send;

        //! If we are switched to ACCUMULATE then back to DRAIN, whether we were
        //! waiting on a buffer
        bool m_waitForBuffer;

        //! The number of QueueFull warnings sent since the last successful enqueue
        //! operation
        U32 m_numWarnings;

        //! The number of buffers drained in a partial drain command
        U32 m_numDrained;

        //! The number of buffers TO drain in a partial drain command
        U32 m_numToDrain;

        //! The DrainBuffers opcode to respond to
        FwOpcodeType m_opCode;

        //! The DrainBuffers cmdSeq to respond to
        U32 m_cmdSeq;

        //! The allocator ID
        NATIVE_INT_TYPE m_allocatorId;
    };

}  // namespace Svc

#endif
