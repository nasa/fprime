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

#include "Svc/StaticMemory/StaticMemoryComponentAc.hpp"
#include "config/StaticMemoryConfig.hpp"

namespace Svc {

class StaticMemoryComponentImpl final : public StaticMemoryComponentBase {
    friend class StaticMemoryTester;

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

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferDeallocate
    //!
    void bufferDeallocate_handler(const FwIndexType portNum, /*!< The port number*/
                                  Fw::Buffer& fwBuffer);

    //! Handler implementation for bufferAllocate
    //!
    Fw::Buffer bufferAllocate_handler(const FwIndexType portNum, /*!< The port number*/
                                      FwSizeType size);

    bool m_allocated[NUM_BUFFERALLOCATE_INPUT_PORTS];
    U8 m_static_memory[NUM_BUFFERALLOCATE_INPUT_PORTS][STATIC_MEMORY_ALLOCATION_SIZE];
};

}  // end namespace Svc

#endif
