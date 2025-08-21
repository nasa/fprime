// ======================================================================
// \file   ExternalArraySet.hpp
// \author bocchino
// \brief  An array-based set with external storage
// ======================================================================

#ifndef Fw_ExternalArraySet_HPP
#define Fw_ExternalArraySet_HPP

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/Nil.hpp"
#include "Fw/DataStructures/SetBase.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

template <typename T>
class ExternalArraySet final : public SetBase<T> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT>
    friend class ExternalArraySetTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a const iterator
    using ConstIterator = SetConstIterator<T>;

    //! The type of a set entry
    using Entry = SetOrMapImplEntry<T, Nil>;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalArraySet() = default;

    //! Constructor providing typed backing storage.
    //! entries must point to at least capacity elements of type ImplEntry.
    ExternalArraySet(Entry* entries,      //!< The entries
                     FwSizeType capacity  //!< The capacity
                     )
        : SetBase<T>() {
        this->setStorage(entries, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    ExternalArraySet(ByteArray data,      //!< The data,
                     FwSizeType capacity  //!< The capacity
                     )
        : SetBase<T>() {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ExternalArraySet(const ExternalArraySet<T>& set) : SetBase<T>() { *this = set; }

    //! Destructor
    ~ExternalArraySet() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ExternalArraySet<T>& operator=(const ExternalArraySet<T>& set) {
        if (&set != this) {
            this->m_impl = set.m_impl;
        }
        return *this;
    }

    //! Get the begin iterator
    //! \return The iterator
    ConstIterator begin() const override { return ConstIterator(this->m_impl.begin()); }

    //! Clear the set
    void clear() override { this->m_impl.clear(); }

    //! Get the end iterator
    //! \return The iterator
    ConstIterator end() const override { return ConstIterator(this->m_impl.end()); }

    //! Find a value associated with an element in the set
    //! \return SUCCESS if the item was found
    Success find(const T& element  //!< The element
    ) const override {
        Nil nil = {};
        return this->m_impl.find(element, nil);
    }

    //! Get the capacity of the set (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_impl.getCapacity(); }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const override { return this->m_impl.getSize(); }

    //! Insert an element in the set
    //! \return SUCCESS if there is room in the set
    Success insert(const T& element  //!< The element
                   ) override {
        return this->m_impl.insert(element, Nil());
    }

    //! Remove an element from the set
    //! \return SUCCESS if the element was there
    Success remove(const T& element  //!< The element
                   ) override {
        Nil nil = {};
        return this->m_impl.remove(element, nil);
    }

    //! Set the backing storage (typed data)
    //! entries must point to at least capacity elements of type ImplEntry.
    void setStorage(Entry* entries,      //!< The entries
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(entries, capacity);
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(data, capacity);
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ArraySetOrMapImpl
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ArraySetOrMapImpl<T, Nil>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalArray of the specified capacity,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return ArraySetOrMapImpl<T, Nil>::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The set implementation
    ArraySetOrMapImpl<T, Nil> m_impl = {};
};

}  // namespace Fw

#endif
