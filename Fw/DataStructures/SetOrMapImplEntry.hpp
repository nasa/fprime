// ======================================================================
// \title  SetOrMapImplEntry
// \author bocchino
// \brief  A class template representing an entry for a set or map implementation
// ======================================================================

#ifndef Fw_SetOrMapImplEntry_HPP
#define Fw_SetOrMapImplEntry_HPP

#include "Fw/DataStructures/MapEntryBase.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapImplEntry final : public MapEntryBase<KE, VN> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(std::is_default_constructible<KE>::value, "key must be default constructible");
    static_assert(std::is_assignable<KE&, KE>::value, "key or element must be assignable");
    static_assert(std::is_default_constructible<VN>::value, "value must be default constructible");
    static_assert(std::is_assignable<VN&, VN>::value, "value must be assignable");

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetOrMapImplEntry() : MapEntryBase<KE, VN>() {}

    //! Constructor providing members
    SetOrMapImplEntry(const KE& keyOrElement,  //!< The key or element
                      const VN& valueOrNil     //!< The value or Nil
                      )
        : MapEntryBase<KE, VN>(), m_keyOrElement(keyOrElement), m_valueOrNil(valueOrNil) {}

    //! Copy constructor
    SetOrMapImplEntry(const SetOrMapImplEntry<KE, VN>& entry) : MapEntryBase<KE, VN>() { *this = entry; }

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
        }
        return *this;
    }

    //! Get the key or element associated with this entry
    //! \return The key or element
    const KE& getKeyOrElement() const { return this->m_keyOrElement; }

    //! Get the value or nil associated with this entry
    //! \return The value or nil
    const VN& getValueOrNil() const { return this->m_valueOrNil; }

    //! Set the key or element
    void setKeyOrElement(const KE& keyOrElement  //!< The key or element
    ) {
        this->m_keyOrElement = keyOrElement;
    }

    //! Set the value or Nil
    void setValueOrNil(const VN& valueOrNil) { this->m_valueOrNil = valueOrNil; }

  public:
    // ----------------------------------------------------------------------
    // MapEntryBase implementation
    // ----------------------------------------------------------------------

    //! Get the key associated with this entry
    //! \return The key
    const KE& getKey() const override { return this->m_keyOrElement; }

    //! Get the value associated with this entry
    //! \return The value
    const VN& getValue() const override { return this->m_valueOrNil; }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The map key or set element
    KE m_keyOrElement = {};

    //! The value or nil
    VN m_valueOrNil = {};
};

}  // namespace Fw

#endif
