// ======================================================================
// \title  SetOrMapEntry
// \author bocchino
// \brief  A class template representing an iterator for a set or map
// ======================================================================

#ifndef Fw_SetOrMapEntry_HPP
#define Fw_SetOrMapEntry_HPP

#include "Fw/DataStructures/MapEntry.hpp"
#include "Fw/DataStructures/SetEntry.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapEntry final : public MapEntry<KE, VN>, public SetEntry<KE> {
  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetOrMapEntry() : MapEntry<KE, VN>() {}

    //! Constructor providing members
    SetOrMapEntry(const KE& keyOrElement,                         //!< The key or element
                     const VN& valueOrNil,                           //!< The value or Nil
                     const SetOrMapEntry<KE, VN>* next = nullptr  //!< The next iterator
                     )
        : m_keyOrElement(keyOrElement), m_valueOrNil(valueOrNil), m_next(next) {}

    //! Copy constructor
    SetOrMapEntry(const SetOrMapEntry<KE, VN>& iterator) { *this = iterator; }

    //! Destructor
    ~SetOrMapEntry() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    SetOrMapEntry<KE, VN>& operator=(const SetOrMapEntry<KE, VN>& iterator) {
        if (this != &iterator) {
            this->m_keyOrElement = iterator.m_keyOrElement;
            this->m_valueOrNil = iterator.m_valueOrNil;
            this->m_next = iterator.m_next;
        }
        return *this;
    }

    //! Get the element associated with this iterator
    //! \return The element
    const KE& getElement() const override { return this->m_keyOrElement; }

    //! Get the key associated with this iterator
    //! \return The key
    const KE& getKey() const override { return this->m_keyOrElement; }

    //! Get the value associated with this iterator
    //! \return The value
    const VN& getValue() const override { return this->m_valueOrNil; }

    //! Get the next iterator
    //! \return The iterator, or nullptr if none
    const SetOrMapEntry<KE, VN>* getNextIterator() const { return this->m_next; }

    //! Get the next map iterator
    //! \return The map iterator, or nullptr if none
    const MapEntry<KE, VN>* getNextMapEntry() const override { return this->m_next; }

    //! Get the next set iterator
    //! \return The set iterator, or nullptr if none
    const SetEntry<KE>* getNextSetEntry() const override { return this->m_next; }

    //! Set the key or element
    void setKeyOrElement(const KE& keyOrElement  //!< The key or element
    ) {
        this->m_keyOrElement = keyOrElement;
    }

    //! Set the next iterator
    void setNextIterator(const SetOrMapEntry<KE, VN>* next) { this->m_next = next; }

    //! Set the value or Nil
    void setValueOrNil(const VN& valueOrNil) { this->m_valueOrNil = valueOrNil; }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The map key or set element
    KE m_keyOrElement = {};

    //! The value or nil
    VN m_valueOrNil = {};

    //! Pointer to the next iterator or nullptr if none
    const SetOrMapEntry<KE, VN>* m_next = nullptr;
};

}  // namespace Fw

#endif
