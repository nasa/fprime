// ====================================================================== 
// \title  BufferManager.hpp
// \author bocchino
// \brief  hpp file for BufferManager component implementation class
//
// \copyright
// Copyright 2015-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef BufferManager_HPP
#define BufferManager_HPP

#include "Svc/BufferManager/BufferManagerComponentAc.hpp"

namespace Svc {

  class BufferManager :
    public BufferManagerComponentBase
  {

    PRIVATE:

      // ---------------------------------------------------------------------- 
      // Warnings
      // ---------------------------------------------------------------------- 

      class Warnings {

        public:

          //! Status
          struct Status {

            typedef enum {
              //! Success
              SUCCESS,
              //! Store size exceeded
              STORE_SIZE_EXCEEDED,
              //! Too many buffers
              TOO_MANY_BUFFERS,
            } t;

          };

        PRIVATE:

          //! Whether we have emitted a warning
          struct State {

            //! Construct a State object
            State(void);

            //! StoreSizeExceeded
            bool storeSizeExceeded;

            //! TooManyBuffers
            bool tooManyBuffers;

          };

        public:

          //! Construct a Warnings object
          Warnings(
              BufferManager& bufferManager //!< The enclosing BufferManager
          );

        public:

          //! Update the warning state
          void update(
              const Status::t status //!< The status
          );

        PRIVATE:

          //! The enclosing BufferManager
          BufferManager& bufferManager;

          //! The warning state
          State state;

      };

    PRIVATE:

      // ----------------------------------------------------------------------
      // The store 
      // ----------------------------------------------------------------------

      class Store {

        public:

          // ----------------------------------------------------------------------
          // Construction and destruction 
          // ----------------------------------------------------------------------

          // Construct a Store
          Store(
              const U32 size
          );

          // Destroy a Store
          ~Store(void);

        public:

          // ----------------------------------------------------------------------
          // Types 
          // ----------------------------------------------------------------------

          typedef enum { SUCCESS, FAILURE } Status;

        public:

          // ----------------------------------------------------------------------
          // Methods 
          // ----------------------------------------------------------------------

          // Get the number of bytes currently allocated
          U32 getAllocatedSize(void) const;

          // Allocate n bytes on the store
          Status allocate(
              const U32 n,
              U8* &result
          );

          // Free bytes from the store
          void free(
              const U32 size, //!< The allocation size
              U8 *const address //!< The allocation address
          );

        PRIVATE:

          // ----------------------------------------------------------------------
          // Constants
          // ----------------------------------------------------------------------

          // The size of the store
          const U32 totalSize;

          // Pointer to the base of the store memory
          U8 *const memoryBase;

          // ----------------------------------------------------------------------
          // Variables
          // ----------------------------------------------------------------------
         
          //! Pointer to the first free byte of store memory
          U32 freeIndex;

          //! The amount of padding at the end of the store, added to allocations that otherwise would wrap around
          U32 padSize;

          //! The total allocated size on the store
          U32 allocatedSize;

      };

    PRIVATE:

      // ---------------------------------------------------------------------- 
      // The allocation queue
      // ---------------------------------------------------------------------- 

      class AllocationQueue {

        public:

          // ----------------------------------------------------------------------
          // Construction and destruction 
          // ----------------------------------------------------------------------

          // Construct an AllocationQueue
          AllocationQueue(const U32 size);

          // Destroy an AllocationQueue
          ~AllocationQueue(void);

        public:

          // ----------------------------------------------------------------------
          // Types 
          // ----------------------------------------------------------------------

          // Allocation status
          struct Allocate {

            typedef enum {
              SUCCESS, // Allocation OK
              FULL // No more room
            } Status;

          };

          // Free status
          struct Free {

            typedef enum {
              SUCCESS, // Free OK
              EMPTY, // Nothing to free
              ID_MISMATCH // ID supplied was not at head of queue
            } Status;

          };

          // An entry in the queue
          typedef struct {
            U32 id;
            U32 size;
          } Entry;

        public:

          // ----------------------------------------------------------------------
          // Public methods
          // ----------------------------------------------------------------------

          // Get the number of buffers currently allocated
          U32 getAllocationSize(void) const;
        
          // Record an allocation of size 'size' and generate a new id
          Allocate::Status allocate(
              const U32 size,
              U32& id
          );

          // Record a deallocation
          Free::Status free(
              const U32 expectedId,
              U32& sawId,
              U32& size
          );

        PRIVATE:

          // ----------------------------------------------------------------------
          // Private methods
          // ----------------------------------------------------------------------
        
          // Get the next ID
          U32 getNextId(void);

          // Update a circular index
          U32 getNextIndex(const U32 index);

        PRIVATE:

          // ----------------------------------------------------------------------
          // Constants
          // ---------------------------------------------------------------------- 

          // The queue size
          const U32 totalSize;

          // Pointer to the queue data
          Entry *const data;

        PRIVATE:

          // ----------------------------------------------------------------------
          // Variables
          // ---------------------------------------------------------------------- 

          // The next id
          U32 nextId;

          // Index into the allocation end of the queue
          U32 allocateIndex;

          // Index into the free end of the queue
          U32 freeIndex;

          // Number of elements allocated
          U32 allocationSize;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object BufferManager
      //!
      BufferManager(
          const char *const compName, //!< The component name
          const U32 storeSize,
          const U32 maxNumBuffers
      );

      //! Initialize object BufferManager
      //!
      void init(
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Destroy object BufferManager
      //!
      ~BufferManager(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler for input port bufferGetCallee
      //
      Fw::Buffer bufferGetCallee_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          U32 size 
      );

      //! Handler implementation for bufferSendIn
      //!
      void bufferSendIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer &buffer
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Variables 
      // ----------------------------------------------------------------------

      //! Warnings
      Warnings warnings;

      //! The store
      Store store;

      //! The allocation queue
      AllocationQueue allocationQueue;

    };

}

#endif
