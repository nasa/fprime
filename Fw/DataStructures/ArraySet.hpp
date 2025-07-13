// ======================================================================
// \file   ArraySet.hpp
// \author bocchino
// \brief  An array-based set with internal storage
// ======================================================================

#ifndef Fw_ArraySet_HPP
#define Fw_ArraySet_HPP

#include "Fw/DataStructures/ExternalArraySet.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class ArraySet final : public SetBase<T> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT, FwSizeType CC>
    friend class ArraySetTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of animplementation entry
    using ImplEntry = SetOrMapImplEntry<T, Nil>;

    //! The type of a set entry
    using SetEntry = SetEntry<T>;

    //! The type of the implementation entries
    using ImplEntries = ImplEntry[C];

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ArraySet() : SetBase<T>(), m_extSet(m_entries, C) {}

    //! Copy constructor
    ArraySet(const ArraySet<T, C>& set) : SetBase<T>(), m_extSet(m_entries, C) { *this = set; }

    //! Destructor
    ~ArraySet() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ArraySet<T, C>& operator=(const ArraySet<T, C>& set) {
        this->m_extSet.copyDataFrom(set);
        return *this;
    }

    //! Clear the set
    void clear() override { this->m_extSet.clear(); }

    //! Find an element in the set
    //! \return SUCCESS if the element was found
    Success find(const T& element  //!< The element
    ) const override {
        return this->m_extSet.find(element);
    }

    //! Get the capacity of the set (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_extSet.getCapacity(); }

    //! Get the head set entry for the set
    //! \return The set entry
    const SetEntry* getHeadSetEntry() const override { return this->m_extSet.getHeadSetEntry(); }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const override { return this->m_extSet.getSize(); }

    //! Insert an element in the set
    //! \return SUCCESS if there is room in the set
    Success insert(const T& element  //!< The element
                   ) override {
        return this->m_extSet.insert(element);
    }

    //! Remove an element from the set
    //! \return SUCCESS if the key was there
    Success remove(const T& element  //!< The element
                   ) override {
        return this->m_extSet.remove(element);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The external set implementation
    ExternalArraySet<T> m_extSet = {};

    //! The array providing the backing memory for m_extSet
    ImplEntries m_entries = {};
};

}  // namespace Fw

#endif
