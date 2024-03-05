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
#include "BufferManagerComponentImplCfg.hpp"

namespace Svc
{

    // To use the class, instantiate an instance of the BufferBins struct below. This
    // table specifies N buffers of M size per bin. Up to MAX_NUM_BINS bins can be specified.
    // The table is copied when setup() is called, so it does not need to be retained after
    // the call.
    //
    // The rules for specifying bins:
    // 1. For each bin (BufferBins.bins[n]), specify the size of the buffers (bufferSize) in the
    //    bin and how many buffers for that bin (numBuffers).
    // 2. The bins should be ordered based on an increasing bufferSize to allow BufferManager to
    //    search for available buffers. When receiving a request for a buffer, the component will
    //    search for the first buffer from the bins that is equal to or greater
    //    than the requested size, starting at the beginning of the table.
    // 3. Any unused bins should have numBuffers set to 0.
    // 4. A single bin can be specified if a single size is needed.
    //
    // If a buffer is requested that can't be found among available buffers, the call will
    //    return an Fw::Buffer with a size of zero. It is expected that the user will notice
    //    and have the appropriate response for the design. If an empty buffer is returned to
    //    the BufferManager instance, a warning event will be issued but no other action will
    //    be taken.
    //
    // Buffer manager will assert under the following conditions:
    // 1. A returned buffer has the incorrect manager ID.
    // 2. A returned buffer has an incorrect buffer ID.
    // 3. A returned buffer is returned with a correct buffer ID, but it isn't already allocated.
    // 4. A returned buffer has an indicated size larger than originally allocated.
    // 5. A returned buffer has a pointer different than the one originally allocated.
    //
    // Note that a pointer to the Fw::MemAllocator used in setup() is stored for later memory cleanup.
    // The instance of the allocator must persist beyond calling the cleanup() function or the
    // destructor of BufferManager if cleanup() is not called. If a project-specific manual memory
    // allocator is not needed, Fw::MallocAllocator can be used.

    class BufferManagerComponentImpl : public BufferManagerComponentBase
    {

    public:

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
            BufferBin bins[BUFFERMGR_MAX_NUM_BINS]; //!< set of bins to define buffers
        };

        //! set up configuration

        void setup(
            NATIVE_UINT_TYPE mgrID,      //!< ID of manager for buffer checking
            NATIVE_UINT_TYPE memID,      //!< Memory segment identifier
            Fw::MemAllocator &allocator, //!< memory allocator. MUST be persistent for later deallocation.
                                         //!  MUST persist past destructor if cleanup() not called explicitly.
            const BufferBins &bins       //!< Set of user bins
        );

        void cleanup();              // Free memory prior to end of program if desired. Otherwise,
                                         // will be deleted in destructor

        //! Destroy object BufferManager
        //!
        ~BufferManagerComponentImpl();

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

        //! Handler implementation for schedIn
        //!
        void schedIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 context /*!< The call order*/
        );


        bool m_setup;             //!< flag to indicate component has been setup
        bool m_cleaned;           //!< flag to indicate memory has been cleaned up
        NATIVE_UINT_TYPE m_mgrId; //!< stored manager ID for buffer checking

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
        NATIVE_UINT_TYPE m_memId; //!< identifier for allocator
        NATIVE_UINT_TYPE m_numStructs; //!< number of allocated structs

        // stats
        U32 m_highWater; //!< high watermark for allocations
        U32 m_currBuffs; //!< number of currently allocated buffers
        U32 m_noBuffs; //!< number of failures to allocate a buffer
        U32 m_emptyBuffs; //!< number of empty buffers returned
    };

} // end namespace Svc

#endif
