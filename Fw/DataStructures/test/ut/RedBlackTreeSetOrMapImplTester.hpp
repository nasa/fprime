// ======================================================================
// \title  RedBlackTreeSetOrMapImplTester.hpp
// \author bocchino
// \brief  Class template for access to RedBlackTreeSetOrMapImpl members
// ======================================================================

#ifndef RedBlackTreeSetOrMapImplTester_HPP
#define RedBlackTreeSetOrMapImplTester_HPP

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename KE, typename VN>
class RedBlackTreeSetOrMapImplTester {
  public:
    using Node = typename RedBlackTreeSetOrMapImpl<KE, VN>::Node;

    using FreeNode = typename RedBlackTreeSetOrMapImpl<KE, VN>::FreeNode;

    using Index = typename RedBlackTreeSetOrMapImpl<KE, VN>::Index;

    RedBlackTreeSetOrMapImplTester<KE, VN>(const RedBlackTreeSetOrMapImpl<KE, VN>& impl) : m_impl(impl) {}

    const ExternalArray<Node>& getNodes() const { return this->m_impl.m_nodes; }

    const ExternalArray<FreeNode>& getFreeNodes() const { return this->m_impl.m_freeNodes; }

    Index getRoot() const { return this->m_impl.m_root; }

  private:
    const RedBlackTreeSetOrMapImpl<KE, VN>& m_impl;
};

}  // namespace Fw

#endif
