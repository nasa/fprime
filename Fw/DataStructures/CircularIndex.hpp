// ======================================================================
// \file   CircularIndex.hpp
// \author bocchino
// \brief  An index value that wraps around modulo an integer
// ======================================================================

#ifndef Fw_CircularIndex_HPP
#define Fw_CircularIndex_HPP

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

class CircularIndex final {
  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    CircularIndex() : m_value(0), m_modulus(1) {}

    //! Constructor with specified members
    explicit CircularIndex(FwSizeType modulus,   //!< The modulus
                           FwSizeType value = 0  //!< The initial value
                           )
        : m_modulus(modulus) {
        FW_ASSERT(modulus > 0);
        this->setValue(value);
    }

    //! Copy constructor
    CircularIndex(const CircularIndex& ci) { *this = ci; }

    //! Destructor
    ~CircularIndex() = default;

  public:
    // ----------------------------------------------------------------------
    // Public functions
    // ----------------------------------------------------------------------

    //! operator=
    CircularIndex& operator=(const CircularIndex& ci) {
        if (this != &ci) {
            this->m_value = ci.m_value;
            this->m_modulus = ci.m_modulus;
        }
        return *this;
    }

    //! Get the index value
    //! \return The index value
    FwSizeType getValue() const {
        FW_ASSERT(this->m_value < this->m_modulus);
        return this->m_value;
    }

    //! Set the index value
    void setValue(FwSizeType value  //!< The index value
    ) {
        FW_ASSERT(this->m_modulus > 0);
        this->m_value = value % this->m_modulus;
    }

    //! Get the modulus
    FwSizeType getModulus() const {
        FW_ASSERT(this->m_value < this->m_modulus);
        return this->m_modulus;
    }

    //! Set the modulus
    void setModulus(FwSizeType modulus  //!< The modulus value
    ) {
        this->m_modulus = modulus;
        this->setValue(this->m_value);
    }

    //! Increment the index value
    //! \return The new value
    FwSizeType increment(FwSizeType amount = 1  //!< The amount by which to increment
    ) {
        FW_ASSERT(this->m_modulus > 0);
        const FwSizeType offset = amount % m_modulus;
        this->setValue(this->m_value + offset);
        return this->m_value;
    }

    //! Decrement the index value
    //! \return The new value
    FwSizeType decrement(FwSizeType amount = 1  //!< The amount by which to decrement
    ) {
        FW_ASSERT(this->m_modulus > 0);
        const FwSizeType offset = amount % this->m_modulus;
        this->setValue(this->m_value + this->m_modulus - offset);
        return this->m_value;
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The index value
    FwSizeType m_value;

    //! The modulus
    FwSizeType m_modulus;
};

}  // namespace Fw

#endif
