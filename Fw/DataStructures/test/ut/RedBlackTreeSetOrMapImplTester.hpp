// ======================================================================
// \title  RedBlackTreeSetOrMapImplTester.hpp
// \author bocchino
// \brief  Class template for access to RedBlackTreeSetOrMapImpl members
// ======================================================================

#ifndef Fw_RedBlackTreeSetOrMapImplTester_HPP
#define Fw_RedBlackTreeSetOrMapImplTester_HPP

#include <gtest/gtest.h>
#include <ostream>

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename KE, typename VN>
class RedBlackTreeSetOrMapImplTester {
  public:
    using Impl = RedBlackTreeSetOrMapImpl<KE, VN>;

    using Color = typename Impl::Color;

    using Direction = typename Impl::Direction;

    using FreeNodes = typename Impl::FreeNodes;

    using Index = typename Impl::Index;

    using Node = typename Impl::Node;

    using Nodes = typename Impl::Nodes;

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

    const Nodes& getNodes() const { return this->m_impl.m_nodes; }

    const FreeNodes& getFreeNodes() const { return this->m_impl.m_freeNodes; }

    Index getRoot() const { return this->m_impl.m_root; }

    // Check properties of the tree
    void checkProperties() {
        this->checkBstProperty();
        this->checkRbtProperties();
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
            const KE key1 = it.getEntry().getKey();
            size++;
            it.increment();
            if (!it.isInRange()) {
                break;
            }
            const KE key2 = it.getEntry().getKey();
            ASSERT_LT(key1, key2);
        }
        ASSERT_EQ(size, this->m_impl.getSize());
    }

    // Check the red-black tree properties of the tree. Return the black height.
    void checkRbtProperties() {
        const auto& nodes = this->m_impl.m_nodes;
        auto node = this->m_impl.getOuterNodeUnder(this->m_impl.m_root, Direction::LEFT);
        const auto capacity = this->m_impl.getCapacity();
        bool done = (capacity > 0);
        for (FwSizeType i = 0; i < capacity; i++) {
            if (node == Node::NONE) {
                done = true;
                break;
            }
            const auto rightChild = nodes[node].getChild(Direction::RIGHT);
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
                    node = nodes[node].m_parent;
                    if ((node == Node::NONE) or (nodes[node].getChild(Direction::LEFT) == previousNode)) {
                        break;
                    }
                }
            }
        }
        ASSERT_TRUE(done);
    }

    // Get the black height of a node
    FwSizeType getBlackHeight(Index node) const { return (node == Node::NONE) ? 1 : this->blackHeights[node]; }

    // Check for a red violation at a node
    void checkForRedViolation(Index node) const {
        if (this->m_impl.getNodeColor(node) == Color::RED) {
            const auto& nodes = this->m_impl.m_nodes;
            const auto leftChild = nodes[node].getChild(Direction::LEFT);
            ASSERT_NE(this->m_impl.getNodeColor(leftChild), Color::RED)
                << "Red child violation at left child\n"
                << "  node index is " << node << "\n"
                << "  node key is " << nodes[node].m_entry.getKeyOrElement() << "\n"
                << "  left child index is " << leftChild << "\n"
                << "  left child key is " << nodes[leftChild].m_entry.getKeyOrElement() << "\n";
            const auto rightChild = nodes[node].getChild(Direction::RIGHT);
            ASSERT_NE(this->m_impl.getNodeColor(rightChild), Color::RED)
                << "Red child violation at right child\n"
                << "  node index is " << node << "\n"
                << "  node key is " << nodes[node].m_entry.getKeyOrElement() << "\n"
                << "  right child index is " << rightChild << "\n"
                << "  right child key is " << nodes[rightChild].m_entry.getKeyOrElement() << "\n";
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
            ASSERT_EQ(leftHeight, rightHeight) << "Black height violation at node " << node << "\n"
                                               << "  left child index is " << leftChild << "\n"
                                               << "  right child index is " << rightChild << "\n";
            const FwSizeType nodeHeight = (this->m_impl.getNodeColor(node) == Color::BLACK) ? 1 : 0;
            const FwSizeType blackHeight = leftHeight + nodeHeight;
            this->blackHeights[node] = blackHeight;
        }
    }

    // Print the black height
    void printBlackHeight() { std::cout << "black height is " << getBlackHeight(this->m_impl.m_root) << "\n"; }

    // Print the tree
    void printTree() {
        auto node = this->m_impl.m_root;
        auto child = Node::NONE;
        const auto capacity = this->m_impl.getCapacity();
        I32 indent = 0;
        constexpr I32 indentIncrement = 2;
        const FwSizeType loopBound = 3 * capacity;
        for (FwSizeType i = 0; i < loopBound; i++) {
            if (node == Node::NONE) {
                break;
            }
            const auto& nodeObject = this->m_impl.m_nodes[node];
            const auto leftChild = nodeObject.m_left;
            const auto rightChild = nodeObject.m_right;
            if (child == Node::NONE) {
                this->printNode(indent, node);
                if (leftChild != Node::NONE) {
                    indent += indentIncrement;
                    node = leftChild;
                } else if (rightChild != Node::NONE) {
                    indent += indentIncrement;
                    node = rightChild;
                } else {
                    indent -= indentIncrement;
                    child = node;
                    node = nodeObject.m_parent;
                }
            } else if ((child == leftChild) && (rightChild != Node::NONE)) {
                indent += indentIncrement;
                node = rightChild;
                child = Node::NONE;
            } else {
                indent -= indentIncrement;
                child = node;
                node = nodeObject.m_parent;
            }
        }
    }

    // Print a node
    void printNode(I32 indent, Index node) {
        for (I32 i = 0; i < indent; i++) {
            std::cout << " ";
        }
        if (node == Node::NONE) {
            std::cout << "NONE\n";
        } else {
            const auto& nodeObject = this->m_impl.m_nodes[node];
            const auto parent = nodeObject.m_parent;
            if (parent != Node::NONE) {
                const auto parentDirection = this->m_impl.getDirectionFromParent(node);
                std::cout << directionToString(parentDirection) << " ";
            }
            Fw::String parentStr;
            indexToString(parent, parentStr);
            std::cout << "Node " << node << " { parent=" << parentStr.toChar()
                      << ", key=" << nodeObject.m_entry.getKeyOrElement()
                      << ", value=" << nodeObject.m_entry.getValueOrNil()
                      << ", color=" << colorToString(nodeObject.m_color) << " }\n";
        }
    }

    // Convert an index to a string
    void indexToString(Index node, Fw::StringBase& str) {
        if (node == Node::NONE) {
            str = "NONE";
        } else {
            str.format("%" PRI_FwSizeType, node);
        }
    }

    // Convert a color to a string
    static const char* colorToString(Color color) { return (color == Color::RED) ? "RED" : "BLACK"; }

    // Convert a direction to a string
    static const char* directionToString(Direction direction) {
        return (direction == Direction::LEFT) ? "LEFT" : "RIGHT";
    }

  private:
    const Impl& m_impl;
    // Array for storing black heights
    ExternalArray<FwSizeType> blackHeights = {};
};

}  // namespace Fw

#endif
