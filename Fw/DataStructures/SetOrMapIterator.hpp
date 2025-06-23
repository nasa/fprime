// ======================================================================
// \title  SetOrMapIterator
// \author bocchino
// \brief  A class template representing an iterator for a set or map
// ======================================================================

#ifndef Fw_SetOrMapIterator_HPP
#define Fw_SetOrMapIterator_HPP

#include "Fw/DataStructures/MapIterator.hpp"
#include "Fw/DataStructures/SetIterator.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapIterator final : public MapIterator<KE, VN>, public SetIterator<KE> {
  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetOrMapIterator() = default;

    //! Constructor providing members
    SetOrMapIterator(const KE& keyOrElement,                         //!< The key or element
                     const VN& valueOrNil,                           //!< The value or Nil
                     const SetOrMapIterator<KE, VN>* next = nullptr  //!< The next iterator
                     )
        : m_keyOrElement(keyOrElement), m_valueOrNil(valueOrNil), m_next(next) {}

    //! Copy constructor
    SetOrMapIterator(const SetOrMapIterator<KE, VN>& iterator) {
      *this = iterator;
    }

    //! Destructor
    ~SetOrMapIterator() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    SetOrMapIterator<KE, VN>& operator=(const SetOrMapIterator<KE, VN>& iterator) {
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
    const SetOrMapIterator<KE, VN>* getNextIterator() const { return this->m_next; }

    //! Get the next map iterator
    //! \return The map iterator, or nullptr if none
    const MapIterator<KE, VN>* getNextMapIterator() const override { return this->m_next; }

    //! Get the next set iterator
    //! \return The set iterator, or nullptr if none
    const SetIterator<KE>* getNextSetIterator() const override { return this->m_next; }

    //! Set the key or element
    void setKeyOrElement(const KE& keyOrElement  //!< The key or element
    ) {
        this->m_keyOrElement = keyOrElement;
    }

    //! Set the next iterator
    void setNextIterator(const SetOrMapIterator<KE, VN>* next) { this->m_next = next; }

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
    const SetOrMapIterator<KE, VN>* m_next = nullptr;
};

}  // namespace Fw

#endif
