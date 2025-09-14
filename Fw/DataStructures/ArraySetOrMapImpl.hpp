// ======================================================================
// \title  ArraySetOrMapImpl
// \author bocchino
// \brief  An array-based implementation of a set or map
// ======================================================================

#ifndef Fw_ArraySetOrMapImpl_HPP
#define Fw_ArraySetOrMapImpl_HPP

#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/DataStructures/SetOrMapImplConstIterator.hpp"
#include "Fw/DataStructures/SetOrMapImplEntry.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename KE, typename VN>
class ArraySetOrMapImpl final {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV>
    friend class ArraySetOrMapImplTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of an entry in the set or map
    using Entry = SetOrMapImplEntry<KE, VN>;

    //! Const iterator
    class ConstIterator final : public SetOrMapImplConstIterator<KE, VN> {
      public:
        using ImplKind = typename SetOrMapImplConstIterator<KE, VN>::ImplKind;

      public:
        //! Default constructor
        ConstIterator() {}

        //! Constructor providing the implementation
        ConstIterator(const ArraySetOrMapImpl<KE, VN>& impl) : SetOrMapImplConstIterator<KE, VN>(), m_impl(&impl) {}

        //! Copy constructor
        ConstIterator(const ConstIterator& it)
            : SetOrMapImplConstIterator<KE, VN>(), m_impl(it.m_impl), m_index(it.m_index) {}

        //! Destructor
        ~ConstIterator() override = default;

      public:
        //! Copy assignment operator
        ConstIterator& operator=(const ConstIterator& it) {
            this->m_impl = it.m_impl;
            this->m_index = it.m_index;
            return *this;
        }

        //! Equality comparison operator
        bool compareEqual(const ConstIterator& it) const {
            bool result = false;
            if ((this->m_impl == nullptr) && (it.m_impl == nullptr)) {
                result = true;
            } else if (this->m_impl == it.m_impl) {
                result |= (this->m_index == it.m_index);
                result |= (!this->isInRange() and !it.isInRange());
            }
            return result;
        }

        //! Return the impl kind
        //! \return The impl kind
        ImplKind implKind() const override { return ImplKind::ARRAY; }

        //! Get the set or map impl entry pointed to by this iterator
        //! \return The set or map impl entry
        const Entry& getEntry() const override {
            FW_ASSERT(this->m_impl != nullptr);
            FW_ASSERT(this->isInRange(), static_cast<FwAssertArgType>(this->m_index),
                      static_cast<FwAssertArgType>(this->m_impl->m_size));
            return this->m_impl->m_entries[this->m_index];
        }

        //! Increment operator
        void increment() override {
            if (this->isInRange()) {
                this->m_index++;
            }
        }

        //! Check whether the iterator is in range
        bool isInRange() const override {
            FW_ASSERT(this->m_impl != nullptr);
            return this->m_index < this->m_impl->m_size;
        }

        //! Set the iterator to the end value
        void setToEnd() {
            FW_ASSERT(this->m_impl != nullptr);
            this->m_index = this->m_impl->m_size;
        }

      private:
        //! The implementation over which to iterate
        const ArraySetOrMapImpl<KE, VN>* m_impl = nullptr;

        //! The current iteration index
        FwSizeType m_index = 0;
    };

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
    ArraySetOrMapImpl(ByteArray data,      //!< The data
                      FwSizeType capacity  //!< The capacity
    ) {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ArraySetOrMapImpl(const ArraySetOrMapImpl<KE, VN>& impl) { *this = impl; }

    //! Destructor
    ~ArraySetOrMapImpl() = default;

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

    //! Get the begin iterator
    ConstIterator begin() const { return ConstIterator(*this); }

    //! Clear the set or map
    void clear() { this->m_size = 0; }

    //! Get the end iterator
    ConstIterator end() const {
        auto it = begin();
        it.setToEnd();
        return it;
    }

    //! Find a value associated with a key in the map or an element in a set
    //! \return SUCCESS if the item was found
    Success find(const KE& keyOrElement,  //!< The key or element
                 VN& valueOrNil           //!< The value or Nil
    ) const {
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
            auto& e = this->m_entries[i];
            if (e.getKey() == keyOrElement) {
                e.setValueOrNil(valueOrNil);
                status = Success::SUCCESS;
                break;
            }
        }
        if ((status == Success::FAILURE) && (this->m_size < this->getCapacity())) {
            this->m_entries[this->m_size] = Entry(keyOrElement, valueOrNil);
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
    void setStorage(ByteArray data,      //!< The data
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
