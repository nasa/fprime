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

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Svc/BufferAccumulator/BufferAccumulator.hpp"
#include <new> // For placement new

namespace Svc {

// ----------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------

BufferAccumulator::ArrayFIFOBuffer ::ArrayFIFOBuffer()
    : m_elements(nullptr),
      m_capacity(0),
      m_enqueueIndex(0),
      m_dequeueIndex(0),
      m_size(0)
{
}

BufferAccumulator::ArrayFIFOBuffer ::~ArrayFIFOBuffer() {}

// ----------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------

void BufferAccumulator::ArrayFIFOBuffer ::init(Fw::Buffer* const elements,
                                               NATIVE_UINT_TYPE capacity) {
  this->m_elements = elements;
  this->m_capacity = capacity;

  // Construct all elements
  for (NATIVE_UINT_TYPE idx = 0; idx < capacity; idx++) {
      new (&this->m_elements[idx]) Fw::Buffer;
  }
}

bool BufferAccumulator::ArrayFIFOBuffer ::enqueue(const Fw::Buffer& e) {

  if (this->m_elements == nullptr) {
    return false;
  }

  bool status;
  if (this->m_size < this->m_capacity) {
    // enqueueIndex is unsigned, no need to compare with 0
    FW_ASSERT(m_enqueueIndex < this->m_capacity, static_cast<FwAssertArgType>(m_enqueueIndex));
    this->m_elements[this->m_enqueueIndex] = e;
    this->m_enqueueIndex = (this->m_enqueueIndex + 1) % this->m_capacity;
    status = true;
    this->m_size++;
  } else {
    status = false;
  }

  return status;
}

bool BufferAccumulator::ArrayFIFOBuffer ::dequeue(Fw::Buffer& e) {

  if (this->m_elements == nullptr) {
    return false;
  }

  FW_ASSERT(this->m_elements);
  bool status;

  if (this->m_size > 0) {
    // dequeueIndex is unsigned, no need to compare with 0
    FW_ASSERT(m_dequeueIndex < this->m_capacity, static_cast<FwAssertArgType>(m_dequeueIndex));
    e = this->m_elements[this->m_dequeueIndex];
    this->m_dequeueIndex = (this->m_dequeueIndex + 1) % this->m_capacity;
    this->m_size--;
    status = true;
  } else {
    status = false;
  }

  return status;
}

U32 BufferAccumulator::ArrayFIFOBuffer ::getSize() const {
  return this->m_size;
}

U32 BufferAccumulator::ArrayFIFOBuffer ::getCapacity() const {
  return this->m_capacity;
}

}  // namespace Svc
