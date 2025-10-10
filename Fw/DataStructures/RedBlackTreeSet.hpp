// ======================================================================
// \file   RedBlackTreeSet.hpp
// \author bocchino
// \brief  An array-based set with internal storage
// ======================================================================

#ifndef Fw_RedBlackTreeSet_HPP
#define Fw_RedBlackTreeSet_HPP

#include "Fw/DataStructures/ExternalRedBlackTreeSet.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class RedBlackTreeSet final : public SetBase<T> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(C > 0, "capacity must be greater than zero");

    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT, FwSizeType CC>
    friend class RedBlackTreeSetTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a const iterator
    using ConstIterator = SetConstIterator<T>;

    //! The type of a tree node
    using Node = typename RedBlackTreeSetOrMapImpl<T, Nil>::Node;

    //! The type of the tree node array
    using Nodes = Node[C];

    //! The type of a tree node index
    using Index = typename RedBlackTreeSetOrMapImpl<T, Nil>::Index;

    //! The type of the free node array
    using FreeNodes = Index[C];

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    RedBlackTreeSet() : SetBase<T>(), m_extSet(m_nodes, m_freeNodes, C) {}

    //! Copy constructor
    RedBlackTreeSet(const RedBlackTreeSet<T, C>& set) : SetBase<T>(), m_extSet(m_nodes, m_freeNodes, C) { *this = set; }

    //! Destructor
    ~RedBlackTreeSet() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    RedBlackTreeSet<T, C>& operator=(const RedBlackTreeSet<T, C>& set) {
        this->m_extSet.copyDataFrom(set);
        return *this;
    }

    //! Get the begin iterator
    //! \return The iterator
    ConstIterator begin() const override { return this->m_extSet.begin(); }

    //! Clear the set
    void clear() override { this->m_extSet.clear(); }

    //! Get the end iterator
    //! \return The iterator
    ConstIterator end() const override { return this->m_extSet.end(); }

    //! Find an element in the set
    //! \return SUCCESS if the element was found
    Success find(const T& element  //!< The element
    ) const override {
        return this->m_extSet.find(element);
    }

    //! Get the capacity of the set (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_extSet.getCapacity(); }

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

    //! The array for storing the tree nodes
    Nodes m_nodes = {};

    //! The array for storing the free node indices
    FreeNodes m_freeNodes = {};

    //! The external set implementation
    ExternalRedBlackTreeSet<T> m_extSet = {};
};

}  // namespace Fw

#endif
