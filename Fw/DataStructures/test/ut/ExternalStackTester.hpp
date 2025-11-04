// ======================================================================
// \title  ExternalStackTester.hpp
// \author bocchino
// \brief  Tester class for ExternalStack
// ======================================================================

#ifndef Fw_ExternalStackTester_HPP
#define Fw_ExternalStackTester_HPP

#include "Fw/DataStructures/ExternalStack.hpp"

namespace Fw {

template <typename T>
class ExternalStackTester {
  public:
    ExternalStackTester<T>(const ExternalStack<T>& stack) : m_stack(stack) {}

    const ExternalArray<T> getItems() const { return this->m_stack.m_items; }

  private:
    const ExternalStack<T>& m_stack;
};

}  // namespace Fw

#endif
