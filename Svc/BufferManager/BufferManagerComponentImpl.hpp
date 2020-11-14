// ======================================================================
// \title  BufferManagerComponentImpl.hpp
// \author tcanham
// \brief  hpp file for BufferManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferManager_HPP
#define BufferManager_HPP

#include "Svc/BufferManager/BufferManagerComponentAc.hpp"
#include <Fw/Types/MemAllocator.hpp>

namespace Svc
{

    class BufferManagerComponentImpl : public BufferManagerComponentBase
    {

    public:
        static const NATIVE_UINT_TYPE MAX_NUM_BINS = 10;

        // ----------------------------------------------------------------------
        // Construction, initialization, and destruction
        // ----------------------------------------------------------------------

        //! Construct object BufferManager
        //!
        BufferManagerComponentImpl(
            const char *const compName /*!< The component name*/
        );

        //! Initialize object BufferManager
        //!
        void init(
            const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
        );

        // Defines a buffer bin
        struct BufferBin
        {
            NATIVE_UINT_TYPE bufferSize; //!< size of the buffers in this bin. Set to zero for unused bins.
            NATIVE_UINT_TYPE numBuffers; //!< number of buffers in this bin. Set to zero for unused bins.
        };

        // Set of bins for the BufferManager
        struct BufferBins
        {
            BufferBin bins[MAX_NUM_BINS]; //!< set of bins to define buffers
        };

        //! set up configuration

        void setup(
            NATIVE_UINT_TYPE mgrID,      //!< ID of manager for buffer checking
            NATIVE_UINT_TYPE memID,      //!< Memory segment identifier
            Fw::MemAllocator &allocator, //!< memory allocator. MUST be persistent for later deallocation.
            const BufferBins &bins       //!< Set of user bins
        );

        //! Destroy object BufferManager
        //!
        ~BufferManagerComponentImpl(void);

    PRIVATE :

        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for bufferSendIn
        //!
        void
        bufferSendIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Buffer &fwBuffer);

        //! Handler implementation for bufferGetCallee
        //!
        Fw::Buffer bufferGetCallee_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 size);

        bool m_setup;             //!< flag to indicate component has been setup
        NATIVE_UINT_TYPE m_mgrID; //!< stored manager ID for buffer checking

        BufferBins m_bufferBins; //!< copy of bins supplied by user

        struct AllocatedBuffer
        {
            Fw::Buffer buff; //!< Buffer class to give to user
            U8 *memory;      //!< pointer to memory buffer
            U32 size;        //!< size of the buffer
            bool allocated;  //!< this buffer has been allocated
        };

        AllocatedBuffer *m_buffers;    //!< pointer to allocated buffer space
        Fw::MemAllocator *m_allocator; //!< allocator for memory
        NATIVE_UINT_TYPE m_identifier; //!< identifier for allocator
        NATIVE_UINT_TYPE m_numStructs; //!< number of allocated structs
    };

} // end namespace Svc

#endif
