// ======================================================================
//
// \title  History.hpp
// \author R. Bocchino
// \brief  Header for a history of value items
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_SmHarness_History_HPP
#define FppTest_SmHarness_History_HPP

#include <FpConfig.hpp>
#include <array>

#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmHarness {

//! A history of value items
template <typename T, FwSizeType size>
class History {
  public:
    //! Constructor
    History() {}

    //! Clear the history
    void clear() { this->m_size = 0; }

    //! Check two histories for equality
    bool operator==(History& history  //!< The other history
    ) const {
        bool result = (this->m_size == history.m_size);
        if (result) {
            for (FwSizeType i = 0; i < this->m_size; i++) {
                if (this->m_items[i] != history.m_items[i]) {
                    result = false;
                    break;
                }
            }
        }
        return result;
    }

    //! Push an item on the history
    void push(const T& item  //!< The item
    ) {
        FW_ASSERT(m_size < size);
        this->m_items[m_size] = T(item);
        this->m_size++;
    }

    //! Get the history size
    FwSizeType getSize() const { return this->m_size; }

    //! Get the history item at an index
    const T& getItemAt(FwIndexType index  //!< The index
    ) const {
        FW_ASSERT(index < this->m_size);
        return this->m_items[index];
    }

  private:
    //! The history size
    FwSizeType m_size = 0;

    //! The items in the history
    std::array<T, size> m_items = {};
};

}  // namespace SmHarness

}  // end namespace FppTest

#endif
