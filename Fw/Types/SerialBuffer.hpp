// ======================================================================
// \title  SerialBuffer.hpp
// \author bocchino
// \brief  hpp file for SerialBuffer type
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Fw_SerialBuffer_HPP
#define Fw_SerialBuffer_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include "Fw/Types/Serializable.hpp"

namespace Fw {

//! \class SerialBuffer
//! \brief A variable-length serializable buffer
//!
class SerialBuffer final : public SerializeBufferBase {
  public:
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Construct a SerialBuffer
    //!
    SerialBuffer(U8* const data,            //!< Pointer to the data
                 const FwSizeType capacity  //!< The buffer capacity
    );

  public:
    // ----------------------------------------------------------------------
    // Pure virtual methods from SerializeBufferBase
    // ----------------------------------------------------------------------

    FwSizeType getBuffCapacity() const;

    U8* getBuffAddr();

    const U8* getBuffAddr() const;

  public:
    // ----------------------------------------------------------------------
    // Public instance methods
    // ----------------------------------------------------------------------

    //! Fill the buffer to capacity with preexisting data
    void fill();

    //! Push n bytes onto the buffer
    SerializeStatus pushBytes(const U8* const addr,  //!< Address of bytes to push
                              const FwSizeType n     //!< Number of bytes
    );

    //! Pop n bytes off the buffer
    SerializeStatus popBytes(U8* const addr,  //!< Address of bytes to pop
                             FwSizeType n     //!< Number of bytes to pop
    );

  private:
    // ----------------------------------------------------------------------
    // Data
    // ----------------------------------------------------------------------

    //! The data
    U8* const m_data;

    //! The capacity
    const FwSizeType m_capacity;
};

}  // namespace Fw

#endif
