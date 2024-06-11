// ======================================================================
// \title  AbstractState.hpp
// \author Rob Bocchino
// \brief  Header file for abstract state
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_AbstractState_HPP
#define Svc_AbstractState_HPP

#include <cstring>

#include "Fw/Types/Assert.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/DpManager.hpp"
#include "TestUtils/OnChangeChannel.hpp"
#include "TestUtils/Option.hpp"

namespace Svc {

class AbstractState {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The minimum buffer size
    static constexpr FwSizeType MIN_BUFFER_SIZE = 1;

    //! The maximum buffer size
    static constexpr FwSizeType MAX_BUFFER_SIZE = 1024;

  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type of the buffer get status
    enum class BufferGetStatus {
        //! Valid
        VALID,
        //! Invalid
        INVALID
    };

  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Construct an AbstractState object
    AbstractState()
        : bufferSizeOpt(),
          bufferGetStatus(BufferGetStatus::VALID),
          NumSuccessfulAllocations(0),
          NumFailedAllocations(0),
          NumDataProducts(0),
          NumBytes(0),
          bufferGetOutPortNumOpt(),
          productResponseOutPortNumOpt(),
          productSendOutPortNumOpt(),
          bufferAllocationFailedEventCount(0) {}

  public:
    // ----------------------------------------------------------------------
    // Accessor methods
    // ----------------------------------------------------------------------

    //! Get the buffer size
    FwSizeType getBufferSize() const {
        return this->bufferSizeOpt.getOrElse(STest::Pick::lowerUpper(MIN_BUFFER_SIZE, MAX_BUFFER_SIZE));
    }

    //! Set the buffer size
    void setBufferSize(FwSizeType bufferSize) { this->bufferSizeOpt.set(bufferSize); }

  private:
    // ----------------------------------------------------------------------
    // Private state variables
    // ----------------------------------------------------------------------

    //! The current buffer size
    TestUtils::Option<FwSizeType> bufferSizeOpt;

  public:
    // ----------------------------------------------------------------------
    // Public state variables
    // ----------------------------------------------------------------------

    //! The buffer get status
    BufferGetStatus bufferGetStatus;

    //! The number of successful buffer allocations
    TestUtils::OnChangeChannel<U32> NumSuccessfulAllocations;

    //! The number of failed buffer allocations
    TestUtils::OnChangeChannel<U32> NumFailedAllocations;

    //! The number of data products handled
    TestUtils::OnChangeChannel<U32> NumDataProducts;

    //! The number of bytes handled
    TestUtils::OnChangeChannel<U64> NumBytes;

    //! Data for buffers
    U8 bufferData[MAX_BUFFER_SIZE];

    //! The last port number used for bufferGetOut
    TestUtils::Option<FwIndexType> bufferGetOutPortNumOpt;

    //! The last port number used for productResponseOut
    TestUtils::Option<FwIndexType> productResponseOutPortNumOpt;

    //! The last port number used for productSendOut
    TestUtils::Option<FwIndexType> productSendOutPortNumOpt;

    //! The number of buffer allocation failed events since the last throttle clear
    FwSizeType bufferAllocationFailedEventCount;
};

}  // namespace Svc

#endif
