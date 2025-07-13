// ======================================================================
// \title  ArraySetOrMapImplTester.hpp
// \author bocchino
// \brief  Class template for access to ArraySetOrMapImpl members
// ======================================================================

#ifndef ArraySetOrMapImplTester_HPP
#define ArraySetOrMapImplTester_HPP

#include <map>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename KE, typename VN>
class ArraySetOrMapImplTester {
  public:
    using Entry = SetOrMapImplEntry<KE, VN>;

    ArraySetOrMapImplTester<KE, VN>(const ArraySetOrMapImpl<KE, VN>& impl) : m_impl(impl) {}

    const ExternalArray<Entry>& getEntries() const { return this->m_impl.m_entries; }

  private:
    const ArraySetOrMapImpl<KE, VN>& m_impl;
};

}  // namespace Fw

#endif
