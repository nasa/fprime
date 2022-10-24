// ======================================================================
// \title  ArrayFIFOBuffer.cpp
// \author mereweth
// \brief  ArrayFIFOBuffer implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/BufferAccumulator/BufferAccumulator.hpp"
#include <FpConfig.hpp>

#include "Fw/Types/Assert.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Constructors
  // ----------------------------------------------------------------------

  BufferAccumulator::ArrayFIFOBuffer ::
    ArrayFIFOBuffer() :
      elements(nullptr),
      capacity(0),
      enqueueIndex(0),
      dequeueIndex(0),
      size(0)
  {

  }

  BufferAccumulator::ArrayFIFOBuffer ::
    ~ArrayFIFOBuffer()
  {

  }

  // ----------------------------------------------------------------------
  // Public functions
  // ----------------------------------------------------------------------

  void BufferAccumulator::ArrayFIFOBuffer ::
    init(Fw::Buffer *const elements, NATIVE_UINT_TYPE capacity)
  {
    this->elements = elements;
    this->capacity = capacity;
  }

  bool BufferAccumulator::ArrayFIFOBuffer ::
    enqueue(const Fw::Buffer& e)
  {
    if (this->elements == nullptr) {
      return false;
    }
    bool status;
    if (this->size < this->capacity) {
      // NOTE(mereweth) enqueueIndex is unsigned, no need to compare with 0
      FW_ASSERT(enqueueIndex < this->capacity, enqueueIndex);
      this->elements[this->enqueueIndex] = e;
      this->enqueueIndex = (this->enqueueIndex + 1) % this->capacity;
      status = true;
      ++this->size;
    }
    else {
      status = false;
    }
    return status;
  }

  bool BufferAccumulator::ArrayFIFOBuffer ::
    dequeue(Fw::Buffer& e)
  {
    if (this->elements == nullptr) {
      return false;
    }
    FW_ASSERT(this->elements);
    bool status;
    if (this->size > 0) {
      // NOTE(mereweth) dequeueIndex is unsigned, no need to compare with 0
      FW_ASSERT(dequeueIndex < this->capacity, enqueueIndex);
      e = this->elements[this->dequeueIndex];
      this->dequeueIndex = (this->dequeueIndex + 1) % this->capacity;
      --this->size;
      status = true;
    }
    else {
      status = false;
    }
    return status;
  }

  U32 BufferAccumulator::ArrayFIFOBuffer ::
    getSize() const
  {
    return this->size;
  }

  U32 BufferAccumulator::ArrayFIFOBuffer ::
    getCapacity() const
  {
    return this->capacity;
  }

}
