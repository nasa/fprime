// ======================================================================
// \title  ArraySetOrMapImpl
// \author bocchino
// \brief  An array-based implementation of a set or map
// ======================================================================

#ifndef Fw_ArraySetOrMapImpl_HPP
#define Fw_ArraySetOrMapImpl_HPP

#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/DataStructures/SetOrMapIterator.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename KE, typename VN>
class ArraySetOrMapImpl {
  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of an entry in the set or map
    using Entry = SetOrMapIterator<KE, VN>;

    //! The type of a set or map iterator
    using Iterator = SetOrMapIterator<KE, VN>;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ArraySetOrMapImpl() = default;

    //! Constructor providing typed backing storage.
    //! entries must point to at least capacity elements of type Entry.
    ArraySetOrMapImpl(Entry* entries,      //!< The entries
                      FwSizeType capacity  //!< The capacity
    ) {
        this->setStorage(entries, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    ArraySetOrMapImpl(ByteArray data,      //!< The data,
                      FwSizeType capacity  //!< The capacity
    ) {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ArraySetOrMapImpl(const ArraySetOrMapImpl<KE, VN>& map) { *this = map; }

    //! Destructor
    virtual ~ArraySetOrMapImpl() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ArraySetOrMapImpl<KE, VN>& operator=(const ArraySetOrMapImpl<KE, VN>& impl) {
        if (&impl != this) {
            m_entries = impl.m_entries;
            m_size = impl.m_size;
        }
        return *this;
    }

    //! Get an iterator at an index in the array.
    //! Fails an assertion if the index is out of range for the set or map.
    //! \return The iterator
    const Iterator& at(FwSizeType index  //!< The index
    ) const {
        FW_ASSERT(index < this->m_size, static_cast<FwSizeType>(index), static_cast<FwSizeType>(this->m_size));
        return this->m_entries[index];
    }

    //! Clear the set or map
    void clear() { this->m_size = 0; }

    //! Find a value associated with a key in the map or an element in a set
    //! \return SUCCESS if the item was found
    Success find(const KE& keyOrElement,  //!< The key or element
                 VN& valueOrNil           //!< The value or Nil
    ) {
        auto status = Success::FAILURE;
        for (FwSizeType i = 0; i < this->m_size; i++) {
            const auto& e = this->m_entries[i];
            if (e.getKey() == keyOrElement) {
                valueOrNil = e.getValue();
                status = Success::SUCCESS;
                break;
            }
        }
        return status;
    }

    //! Get the capacity of the set or map (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const { return this->m_entries.getSize(); }

    //! Get the head iterator for the set or map
    //! \return The iterator
    const Iterator* getHeadIterator() const {
        Iterator* result = nullptr;
        if (this->m_size > 0) {
            result = &this->m_entries[0];
        }
        return result;
    }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const { return this->m_size; }

    //! Insert an element in the set or a (key, value) pair in the map
    //! \return SUCCESS if there is room in the set or map
    Success insert(const KE& keyOrElement,  //!< The key or element
                   const VN& valueOrNil     //!< The value or Nil
    ) {
        auto status = Success::FAILURE;
        for (FwSizeType i = 0; i < this->m_size; i++) {
            const auto& e = this->m_entries[i];
            if (e.getKey() == keyOrElement) {
                e.setValue(valueOrNil);
                status = Success::SUCCESS;
                break;
            }
        }
        if ((status == Success::FAILURE) && (this->m_size < this->getCapacity())) {
            this->m_entries[this->m_size] = Iterator(keyOrElement, valueOrNil);
            if (this->m_size > 0) {
                this->m_entries[this->m_size - 1].setNextIterator(&this->m_entries[this->m_size]);
            }
            this->m_size++;
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Remove an element from the set or a (key, value) pair from the map
    //! \return SUCCESS if the key or element was there
    Success remove(const KE& keyOrElement,  //!< The key or element
                   VN& valueOrNil           //!< The value or Nil
    ) {
        auto status = Success::FAILURE;
        for (FwSizeType i = 0; i < this->m_size; i++) {
            if (this->m_entries[i].getKey() == keyOrElement) {
                valueOrNil = this->m_entries[i].getValue();
                if (i < this->m_size - 1) {
                    this->m_entries[i] = this->m_entries[this->m_size - 1];
                    this->m_entries[i].setNextIterator(&this->m_entries[i + 1]);
                } else {
                    this->m_entries[i].setNextIterator(nullptr);
                }
                this->m_size--;
                status = Success::SUCCESS;
                break;
            }
        }
        return status;
    }

    //! Set the backing storage (typed data)
    //! entries must point to at least capacity elements of type Entry.
    void setStorage(Entry* entries,      //!< The entries
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_entries.setStorage(entries, capacity);
        this->clear();
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< THe data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_entries.setStorage(data, capacity);
        this->clear();
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ArraySetOrMapImpl
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ExternalArray<Entry>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalArray of the specified capacity,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return ExternalArray<Entry>::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array for storing the set or map entries
    ExternalArray<Entry> m_entries = {};

    //! The number of entries in the set or map
    FwSizeType m_size = 0;
};

}  // namespace Fw

#endif
