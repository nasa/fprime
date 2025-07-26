// ======================================================================
// \title  RedBlackTreeSetOrMapImplTester.hpp
// \author bocchino
// \brief  Class template for access to RedBlackTreeSetOrMapImpl members
// ======================================================================

#ifndef RedBlackTreeSetOrMapImplTester_HPP
#define RedBlackTreeSetOrMapImplTester_HPP

#include <gtest/gtest.h>

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename KE, typename VN>
class RedBlackTreeSetOrMapImplTester {
  public:
    using Impl = RedBlackTreeSetOrMapImpl<KE, VN>;

    using Color = typename Impl::Color;

    using Direction = typename Impl::Direction;

    using FreeNode = typename Impl::FreeNode;

    using Index = typename Impl::Index;

    using Node = typename Impl::Node;

    RedBlackTreeSetOrMapImplTester<KE, VN>(const Impl& impl) : m_impl(impl) {
        const auto capacity = this->m_impl.getCapacity();
        this->blackHeights.setStorage(new FwSizeType[capacity], capacity);
    }

    ~RedBlackTreeSetOrMapImplTester<KE, VN>() {
        auto* const elements = this->blackHeights.getElements();
        if (elements != nullptr) {
            delete[] elements;
        }
    }

    const ExternalArray<Node>& getNodes() const { return this->m_impl.m_nodes; }

    const ExternalArray<FreeNode>& getFreeNodes() const { return this->m_impl.m_freeNodes; }

    Index getRoot() const { return this->m_impl.m_root; }

    // Check properties of the tree
    void checkProperties() const {
      this->checkBstProperty();
      (void) this->checkRbtProperties();
    }

    // Check the BST property of the tree
    void checkBstProperty() const {
        const auto capacity = this->m_impl.getCapacity();
        auto it = this->m_impl.begin();
        FwSizeType size = 0;
        for (FwSizeType i = 0; i < capacity; i++) {
            if (!it.isInRange()) {
                break;
            }
            const KE key1 = it->getKey();
            size++;
            it++;
            if (!it.isInRange()) {
                break;
            }
            const KE key2 = it->getKey();
            ASSERT_LT(key1, key2);
        }
        ASSERT_EQ(size, this->m_impl.getSize());
    }

    // Check the red-black tree properties of the tree. Return the black height.
    FwSizeType checkRbtProperties() const {
        auto node = this->m_impl.getOuterNodeUnder(this->m_impl.m_root, Direction::LEFT);
        const auto capacity = this->m_impl.getCapacity();
        for (FwSizeType i = 0; i < capacity; i++) {
            if (node == Node::NONE) {
                break;
            }
            const auto rightChild = this->m_impl.m_nodes[node].getChild(Direction::RIGHT);
            if (rightChild != Node::NONE) {
                // There is a right child. Go to the leftmost node under that child.
                node = this->m_impl.getOuterNodeUnder(rightChild, Direction::LEFT);
            } else {
                // There is no right child. Go upwards until we pass through a left child
                // or we hit the root.
                for (FwSizeType j = 0; j < capacity; j++) {
                    this->checkForRedViolation(node);
                    this->updateBlackHeight(node);
                    const auto previousNode = node;
                    node = this->m_impl.m_nodes[node].parent;
                    if ((node == Node::NONE) or (node.getChild(Direction::LEFT) == previousNode)) {
                        break;
                    }
                }
            }
        }
        return getBlackHeight(this->m_impl.m_root);
    }

    // Get the black height of a node
    FwSizeType getBlackHeight(Index node) const { return (node == Node::NONE) ? 1 : this->blackHeights[node]; }

    // Check for a red violation at a node
    void checkForRedViolation(Index node) const {
        if (Impl::getNodeColor(node) == Color::RED) {
            const auto& nodes = this->m_impl.m_nodes;
            const auto leftChild = nodes[node].getChild(Direction::LEFT);
            ASSERT_NE(nodes[leftChild].color, Color::RED)
              << "Red child violation at left child\n"
              << "  node index is " << node << "\n"
              << "  node key is " << nodes[node].entry.getKeyOrElement() << "\n"
              << "  left child index is " << leftChild << "\n"
              << "  left child key is " << nodes[leftChild].entry.getKeyOrElement() << "\n";
            const auto rightChild = nodes[node].getChild(Direction::RIGHT);
            ASSERT_NE(nodes[rightChild].color, Color::RED)
              << "Red child violation at right child\n"
              << "  node index is " << node << "\n"
              << "  node key is " << nodes[node].entry.getKeyOrElement() << "\n"
              << "  right child index is " << rightChild << "\n"
              << "  right child key is " << nodes[rightChild].entry.getKeyOrElement() << "\n";
        }
    }

    // Update the black height of a node, after visiting all its descendants.
    // Check for a black height violation.
    void updateBlackHeight(Index node) {
        const auto& nodes = this->m_impl.m_nodes;
        if (node != Node::NONE) {
            const auto leftChild = nodes[node].getChild(Direction::LEFT);
            const auto leftHeight = getBlackHeight(leftChild);
            const auto rightChild = nodes[node].getChild(Direction::RIGHT);
            const auto rightHeight = getBlackHeight(rightChild);
            ASSERT_EQ(leftHeight, rightHeight)
              << "Black height violation\n"
              << "  left child index is " << leftChild << "\n"
              << "  left child key is " << nodes[leftChild].entry.getKeyOrElement() << "\n"
              << "  right child index is " << rightChild << "\n"
              << "  right child key is " << nodes[rightChild].entry.getKeyOrElement() << "\n";
            const FwSizeType nodeHeight =
                (Impl::getNodeColor(node) == Color::BLACK) ? 1 : 0;
            this->blackHeights[node] = leftHeight + rightHeight + nodeHeight;
        }
    }

  private:
    const Impl& m_impl;
    // Array for storing black heights
    ExternalArray<FwSizeType> blackHeights = {};
};

}  // namespace Fw

#endif
