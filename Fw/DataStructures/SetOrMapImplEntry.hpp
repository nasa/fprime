// ======================================================================
// \title  SetOrMapImplEntry
// \author bocchino
// \brief  A class template representing an entry for a set or map implementation
// ======================================================================

#ifndef Fw_SetOrMapImplEntry_HPP
#define Fw_SetOrMapImplEntry_HPP

#include "Fw/DataStructures/MapEntry.hpp"
#include "Fw/DataStructures/SetEntry.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapImplEntry final : public MapEntry<KE, VN>, public SetEntry<KE> {
  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetOrMapImplEntry() : MapEntry<KE, VN>(), SetEntry<KE>() {}

    //! Constructor providing members
    SetOrMapImplEntry(const KE& keyOrElement,                      //!< The key or element
                  const VN& valueOrNil,                        //!< The value or Nil
                  const SetOrMapImplEntry<KE, VN>* next = nullptr  //!< The next entry
                  )
        : MapEntry<KE, VN>(), SetEntry<KE>(), m_keyOrElement(keyOrElement), m_valueOrNil(valueOrNil), m_next(next) {}

    //! Copy constructor
    SetOrMapImplEntry(const SetOrMapImplEntry<KE, VN>& entry) { *this = entry; }

    //! Destructor
    ~SetOrMapImplEntry() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    SetOrMapImplEntry<KE, VN>& operator=(const SetOrMapImplEntry<KE, VN>& entry) {
        if (this != &entry) {
            this->m_keyOrElement = entry.m_keyOrElement;
            this->m_valueOrNil = entry.m_valueOrNil;
            this->m_next = entry.m_next;
        }
        return *this;
    }

    //! Get the element associated with this entry
    //! \return The element
    const KE& getElement() const override { return this->m_keyOrElement; }

    //! Get the key associated with this entry
    //! \return The key
    const KE& getKey() const override { return this->m_keyOrElement; }

    // TODO
    const KE& getKeyOrelement() const { return this->m_keyOrElement; }

    //! Get the value associated with this entry
    //! \return The value
    const VN& getValue() const override { return this->m_valueOrNil; }

    // TODO
    const KE& getValueOrNil() const { return this->m_valueOrNil; }

    //! Get the next entry
    //! \return The entry, or nullptr if none
    const SetOrMapImplEntry<KE, VN>* getNextEntry() const { return this->m_next; }

    //! Get the next map entry
    //! \return The map entry, or nullptr if none
    const MapEntry<KE, VN>* getNextMapEntry() const override { return this->m_next; }

    //! Get the next set entry
    //! \return The set entry, or nullptr if none
    const SetEntry<KE>* getNextSetEntry() const override { return this->m_next; }

    //! Set the key or element
    void setKeyOrElement(const KE& keyOrElement  //!< The key or element
    ) {
        this->m_keyOrElement = keyOrElement;
    }

    //! Set the next entry
    void setNextEntry(const SetOrMapImplEntry<KE, VN>* next) { this->m_next = next; }

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

    //! Pointer to the next entry or nullptr if none
    const SetOrMapImplEntry<KE, VN>* m_next = nullptr;
};

}  // namespace Fw

#endif
