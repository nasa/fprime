// ======================================================================
// \title  StaticMemoryComponentImpl.hpp
// \author mstarch
// \brief  hpp file for StaticMemory component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef StaticMemory_HPP
#define StaticMemory_HPP

#include "StaticMemoryConfig.hpp"
#include "Svc/StaticMemory/StaticMemoryComponentAc.hpp"

namespace Svc {

class StaticMemoryComponentImpl : public StaticMemoryComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object StaticMemory
    //!
    StaticMemoryComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Destroy object StaticMemory
    //!
    ~StaticMemoryComponentImpl();

  PRIVATE:

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferDeallocate
    //!
    void bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                  Fw::Buffer& fwBuffer);

    //! Handler implementation for bufferAllocate
    //!
    Fw::Buffer bufferAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                      U32 size);

    bool m_allocated[NUM_BUFFERALLOCATE_INPUT_PORTS];
    U8 m_static_memory[NUM_BUFFERALLOCATE_INPUT_PORTS][STATIC_MEMORY_ALLOCATION_SIZE];
};

}  // end namespace Svc

#endif
