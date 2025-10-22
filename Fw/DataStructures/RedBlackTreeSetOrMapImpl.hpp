//
// ======================================================================
// \title  RedBlackTreeSetOrMapImpl
// \author bocchino
// \brief  An implementation of a set or map based on a red-black tree
// ======================================================================

#ifndef Fw_RedBlackTreeSetOrMapImpl_HPP
#define Fw_RedBlackTreeSetOrMapImpl_HPP

#include <limits>

#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/DataStructures/ExternalStack.hpp"
#include "Fw/DataStructures/SetOrMapImplConstIterator.hpp"
#include "Fw/DataStructures/SetOrMapImplEntry.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

//! This class template implements a red-black tree that can be used as a
//! set or map. A red-black tree is a binary search tree (BST) such that
//! each node is colored red or black. A red-black tree is valid if
//!
//! 1. It satisfies the red child invariant: No red node has a red child.
//!
//! 2. It satisfies the black height invariant (described below).
//!
//! The black height invariant of a red-black tree T is checked with respect to the
//! leaf-augmented tree T'. T' is constructed from T by replacing every "missing"
//! node in T (i.e., every place where there could be a child but is not) with
//! a new black node. The black height invariant says that for every node N in T',
//! every path from N to a leaf in T' goes through the same number of black nodes.
//!
//! A valid red-black tree is balanced, in the sense that the find operation
//! takes O(log n) steps.
template <typename KE, typename VN>
class RedBlackTreeSetOrMapImpl final {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV>
    friend class RedBlackTreeSetOrMapImplTester;

  public:
    // ----------------------------------------------------------------------
    // The Node type
    // ----------------------------------------------------------------------

    //! Node
    class Node {
      public:
        //! Color
        enum class Color : U8 { BLACK, RED };

        //! Direction
        enum class Direction : U8 { LEFT, RIGHT };

        //! The type of a node index
        using Index = FwSizeType;

        //! The type of an entry in the set or map
        using Entry = SetOrMapImplEntry<KE, VN>;

      public:
        //! Constant value representing no node
        static constexpr Index NONE = std::numeric_limits<Index>::max();

      public:
        //! The index of the parent of this node
        Index m_parent = NONE;

        //! The index of the left child of this node
        Index m_left = NONE;

        //! The index of the right child of this node
        Index m_right = NONE;

        //! The color of this node
        Color m_color = Color::BLACK;

        //! The set or map entry stored in this node
        Entry m_entry = {};

      public:
        //! Get the child of this node in the specified direction
        //! \return The child
        Index getChild(Direction direction  //!< The direction
        ) const {
            return (direction == Direction::LEFT) ? this->m_left : this->m_right;
        }

        //! Set the child of this node in the specified direction
        void setChild(Direction direction,  //!< The direction
                      Index node            //!< The node index
        ) {
            if (direction == Direction::LEFT) {
                this->m_left = node;
            } else {
                this->m_right = node;
            }
        }

      public:
        // Get the opposite direction
        static Direction getOppositeDirection(Direction direction  //!< The direction
        ) {
            return (direction == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
        }
    };

  public:
    // ----------------------------------------------------------------------
    // Type aliases
    // ----------------------------------------------------------------------

    //! The color type
    using Color = typename Node::Color;

    //! The direction type
    using Direction = typename Node::Direction;

    //! The entry type
    using Entry = typename Node::Entry;

    //! The node index type
    using Index = typename Node::Index;

    //! The type of the array for storing the tree nodes
    using Nodes = ExternalArray<Node>;

    //! The type of the stack of indices of free nodes
    using FreeNodes = ExternalStack<Index>;

  public:
    // ----------------------------------------------------------------------
    // The ConstIterator type
    // ----------------------------------------------------------------------

    //! Const iterator
    class ConstIterator final : public SetOrMapImplConstIterator<KE, VN> {
      public:
        using ImplKind = typename SetOrMapImplConstIterator<KE, VN>::ImplKind;

      public:
        //! Default constructor
        ConstIterator() {}

        //! Constructor providing the implementation
        ConstIterator(const RedBlackTreeSetOrMapImpl<KE, VN>& impl)
            : SetOrMapImplConstIterator<KE, VN>(), m_impl(&impl) {
            this->m_node = this->m_impl->getOuterNodeUnder(this->m_impl->m_root, Direction::LEFT);
        }

        //! Copy constructor
        ConstIterator(const ConstIterator& it)
            : SetOrMapImplConstIterator<KE, VN>(), m_impl(it.m_impl), m_node(it.m_node) {}

        //! Destructor
        ~ConstIterator() override = default;

      public:
        //! Copy assignment operator
        ConstIterator& operator=(const ConstIterator& it) {
            this->m_impl = it.m_impl;
            this->m_node = it.m_node;
            return *this;
        }

        //! Equality comparison operator
        bool compareEqual(const ConstIterator& it) const {
            bool result = false;
            if ((this->m_impl == nullptr) && (it.m_impl == nullptr)) {
                result = true;
            } else if (this->m_impl == it.m_impl) {
                result |= (this->m_node == it.m_node);
                result |= (!this->isInRange() and !it.isInRange());
            }
            return result;
        }

        //! Return the impl kind
        //! \return The impl kind
        ImplKind implKind() const override { return ImplKind::RED_BLACK_TREE; }

        //! Get the set or map impl entry pointed to by this iterator
        //! \return The set or map impl entry
        const Entry& getEntry() const override { return this->m_impl->m_nodes[this->m_node].m_entry; }

        //! Increment operator
        void increment() override {
            FW_ASSERT(this->m_impl != nullptr);
            const auto& nodes = this->m_impl->m_nodes;
            if (this->m_node != Node::NONE) {
                const auto rightChild = nodes[this->m_node].getChild(Direction::RIGHT);
                if (rightChild != Node::NONE) {
                    // There is a right child. Go to the leftmost node under that child.
                    this->m_node = this->m_impl->getOuterNodeUnder(rightChild, Direction::LEFT);
                } else {
                    // There is no right child. Go upwards until we pass through a left child
                    // or we hit the root.
                    const auto capacity = this->m_impl->getCapacity();
                    bool done = (capacity == 0);
                    for (FwSizeType i = 0; i < capacity; i++) {
                        const auto previousNode = this->m_node;
                        this->m_node = nodes[this->m_node].m_parent;
                        if ((this->m_node == Node::NONE) or
                            (nodes[this->m_node].getChild(Direction::LEFT) == previousNode)) {
                            done = true;
                            break;
                        }
                    }
                    FW_ASSERT(done == true);
                }
            }
        }

        //! Check whether the iterator is in range
        bool isInRange() const override {
            FW_ASSERT(this->m_impl != nullptr);
            return this->m_node < this->m_impl->getCapacity();
        }

        //! Set the iterator to the end value
        void setToEnd() { this->m_node = Node::NONE; }

      private:
        //! The implementation over which to iterate
        const RedBlackTreeSetOrMapImpl<KE, VN>* m_impl = nullptr;

        //! The current node
        Index m_node = Node::NONE;
    };

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    RedBlackTreeSetOrMapImpl() = default;

    //! Constructor providing typed backing storage.
    //! nodes must point to at least capacity elements of type Node.
    //! freeNodes must point to at least capacity elements of type FwSizeType.
    RedBlackTreeSetOrMapImpl(Node* nodes,         //!< The nodes
                             Index* freeNodes,    //!< The free nodes
                             FwSizeType capacity  //!< The capacity
    ) {
        this->setStorage(nodes, freeNodes, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    RedBlackTreeSetOrMapImpl(ByteArray data,      //!< The data
                             FwSizeType capacity  //!< The capacity
    ) {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    RedBlackTreeSetOrMapImpl(const RedBlackTreeSetOrMapImpl<KE, VN>& impl) { *this = impl; }

    //! Destructor
    ~RedBlackTreeSetOrMapImpl() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    RedBlackTreeSetOrMapImpl<KE, VN>& operator=(const RedBlackTreeSetOrMapImpl<KE, VN>& impl) {
        if (&impl != this) {
            this->m_nodes = impl.m_nodes;
            this->m_freeNodes = impl.m_freeNodes;
            this->m_root = impl.m_root;
        }
        return *this;
    }

    //! Get the begin iterator
    ConstIterator begin() const { return ConstIterator(*this); }

    //! Clear the set or map
    void clear() {
        // Set the root to NONE
        this->m_root = Node::NONE;
        // Clear the free node stack
        this->m_freeNodes.clear();
        // Push all the nodes on the free node stack
        const auto capacity = this->getCapacity();
        for (FwSizeType i = 0; i < capacity; i++) {
            const auto status = this->m_freeNodes.push(capacity - i - 1);
            FW_ASSERT(status == Success::SUCCESS, static_cast<FwAssertArgType>(status));
        }
    }

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
        auto node = Node::NONE;
        auto direction = Direction::LEFT;
        const auto status = this->findNode(keyOrElement, node, direction);
        if (status == Success::SUCCESS) {
            valueOrNil = this->m_nodes[node].m_entry.getValueOrNil();
        }
        return status;
    }

    //! Get the capacity of the set or map (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const { return this->m_nodes.getSize(); }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const {
        const auto capacity = this->getCapacity();
        const auto freeNodesSize = this->m_freeNodes.getSize();
        FW_ASSERT(freeNodesSize <= capacity, static_cast<FwAssertArgType>(freeNodesSize),
                  static_cast<FwAssertArgType>(capacity));
        return capacity - freeNodesSize;
    }

    //! Insert an element in the set or a (key, value) pair in the map
    //! \return SUCCESS if there is room in the set or map
    Success insert(const KE& keyOrElement,  //!< The key or element
                   const VN& valueOrNil     //!< The value or Nil
    ) {
        auto node = Node::NONE;
        auto direction = Direction::LEFT;
        auto status = Success::FAILURE;
        const auto findStatus = this->findNode(keyOrElement, node, direction);
        if (findStatus == Success::SUCCESS) {
            this->m_nodes[node].m_entry.setValueOrNil(valueOrNil);
            status = Success::SUCCESS;
        } else {
            const auto parent = node;
            status = this->m_freeNodes.pop(node);
            if (status == Success::SUCCESS) {
                this->m_nodes[node] = Node();
                this->m_nodes[node].m_entry.setKeyOrElement(keyOrElement);
                this->m_nodes[node].m_entry.setValueOrNil(valueOrNil);
                this->insertNode(node, parent, direction);
            }
        }
        return status;
    }

    //! Remove an element from the set or a (key, value) pair from the map
    //! \return SUCCESS if the key or element was there
    Success remove(const KE& keyOrElement,  //!< The key or element
                   VN& valueOrNil           //!< The value or Nil
    ) {
        auto node = Node::NONE;
        auto direction = Direction::LEFT;
        const auto status = findNode(keyOrElement, node, direction);
        if (status == Success::SUCCESS) {
            valueOrNil = this->m_nodes[node].m_entry.getValue();
            auto removedNode = Node::NONE;
            this->removeNode(node, removedNode);
            const auto pushStatus = this->m_freeNodes.push(removedNode);
            FW_ASSERT(pushStatus == Success::SUCCESS, static_cast<FwAssertArgType>(pushStatus));
        }
        return status;
    }

    //! Set the backing storage (typed data)
    //! nodes must point to at least capacity elements of type Node.
    //! freeNodes must point to at least capacity elements of type FwSizeType.
    void setStorage(Node* nodes,         //!< The nodes
                    Index* freeNodes,    //!< The free nodes
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_nodes.setStorage(nodes, capacity);
        this->m_freeNodes.setStorage(freeNodes, capacity);
        this->clear();
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_nodes.setStorage(data, capacity);
        const auto nodesSize = Nodes::getByteArraySize(capacity);
        // Compute the nearest offset at or after nodesSize that is aligned for FreeNodes
        const auto freeNodesAlignment = FreeNodes::getByteArrayAlignment();
        const U8 modulus = nodesSize % freeNodesAlignment;
        const FwSizeType freeNodesOffset = (modulus == 0) ? 0 : freeNodesAlignment - modulus;
        FW_ASSERT(freeNodesOffset % freeNodesAlignment == 0, static_cast<FwAssertArgType>(freeNodesOffset),
                  static_cast<FwAssertArgType>(freeNodesAlignment));
        const auto freeNodesSize = FreeNodes::getByteArraySize(capacity);
        // Make sure that data has enough room
        FW_ASSERT(freeNodesOffset + freeNodesSize <= data.size, static_cast<FwAssertArgType>(freeNodesOffset),
                  static_cast<FwAssertArgType>(freeNodesSize), static_cast<FwAssertArgType>(data.size));
        ByteArray freeNodesData(&data.bytes[freeNodesOffset], freeNodesSize);
        // Set the storage and clear freeNodes
        this->m_freeNodes.setStorage(freeNodesData, capacity);
        this->clear();
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for a RedBlackTreeSetOrMapImpl
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ExternalArray<Entry>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalArray of the specified capacity,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return Nodes::getByteArraySize(capacity) + FreeNodes::getByteArrayAlignment() +
               FreeNodes::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! This function tries to find a node whose key or element ke matches keyOrElement.
    //! On return from the function:
    //! 1. If such a node exists, then the return value is SUCCESS,
    //!    and node stores the index of N.
    //! 2. Otherwise
    //!    a. The return value is FAILURE.
    //!    b. If the tree is empty, then node holds NONE.
    //!    c. Otherwise node stores the index of the node N containing the NONE
    //!       child where ke should be inserted, and direction stores the direction
    //!       of the child in N (left or right).
    Success findNode(const KE& keyOrElement,  //!< The key or element (input)
                     Index& node,             //!< The node index (output)
                     Direction& direction     //!< The direction (output)
    ) const {
        auto result = Success::FAILURE;
        auto parent = Node::NONE;
        auto child = this->m_root;
        const auto capacity = this->getCapacity();
        bool done = (capacity == 0);
        for (FwSizeType i = 0; i < capacity; i++) {
            if (child == Node::NONE) {
                node = parent;
                done = true;
                break;
            }
            const auto& entryKey = this->m_nodes[child].m_entry.getKeyOrElement();
            if (keyOrElement == entryKey) {
                result = Success::SUCCESS;
                node = child;
                done = true;
                break;
            } else if (keyOrElement < entryKey) {
                direction = Direction::LEFT;
                parent = child;
                child = this->m_nodes[parent].m_left;
            } else {
                direction = Direction::RIGHT;
                parent = child;
                child = this->m_nodes[parent].m_right;
            }
        }
        FW_ASSERT(done);
        return result;
    }

    //! Get the direction from the parent, i.e., the direction (left or
    //! right) to follow from the parent of node to get to node.
    //! node must not be NONE. The parent of node must not be NONE.
    Direction getDirectionFromParent(Index node  //!< The node index
    ) const {
        const auto parent = this->m_nodes[node].m_parent;
        const auto parentRight = m_nodes[parent].m_right;
        return (node == parentRight) ? Direction::RIGHT : Direction::LEFT;
    }

    //! Get the color of a node
    //! \return The color
    Color getNodeColor(Index index  //!< The node index
    ) const {
        return (index == Node::NONE) ? Color::BLACK : this->m_nodes[index].m_color;
    }

    //! Get the outer node under node in the specified direction. If node has
    //! no child in that direction, then the result is node.
    Index getOuterNodeUnder(Index node,          //!< The node index
                            Direction direction  //!< The direction
    ) const {
        auto child = (node != Node::NONE) ? this->m_nodes[node].getChild(direction) : Node::NONE;
        const auto capacity = this->getCapacity();
        bool done = (capacity == 0);
        for (FwSizeType i = 0; i < capacity; i++) {
            if (child == Node::NONE) {
                done = true;
                break;
            }
            node = child;
            child = this->m_nodes[child].getChild(direction);
        }
        FW_ASSERT(done == true);
        return node;
    }

    //! This function inserts node into the tree as a left or right child of parent,
    //! according to direction. It rebalances the tree as needed to maintain the
    //! red-black invariant.
    //!
    //! It is permissible for parent to be NONE. In this case we are inserting
    //! at the root of the tree, and direction is ignored.
    //!
    //! It is not permissible for node to be NONE.
    void insertNode(Index node,          //!< The node to insert
                    Index parent,        //!< The new parent
                    Direction direction  //!< The direction under the new parent
    ) {
        // We assume (1) that the tree is a red-black tree, (2) that parent is NONE or
        // the child of parent in the direction `direction` is NONE, and (3) that
        // both children of node are NONE.
        FW_ASSERT(this->m_nodes[node].getChild(Direction::LEFT) == Node::NONE);
        FW_ASSERT(this->m_nodes[node].getChild(Direction::RIGHT) == Node::NONE);
        this->m_nodes[node].m_color = Color::RED;
        this->m_nodes[node].m_parent = parent;
        if (parent == Node::NONE) {
            this->m_root = node;
            // The tree was empty, and now it consists of a single red node.
        } else {
            FW_ASSERT(this->m_nodes[parent].getChild(direction) == Node::NONE,
                      static_cast<FwAssertArgType>(this->m_nodes[parent].getChild(direction)));
            // Set the parent
            this->m_nodes[parent].setChild(direction, node);
            const auto capacity = this->getCapacity();
            bool done = (capacity == 0);
            for (FwSizeType i = 0; i < capacity; i++) {
                // The following invariants hold: (1) node is colored red; (2)
                // there may be a red child violation from parent to node; and
                // (3) there are no other violations at any nodes.
                if (this->getNodeColor(parent) == Color::BLACK) {
                    // There is no red child violation at parent, because parent is black.
                    done = true;
                    break;
                }
                const auto grandparent = this->m_nodes[parent].m_parent;
                if (grandparent == Node::NONE) {
                    this->m_nodes[parent].m_color = Color::BLACK;
                    // This step removes the red child violation at parent.
                    // It preserves all other invariants.
                    done = true;
                    break;
                }
                const auto parentDirection = this->getDirectionFromParent(parent);
                const auto parentOppositeDirection = Node::getOppositeDirection(parentDirection);
                const auto uncle = this->m_nodes[grandparent].getChild(parentOppositeDirection);
                if (this->getNodeColor(uncle) == Color::BLACK) {
                    if (this->m_nodes[parent].getChild(parentOppositeDirection) == node) {
                        // The subtree rooted at grandparent has the following
                        // shape, assuming that parentDirection is RIGHT.
                        // There is a red child violation from parent to node.
                        // There are no other violations at any nodes.
                        // K1, K2, K3, and K4 are arbitrary keys with K1 < K2 < K3 < K4.
                        /*
                        //                    BBBBBBBBBBBBBBBBBBBB
                        //                   B                    B
                        //                   B  K2 (grandparent)  B
                        //                   B                    B
                        //                    BBBBBBBBBBBBBBBBBBBB
                        //                        /          \
                        //                       /            \
                        //                      V              V
                        //         BBBBBBBBBBBBBB              RRRRRRRRRRRRRRR
                        //        B              B            R               R
                        //        B  K1 (uncle)  B            R  K4 (parent)  R
                        //        B              B            R               R
                        //         BBBBBBBBBBBBBB              RRRRRRRRRRRRRRR
                        //             |     |                   /         \
                        //             |     |                  /           \
                        //             V     V                 V             \
                        //        ------------------      RRRRRRRRRRRRR      |
                        //        |                |     R             R     |
                        //        | black height n |     R  K3 (node)  R     |
                        //        |                |     R             R     |
                        //        ------------------      RRRRRRRRRRRRR      /
                        //                                   |     |        /
                        //                                   |     |       /
                        //                                   V     V      V
                        //                              ----------------------
                        //                              |                    |
                        //                              | black height n + 1 |
                        //                              |                    |
                        //                              ----------------------
                        */
                        this->rotateSubtree(parent, parentDirection);
                        parent = this->m_nodes[grandparent].getChild(parentDirection);
                    }
                    // The subtree rooted at grandparent has the following
                    // shape, assuming that parentDirection is RIGHT.
                    // There is a red child violation from parent to K4.
                    // There are no other violations at any nodes.
                    /*
                    //                    BBBBBBBBBBBBBBBBBBBB
                    //                   B                    B
                    //                   B  K2 (grandparent)  B
                    //                   B                    B
                    //                    BBBBBBBBBBBBBBBBBBBB
                    //                        /          \
                    //                       /            \
                    //                      V              V
                    //         BBBBBBBBBBBBBB              RRRRRRRRRRRRRRR
                    //        B              B            R               R
                    //        B  K1 (uncle)  B            R  K3 (parent)  R
                    //        B              B            R               R
                    //         BBBBBBBBBBBBBB              RRRRRRRRRRRRRRR
                    //             |     |                    /        \
                    //             |     |                   /          \
                    //             V     V                  |            V
                    //        ------------------            |         RRRRRRRRRRRRRR
                    //        |                |            |        R              R
                    //        | black height n |            |        R      K4      R
                    //        |                |            |        R              R
                    //        ------------------            |         RRRRRRRRRRRRRR
                    //                                      \             |    |
                    //                                       \            |    |
                    //                                        V           V    V
                    //                                      ----------------------
                    //                                      |                    |
                    //                                      | black height n + 1 |
                    //                                      |                    |
                    //                                      ----------------------
                    */
                    this->rotateSubtree(grandparent, parentOppositeDirection);
                    this->m_nodes[parent].m_color = Color::BLACK;
                    this->m_nodes[grandparent].m_color = Color::RED;
                    // The subtree has the following shape.
                    /*
                    //                               BBBBBBBBBBBBBBBBBBB
                    //                              B                   B
                    //                              B    K3 (parent)    B
                    //                              B                   B
                    //                               BBBBBBBBBBBBBBBBBBB
                    //                                   /          \
                    //                                  /            \
                    //                                 V              V
                    //              RRRRRRRRRRRRRRRRRRRR              RRRRRRRRRRRRRRRR
                    //             R                    R            R                R
                    //             R  K2 (grandparent)  R            R       K4       R
                    //             R                    R            R                R
                    //              RRRRRRRRRRRRRRRRRRRR              RRRRRRRRRRRRRRRR
                    //                   /            \                    |    |
                    //                  /              \                   |    |
                    //                 V                V                  V    V
                    //     BBBBBBBBBBBBBBBBBB           ----------------------------
                    //    B                  B          |                          |
                    //    B    K1 (uncle)    B          |    black height n + 1    |
                    //    B                  B          |                          |
                    //     BBBBBBBBBBBBBBBBBB           ----------------------------
                    //           |     |
                    //           |     |
                    //           V     V
                    //      ------------------
                    //      |                |
                    //      | black height n |
                    //      |                |
                    //      ------------------
                    */
                    done = true;
                    break;
                } else {
                    // The subtree rooted at grandparent has one of four
                    // shapes, one of which is shown below. Each of the arrows
                    // to red nodes may point the other way.
                    // There is a red child violation from parent to K4.
                    // There are no other violations at any nodes.
                    /*
                    //                    BBBBBBBBBBBBBBBBBBBB
                    //                   B                    B
                    //                   B  K2 (grandparent)  B
                    //                   B                    B
                    //                    BBBBBBBBBBBBBBBBBBBB
                    //                        /          \
                    //                       /            \
                    //                      V              V
                    //         RRRRRRRRRRRRRR              RRRRRRRRRRRRRRR
                    //        R              R            R               R
                    //        R  K1 (uncle)  R            R  K3 (parent)  R
                    //        R              R            R               R
                    //         RRRRRRRRRRRRRR              RRRRRRRRRRRRRRR
                    //             \     \                     /     \
                    //              \     \                   /       \
                    //               \     \                 /         V
                    //                \     \               /       RRRRRRRRRRRRRR
                    //                 \     \             /       R              R
                    //                  \     \           /        R      K4      R
                    //                   \     \          |        R              R
                    //                    \     \         |         RRRRRRRRRRRRRR
                    //                     \     \        |           /   /
                    //                      \     \       |          /   /
                    //                       V     V      V         V   V
                    //                   ----------------------------------
                    //                   |                                |
                    //                   |         black height n         |
                    //                   |                                |
                    //                   ----------------------------------
                    */
                    this->m_nodes[parent].m_color = Color::BLACK;
                    this->m_nodes[uncle].m_color = Color::BLACK;
                    this->m_nodes[grandparent].m_color = Color::RED;
                    node = grandparent;
                    parent = this->m_nodes[node].m_parent;
                    // The tree shown above now has the following shape.
                    /*
                    //                    ???????????????????
                    //                   ?                   ?
                    //                   ?       parent      ?
                    //                   ?                   ?
                    //                    ???????????????????
                    //                             ^
                    //                             |
                    //                             |
                    //                    RRRRRRRRRRRRRRRRRRR
                    //                   R                   R
                    //                   R     K2 (node)     R
                    //                   R                   R
                    //                    RRRRRRRRRRRRRRRRRRR
                    //                        /          \
                    //                       /            \
                    //                      V              V
                    //         BBBBBBBBBBBBBB              BBBBBBBBBBBBBBB
                    //        B              B            B               B
                    //        B  K1 (uncle)  B            B       K3      B
                    //        B              B            B               B
                    //         BBBBBBBBBBBBBB              BBBBBBBBBBBBBBB
                    //             \     \                     /     \
                    //              \     \                   /       \
                    //               \     \                 /         V
                    //                \     \               /       RRRRRRRRRRRRRR
                    //                 \     \             /       R              R
                    //                  \     \           /        R      K4      R
                    //                   \     \          |        R              R
                    //                    \     \         |         RRRRRRRRRRRRRR
                    //                     \     \        |           /   /
                    //                      \     \       |          /   /
                    //                       V     V      V         V   V
                    //                   ----------------------------------
                    //                   |                                |
                    //                   |         black height n         |
                    //                   |                                |
                    //                   ----------------------------------
                    */
                    if (parent == Node::NONE) {
                        // We have reached the root of the tree.
                        // Break out of the loop.
                        done = true;
                        break;
                    }
                    // The invariants at the top of the loop are satisfied.
                    // Continue to the next iteration.
                }
            }
            FW_ASSERT(done);
        }
        // The tree is a red-black tree.
    }

    //! This function removes a node that is colored black and is a leaf node
    //! (i.e., it has no children) and is not the root. node stores the node to
    //! remove. It must not be NONE.
    void removeBlackLeafNode(Index node  //!< The node to remove
    ) {
        // We assume that the tree is a red-black tree, that node is colored
        // black, that node is a leaf node, and that node is not the root.
        auto parent = m_nodes[node].m_parent;
        // Since node is not the root, parent is not NONE.
        auto direction = this->getDirectionFromParent(node);
        auto oppositeDirection = Node::getOppositeDirection(direction);
        // The leaf-augmented subtree rooted at parent has this shape, assuming
        // direction == RIGHT. We use ? to represent the unknown color (red or
        // black) of parent.
        /*
        //                      ????????????????
        //                     ?                ?
        //                     ?     parent     ?
        //                     ?                ?
        //                      ????????????????
        //                        /          \
        //                       /            \
        //                      V              V
        //        ------------------           BBBBBBBBBBBB
        //        |                |          B            B
        //        | black height 2 |          B    node    B
        //        |                |          B            B
        //        ------------------           BBBBBBBBBBBB
        //                                      /        \
        //                                     /          \
        //                                    V            V
        //                             BBBBBBBBBB        BBBBBBBBBB
        //                            B          B      B          B
        //                            B          B      B          B
        //                            B          B      B          B
        //                             BBBBBBBBBB        BBBBBBBBBB
        */
        this->m_nodes[parent].setChild(direction, Node::NONE);
        // The previous step performs the deletion.
        // The remaining steps are for rebalancing.
        /*
        // The leaf-augmented subtree rooted at parent looks like this,
        // assuming direction == RIGHT:
        //
        //                      ????????????????
        //                     ?                ?
        //                     ?     parent     ?
        //                     ?                ?
        //                      ????????????????
        //                        /          \
        //                       /            \
        //                      V              V
        //        ------------------         BBBBBBBBBBBB
        //        |                |        B            B
        //        | black height 2 |        B            B
        //        |                |        B            B
        //        ------------------         BBBBBBBBBBBB
        //
        // The black height invariant is violated, because the left subtree of
        // parent has black height 2, and the right subtree has black height 1.
        // Therefore there is a black height violation at parent and at every
        // node in the path from the root to parent. To restore the black
        // height invariant, we must perform rebalancing.
        */
        bool done = false;
        const auto capacity = this->getCapacity();
        for (FwSizeType i = 0; i < capacity; i++) {
            // The red child constraint is satisfied. The black height
            // constraint is violated because the leaf-augmented subtree rooted
            // at parent has this shape, assuming direction == RIGHT. i is the
            // loop index. Note that this diagram agrees with the previous one
            // when i = 0.
            /*
            //                          ????????????????
            //                         ?                ?
            //                         ?     parent     ?
            //                         ?                ?
            //                          ????????????????
            //                            /          \
            //                           /            \
            //                          V              V
            //        ----------------------        ----------------------
            //        |                    |        |                    |
            //        | black height i + 2 |        | black height i + 1 |
            //        |                    |        |                    |
            //        ----------------------        ----------------------
            //
            // The black height constraint would be satisfied if the child of
            // parent in the direction `direction` were replaced with a red-black
            // tree of black height i + 2.
            */
            auto sibling = this->m_nodes[parent].getChild(oppositeDirection);
            auto closeNephew = this->m_nodes[sibling].getChild(direction);
            auto distantNephew = this->m_nodes[sibling].getChild(oppositeDirection);
            // The leaf-augmented subtree rooted at parent has this shape
            // (direction == RIGHT). If distantNephew or closeNephew are leaves
            // in the leaf-augmented tree, then K1 and K4 are names for the
            // nodes; they are not keys in the original tree.
            // There is a black height violation at parent.
            /*
            //                                 ???????????????????
            //                                ?                   ?
            //                                ?    K6 (parent)    ?
            //                                ?                   ?
            //                                 ???????????????????
            //                                    /           \
            //                                   /             \
            //                                  V               V
            //                     ?????????????????          ----------------------
            //                    ?                 ?         |                    |
            //                    ?   K2 (sibling)  ?         | black height i + 1 |
            //                    ?                 ?         |                    |
            //                     ?????????????????          ----------------------
            //                         /       \
            //                        /         \
            //                       V           V
            //     ????????????????????         ????????????????????
            //    ?                    ?       ?                    ?
            //    ? K1 (distantNephew) ?       ?  K4 (closeNephew)  ?
            //    ?                    ?       ?                    ?
            //     ????????????????????         ????????????????????
            //           |      |                     |      |
            //           |      |                     |      |
            //           V      V                     V      V
            //  -------------------------     -------------------------
            //  |                       |     |                       |
            //  | black height i + 2 -  |     | black height i + 2 -  |
            //  | number of black nodes |     | number of black nodes |
            //  |     in { K1, K2 }     |     |     in { K2, K4 }     |
            //  |                       |     |                       |
            //  -------------------------     -------------------------
            */
            if (this->getNodeColor(sibling) == Color::RED) {
                // The leaf-augmented subtree rooted at parent has this shape.
                // There is a black height violation at parent.
                /*
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                B                   B
                //                                B    K6 (parent)    B
                //                                B                   B
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     RRRRRRRRRRRRRRRRR          ----------------------
                //                    R                 R         |                    |
                //                    R   K2 (sibling)  R         | black height i + 1 |
                //                    R                 R         |                    |
                //                     RRRRRRRRRRRRRRRRR          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //    B                    B       B                    B
                //    B K1 (distantNephew) B       B  K4 (closeNephew)  B
                //    B                    B       B                    B
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //           |      |                     |      |
                //           |      |                     |      |
                //           V      V                     V      V
                //   ------------------------     ------------------------
                //   |                      |     |                      |
                //   |  black height i + 1  |     |  black height i + 1  |
                //   |                      |     |                      |
                //   ------------------------     ------------------------
                */
                this->rotateSubtree(parent, direction);
                this->m_nodes[parent].m_color = Color::RED;
                this->m_nodes[sibling].m_color = Color::BLACK;
                sibling = closeNephew;
                closeNephew = this->m_nodes[sibling].getChild(direction);
                distantNephew = this->m_nodes[sibling].getChild(oppositeDirection);
                // The subtree has this shape:
                /*
                //                           BBBBBBBBBBBBBBBB
                //                          B                B
                //                          B       K2       B
                //                          B                B
                //                           BBBBBBBBBBBBBBBB
                //                             /          \
                //                            /            \
                //                           /              \
                //                          V                V
                //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                //            B              B              R                   R
                //            B      K1      B              R    K6 (parent)    R
                //            B              B              R                   R
                //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                //                 |    |                        /         \
                //                 |    |                       /           \
                //                 V    V                      V             V
                //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                //   |                      |     B                  B     |                      |
                //   |  black height i + 1  |     B   K4 (sibling)   B     |  black height i + 1  |
                //   |                      |     B                  B     |                      |
                //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                //                                      /      \
                //                                     /        \
                //                                    V          V
                //       --------------------------------      --------------------------------
                //       |                              |      |                              |
                //       |      black height i + 1      |      |      black height i + 1      |
                //       | with root K3 (distantNephew) |      |  with root K5 (closeNephew)  |
                //       |                              |      |                              |
                //       --------------------------------      --------------------------------
                */
                if (this->getNodeColor(distantNephew) == Color::RED) {
                    // The subtree has this shape:
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //            B              B              R                   R
                    //            B      K1      B              R    K6 (parent)    R
                    //            B              B              R                   R
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //                 |    |                        /         \
                    //                 |    |                       /           \
                    //                 V    V                      V             V
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //   |                      |     B                  B     |                      |
                    //   |  black height i + 1  |     B   K4 (sibling)   B     |  black height i + 1  |
                    //   |                      |     B                  B     |                      |
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //                                      /      \
                    //                                     /        \
                    //                                    V          V
                    //            RRRRRRRRRRRRRRRRRRRRRRRRRR       ------------------------
                    //           R                          R      |                      |
                    //           R    K3 (distantNephew)    R      |  black height i + 1  |
                    //           R                          R      |                      |
                    //            RRRRRRRRRRRRRRRRRRRRRRRRRR       ------------------------
                    //                     |      |
                    //                     |      |
                    //                     V      V
                    //             ------------------------
                    //             |                      |
                    //             |  black height i + 1  |
                    //             |                      |
                    //             ------------------------
                    */
                    this->removeBlackLeafNodeHelper2(parent, sibling, distantNephew, direction);
                    // The subtree has this shape. The entire tree is a valid red-black tree.
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //            B              B              R                   R
                    //            B      K1      B              R    K4 (sibling)   R
                    //            B              B              R                   R
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //                 |    |                        /         \
                    //                 |    |                       /           \
                    //                 V    V                      V             V
                    //   ------------------------    BBBBBBBBBBBBBBBBBBBB      BBBBBBBBBBBBBBB
                    //   |                      |   B                    B    B               B
                    //   |  black height i + 1  |   B K3 (distantNephew) B    B  K6 (parent)  B
                    //   |                      |   B                    B    B               B
                    //   ------------------------    BBBBBBBBBBBBBBBBBBBB      BBBBBBBBBBBBBBB
                    //                                      |      |               |     |
                    //                                      |      |               |     |
                    //                                      V      V               V     V
                    //                             ------------------------  ------------------------
                    //                             |                      |  |                      |
                    //                             |  black height i + 1  |  |  black height i + 1  |
                    //                             |                      |  |                      |
                    //                             ------------------------  ------------------------
                    */
                    done = true;
                    break;
                } else if (this->getNodeColor(closeNephew) == Color::RED) {
                    // The subtree has this shape:
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //            B              B              R                   R
                    //            B      K1      B              R    K6 (parent)    R
                    //            B              B              R                   R
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //                 |    |                        /         \
                    //                 |    |                       /           \
                    //                 V    V                      V             V
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //   |                      |     B                  B     |                      |
                    //   |  black height i + 1  |     B   K4 (sibling)   B     |  black height i + 1  |
                    //   |                      |     B                  B     |                      |
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //                                      /      \
                    //                                     /        \
                    //                                    V          V
                    //              ------------------------        RRRRRRRRRRRRRRRRRRRRRRRRRR
                    //              |                      |       R                          R
                    //              |  black height i + 1  |       R     K5 (closeNephew)     R
                    //              |                      |       R                          R
                    //              ------------------------        RRRRRRRRRRRRRRRRRRRRRRRRRR
                    //                                                       |      |
                    //                                                       |      |
                    //                                                       V      V
                    //                                               ------------------------
                    //                                               |                      |
                    //                                               |  black height i + 1  |
                    //                                               |                      |
                    //                                               ------------------------
                    */
                    this->removeBlackLeafNodeHelper1(closeNephew, oppositeDirection, sibling, distantNephew);
                    // The subtree has this shape:
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //            B              B              R                   R
                    //            B      K1      B              R    K6 (parent)    R
                    //            B              B              R                   R
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRR
                    //                 |    |                        /         \
                    //                 |    |                       /           \
                    //                 V    V                      V             V
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //   |                      |     B                  B     |                      |
                    //   |  black height i + 1  |     B   K5 (sibling)   B     |  black height i + 1  |
                    //   |                      |     B                  B     |                      |
                    //   ------------------------      BBBBBBBBBBBBBBBBBB      ------------------------
                    //                                      /      \
                    //                                     /        \
                    //                                    V          V
                    //              RRRRRRRRRRRRRRRRRRRRRRRRRR     ------------------------
                    //             R                          R    |                      |
                    //             R    K4 (distantNephew)    R    |  black height i + 1  |
                    //             R                          R    |                      |
                    //              RRRRRRRRRRRRRRRRRRRRRRRRRR     ------------------------
                    //                       |      |
                    //                       |      |
                    //                       V      V
                    //               ------------------------
                    //               |                      |
                    //               |  black height i + 1  |
                    //               |                      |
                    //               -----------------------
                    */
                    this->removeBlackLeafNodeHelper2(parent, sibling, distantNephew, direction);
                    // The subtree has this shape. The entire tree is a valid red-black tree.
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRRR
                    //            B              B              R                    R
                    //            B      K1      B              R    K5 (sibling)    R
                    //            B              B              R                    R
                    //             BBBBBBBBBBBBBB                RRRRRRRRRRRRRRRRRRRR
                    //                 |    |                         /        \
                    //                 |    |                        /          \
                    //                 V    V                       V            V
                    //   ------------------------      BBBBBBBBBBBBBBBBBBBB     BBBBBBBBBBBBBBB
                    //   |                      |     B                    B   B               B
                    //   |  black height i + 1  |     B K4 (distantNephew) B   B  K6 (parent)  B
                    //   |                      |     B                    B   B               B
                    //   ------------------------      BBBBBBBBBBBBBBBBBBBB     BBBBBBBBBBBBBBB
                    //                                         |   |                 |   |
                    //                                         |   |                 |   |
                    //                                         V   V                 V   V
                    //                              ------------------------  ------------------------
                    //                              |                      |  |                      |
                    //                              |  black height i + 1  |  |  black height i + 1  |
                    //                              |                      |  |                      |
                    //                              ------------------------  ------------------------
                    */
                    done = true;
                    break;
                } else {
                    this->m_nodes[sibling].m_color = Color::RED;
                    this->m_nodes[parent].m_color = Color::BLACK;
                    // The subtree has this shape. The entire tree is a valid red-black tree.
                    /*
                    //                           BBBBBBBBBBBBBBBB
                    //                          B                B
                    //                          B       K2       B
                    //                          B                B
                    //                           BBBBBBBBBBBBBBBB
                    //                             /          \
                    //                            /            \
                    //                           /              \
                    //                          V                V
                    //             BBBBBBBBBBBBBB                BBBBBBBBBBBBBBBBBBB
                    //            B              B              B                   B
                    //            B      K1      B              B    K6 (parent)    B
                    //            B              B              B                   B
                    //             BBBBBBBBBBBBBB                BBBBBBBBBBBBBBBBBBB
                    //                 |    |                        /         \
                    //                 |    |                       /           \
                    //                 V    V                      V             V
                    //   ------------------------      RRRRRRRRRRRRRRRRRR      ------------------------
                    //   |                      |     R                  R     |                      |
                    //   |  black height i + 1  |     R   K4 (sibling)   R     |  black height i + 1  |
                    //   |                      |     R                  R     |                      |
                    //   ------------------------      RRRRRRRRRRRRRRRRRR      ------------------------
                    //                                      /      \
                    //                                     /        \
                    //                                    V          V
                    //            BBBBBBBBBBBBBBBBBBBBBBBBBB        BBBBBBBBBBBBBBBBBBBBBBBB
                    //           B                          B      B                        B
                    //           B    K3 (distantNephew)    B      B    K5 (closeNephew)    B
                    //           B                          B      B                        B
                    //            BBBBBBBBBBBBBBBBBBBBBBBBBB        BBBBBBBBBBBBBBBBBBBBBBBB
                    //                     |      |                         |      |
                    //                     |      |                         |      |
                    //                     V      V                         V      V
                    //             ------------------------         ------------------------
                    //             |                      |         |                      |
                    //             |    black height i    |         |    black height i    |
                    //             |                      |         |                      |
                    //             ------------------------         ------------------------
                    */
                    done = true;
                    break;
                }
            } else if (this->getNodeColor(distantNephew) == Color::RED) {
                // The leaf-augmented subtree rooted at parent has this shape.
                // There is a black height violation at parent.
                /*
                //                                 ???????????????????
                //                                ?                   ?
                //                                ?    K6 (parent)    ?
                //                                ?                   ?
                //                                 ???????????????????
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                    B                 B         |                    |
                //                    B   K2 (sibling)  B         | black height i + 1 |
                //                    B                 B         |                    |
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //     RRRRRRRRRRRRRRRRRRRR       ------------------------
                //    R                    R      |                      |
                //    R K1 (distantNephew) R      |  black height i + 1  |
                //    R                    R      |                      |
                //     RRRRRRRRRRRRRRRRRRRR       ------------------------
                //           |      |
                //           |      |
                //           V      V
                //   ------------------------
                //   |                      |
                //   |  black height i + 1  |
                //   |                      |
                //   ------------------------
                */
                this->removeBlackLeafNodeHelper2(parent, sibling, distantNephew, direction);
                // The subtree has this shape. The entire tree is a valid red-black tree.
                /*
                //                     BBBBBBBBBBBBBBBBB
                //                    B                 B
                //                    B   K2 (sibling)  B
                //                    B                 B
                //                     BBBBBBBBBBBBBBBBB
                //                         /       \
                //                        /         \
                //                       V           V
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBB
                //    B                    B       B                 B
                //    B K1 (distantNephew) B       B   K6 (parent)   B
                //    B                    B       B                 B
                //     RRRRRRRRRRRRRRRRRRRR         BBBBBBBBBBBBBBBBB
                //           |      |                    |     |
                //           |      |                    |     |
                //           V      V                    V     V
                //   ------------------------   ------------------------
                //   |                      |   |                      |
                //   |  black height i + 1  |   |  black height i + 1  |
                //   |                      |   |                      |
                //   ------------------------   ------------------------
                */
                done = true;
                break;
            } else if (this->getNodeColor(closeNephew) == Color::RED) {
                // The leaf-augmented subtree rooted at parent has this shape.
                // There is a black height violation at parent.
                /*
                //                                 ???????????????????
                //                                ?                   ?
                //                                ?    K6 (parent)    ?
                //                                ?                   ?
                //                                 ???????????????????
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                    B                 B         |                    |
                //                    B   K2 (sibling)  B         | black height i + 1 |
                //                    B                 B         |                    |
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //  ------------------------        RRRRRRRRRRRRRRRRRRRR
                //  |                      |       R                    R
                //  |  black height i + 1  |       R  K4 (closeNephew)  R
                //  |                      |       R                    R
                //  ------------------------        RRRRRRRRRRRRRRRRRRRR
                //                                        |      |
                //                                        |      |
                //                                        V      V
                //                                ------------------------
                //                                |                      |
                //                                |  black height i + 1  |
                //                                |                      |
                //                                ------------------------
                */
                this->removeBlackLeafNodeHelper1(closeNephew, oppositeDirection, sibling, distantNephew);
                // The subtree has this shape:
                /*
                //                                 ???????????????????
                //                                ?                   ?
                //                                ?    K6 (parent)    ?
                //                                ?                   ?
                //                                 ???????????????????
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                    B                 B         |                    |
                //                    B   K4 (sibling)  B         | black height i + 1 |
                //                    B                 B         |                    |
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //     RRRRRRRRRRRRRRRRRRRR       ------------------------
                //    R                    R      |                      |
                //    R K2 (distantNephew) R      |  black height i + 1  |
                //    R                    R      |                      |
                //     RRRRRRRRRRRRRRRRRRRR       ------------------------
                //           |      |
                //           |      |
                //           V      V
                //   ------------------------
                //   |                      |
                //   |  black height i + 1  |
                //   |                      |
                //   ------------------------
                */
                this->removeBlackLeafNodeHelper2(parent, sibling, distantNephew, direction);
                // The subtree has this shape. The entire tree is a valid red-black tree.
                /*
                //                     BBBBBBBBBBBBBBBBB
                //                    B                 B
                //                    B   K4 (sibling)  B
                //                    B                 B
                //                     BBBBBBBBBBBBBBBBB
                //                         /       \
                //                        /         \
                //                       V           V
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBB
                //    B                    B       B                 B
                //    B K2 (distantNephew) B       B   K6 (parent)   B
                //    B                    B       B                 B
                //     RRRRRRRRRRRRRRRRRRRR         BBBBBBBBBBBBBBBBB
                //           |      |                    |     |
                //           |      |                    |     |
                //           V      V                    V     V
                //   ------------------------   ------------------------
                //   |                      |   |                      |
                //   |  black height i + 1  |   |  black height i + 1  |
                //   |                      |   |                      |
                //   ------------------------   ------------------------
                */
                done = true;
                break;
            } else if (this->getNodeColor(parent) == Color::RED) {
                this->m_nodes[sibling].m_color = Color::RED;
                this->m_nodes[parent].m_color = Color::BLACK;
                // The leaf-augmented tree rooted at parent has this shape.
                // The entire tree is a valid red-black tree.
                /*
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                B                   B
                //                                B    K6 (parent)    B
                //                                B                   B
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     RRRRRRRRRRRRRRRRR          ----------------------
                //                    R                 R         |                    |
                //                    R   K2 (sibling)  R         | black height i + 1 |
                //                    R                 R         |                    |
                //                     RRRRRRRRRRRRRRRRR          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //    B                    B       B                    B
                //    B K1 (distantNephew) B       B  K4 (closeNephew)  B
                //    B                    B       B                    B
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //           |      |                     |      |
                //           |      |                     |      |
                //           V      V                     V      V
                //   ------------------------     ------------------------
                //   |                      |     |                      |
                //   |    black height i    |     |    black height i    |
                //   |                      |     |                      |
                //   ------------------------     ------------------------
                */
                done = true;
                break;
            } else {
                // The leaf-augmented subtree rooted at parent has this shape.
                /*
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                B                   B
                //                                B    K6 (parent)    B
                //                                B                   B
                //                                 BBBBBBBBBBBBBBBBBBB
                //                                    /           \
                //                                   /             \
                //                                  V               V
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                    B                 B         |                    |
                //                    B   K2 (sibling)  B         | black height i + 1 |
                //                    B                 B         |                    |
                //                     BBBBBBBBBBBBBBBBB          ----------------------
                //                         /       \
                //                        /         \
                //                       V           V
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //    B                    B       B                    B
                //    B K1 (distantNephew) B       B  K4 (closeNephew)  B
                //    B                    B       B                    B
                //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                //           |      |                     |      |
                //           |      |                     |      |
                //           V      V                     V      V
                //   ------------------------     ------------------------
                //   |                      |     |                      |
                //   |    black height i    |     |    black height i    |
                //   |                      |     |                      |
                //   ------------------------     ------------------------
                */
                this->m_nodes[sibling].m_color = Color::RED;
                node = parent;
                parent = this->m_nodes[node].m_parent;
                if (parent == Node::NONE) {
                    // The entire leaf-augmented tree has this shape.
                    // The entire tree is a valid red-black tree.
                    /*
                    //                                 BBBBBBBBBBBBBBBBBBB
                    //                                B                   B
                    //                                B     K6 (node)     B
                    //                                B                   B
                    //                                 BBBBBBBBBBBBBBBBBBB
                    //                                    /           \
                    //                                   /             \
                    //                                  V               V
                    //                     RRRRRRRRRRRRRRRRR          ----------------------
                    //                    R                 R         |                    |
                    //                    R   K2 (sibling)  R         | black height i + 1 |
                    //                    R                 R         |                    |
                    //                     RRRRRRRRRRRRRRRRR          ----------------------
                    //                         /       \
                    //                        /         \
                    //                       V           V
                    //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                    //    B                    B       B                    B
                    //    B K1 (distantNephew) B       B  K4 (closeNephew)  B
                    //    B                    B       B                    B
                    //     BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                    //           |      |                     |      |
                    //           |      |                     |      |
                    //           V      V                     V      V
                    //   ------------------------     ------------------------
                    //   |                      |     |                      |
                    //   |    black height i    |     |    black height i    |
                    //   |                      |     |                      |
                    //   ------------------------     ------------------------
                    */
                    done = true;
                    break;
                } else {
                    direction = getDirectionFromParent(node);
                    oppositeDirection = Node::getOppositeDirection(direction);
                    // The leaf-augmented subtree rooted at parent has this
                    // shape, assuming that direction == RIGHT:
                    /*
                    //                         ??????????????
                    //                        ?              ?
                    //                        ?    parent    ?
                    //                        ?              ?
                    //                         ??????????????
                    //                            /      \
                    //                           /        \
                    //                          V          V
                    //       ------------------------     BBBBBBBBBBBBBBBBBBB
                    //       |                      |    B                   B
                    //       |  black height i + 3  |    B     K6 (node)     B
                    //       |                      |    B                   B
                    //       ------------------------     BBBBBBBBBBBBBBBBBBB
                    //                                       /           \
                    //                                      /             \
                    //                                     V               V
                    //                        RRRRRRRRRRRRRRRRR          ----------------------
                    //                       R                 R         |                    |
                    //                       R   K2 (sibling)  R         | black height i + 1 |
                    //                       R                 R         |                    |
                    //                        RRRRRRRRRRRRRRRRR          ----------------------
                    //                            /       \
                    //                           /         \
                    //                          V           V
                    //        BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                    //       B                    B       B                    B
                    //       B K1 (distantNephew) B       B  K4 (closeNephew)  B
                    //       B                    B       B                    B
                    //        BBBBBBBBBBBBBBBBBBBB         BBBBBBBBBBBBBBBBBBBB
                    //              |      |                     |      |
                    //              |      |                     |      |
                    //              V      V                     V      V
                    //      ------------------------     ------------------------
                    //      |                      |     |                      |
                    //      |    black height i    |     |    black height i    |
                    //      |                      |     |                      |
                    //      ------------------------     ------------------------
                    //
                    // There is a black height violation at parent because the
                    // left subtree of parent has black height i + 3, and the
                    // right subtree has black height i + 2. So we need at
                    // least one more loop iteration. After incrementing i, the
                    // invariant at the top of the loop is satisfied.
                    */
                }
            }
        }
        FW_ASSERT(done);
        // The tree is a valid red-black tree.
    }

    //! This is a helper function for removeBlackLeafNode. sibling and
    //! distantNephew are in-out parameters (they are both read and written).
    void removeBlackLeafNodeHelper1(Index closeNephew,            //!< The close nephew (input)
                                    Direction oppositeDirection,  //!< The opposite direction (input)
                                    Index& sibling,               //!< The sibling (input and output)
                                    Index& distantNephew          //!< The distant nephew (input and output)
    ) {
        this->rotateSubtree(sibling, oppositeDirection);
        this->m_nodes[sibling].m_color = Color::RED;
        this->m_nodes[closeNephew].m_color = Color::BLACK;
        distantNephew = sibling;
        sibling = closeNephew;
    }

    //! This is a helper function for removeBlackLeafNode.
    void removeBlackLeafNodeHelper2(Index parent,         //!< The parent
                                    Index sibling,        //!< The sibling
                                    Index distantNephew,  //!< The distant nephew
                                    Direction direction   //!< The direction
    ) {
        this->rotateSubtree(parent, direction);
        this->m_nodes[sibling].m_color = this->m_nodes[parent].m_color;
        this->m_nodes[parent].m_color = Color::BLACK;
        this->m_nodes[distantNephew].m_color = Color::BLACK;
    }

    //! This function removes a node of the tree. On entry, node stores the key
    //! and value to be removed. It must not be NONE. On return, removedNode
    //! stores the node that was actually removed.
    void removeNode(Index node,         //!< The node to remove (input)
                    Index& removedNode  //!< The node actually removed (output)
    ) {
        if ((this->m_nodes[node].m_left) != Node::NONE && (this->m_nodes[node].m_right != Node::NONE)) {
            this->removeNodeWithTwoChildren(node, removedNode);
        } else {
            this->removeNodeWithAtMostOneChild(node);
            removedNode = node;
        }
    }

    //! This function removes a node of the tree with at most one child. On
    //! entry, node stores the node to be removed. It must not be NONE.
    void removeNodeWithAtMostOneChild(Index node  //!< The node to remove
    ) {
        if (this->m_nodes[node].m_left != Node::NONE) {
            this->removeNodeWithOneChild(node, Direction::LEFT);
        } else if (this->m_nodes[node].m_right != Node::NONE) {
            this->removeNodeWithOneChild(node, Direction::RIGHT);
        } else if (node == this->m_root) {
            this->m_root = Node::NONE;
        } else if (this->m_nodes[node].m_color == Color::RED) {
            this->removeRedLeafNode(node);
        } else {
            this->removeBlackLeafNode(node);
        }
    }

    //! This function removes a node of the tree with exactly one child. node
    //! stores the node to remove. It must not be NONE. direction stores the
    //! direction of the child.
    void removeNodeWithOneChild(Index node,          //!< The node
                                Direction direction  //!< The direction of the child
    ) {
        // Since the tree is a valid red-black tree, a node with exactly one
        // child must be black, and the child must be red.
        FW_ASSERT(this->m_nodes[node].m_color == Color::BLACK,
                  static_cast<FwAssertArgType>(this->m_nodes[node].m_color));
        const auto parent = this->m_nodes[node].m_parent;
        const auto child = this->m_nodes[node].getChild(direction);
        FW_ASSERT(this->m_nodes[child].m_color == Color::RED,
                  static_cast<FwAssertArgType>(this->m_nodes[node].m_color));
        if (parent == Node::NONE) {
            this->m_root = child;
        } else {
            const auto parentDirection = this->getDirectionFromParent(node);
            this->m_nodes[parent].setChild(parentDirection, child);
        }
        this->m_nodes[child].m_parent = parent;
        this->m_nodes[child].m_color = Color::BLACK;
    }

    //! This function removes a node of the tree that has two children. On
    //! entry, node stores the key and value to be removed. It must not be NONE,
    //! and it must have two children. On return, removedNode stores the node
    //! that was actually removed.
    void removeNodeWithTwoChildren(Index node,         //!< The node to remove (input)
                                   Index& removedNode  //!< The node actually removed (output)
    ) {
        const auto rightChild = this->m_nodes[node].m_right;
        removedNode = this->getOuterNodeUnder(rightChild, Direction::LEFT);
        this->m_nodes[node].m_entry = this->m_nodes[removedNode].m_entry;
        this->removeNodeWithAtMostOneChild(removedNode);
    }

    //! This function removes a node that is colored red and is a leaf node
    //! (i.e., it has no children) and is not the root. node stores the node to
    //! remove. It must not be NONE.
    void removeRedLeafNode(Index node  //!< The node to remove
    ) {
        FW_ASSERT(node != this->m_root);
        const auto& nodeObj = this->m_nodes[node];
        FW_ASSERT(nodeObj.m_color == Color::RED);
        FW_ASSERT(nodeObj.m_left == Node::NONE);
        FW_ASSERT(nodeObj.m_right == Node::NONE);
        const auto parent = nodeObj.m_parent;
        const auto direction = this->getDirectionFromParent(node);
        this->m_nodes[parent].setChild(direction, Node::NONE);
    }

    //! This function performs a left or right rotation on the subtree whose
    //! root is node. The following invariants must hold on entry to this
    //! function, or an assertion failure will occur:
    //!
    //! 1. node must not be NONE.
    //!
    //! 2. The child of node in the direction opposite direction must not be
    //!    NONE.
    void rotateSubtree(Index node,          //!< The node index
                       Direction direction  //!< The direction
    ) {
        // We assume that the tree is a binary search tree (BST).
        const auto parent = this->m_nodes[node].m_parent;
        const auto oppositeDirection = Node::getOppositeDirection(direction);
        const auto newRoot = this->m_nodes[node].getChild(oppositeDirection);
        const auto newChild = this->m_nodes[newRoot].getChild(direction);
        this->m_nodes[node].setChild(oppositeDirection, newChild);
        if (newChild != Node::NONE) {
            this->m_nodes[newChild].m_parent = node;
        }
        this->m_nodes[newRoot].setChild(direction, node);
        this->m_nodes[newRoot].m_parent = parent;
        if (parent != Node::NONE) {
            const auto parentDirection = getDirectionFromParent(node);
            this->m_nodes[parent].setChild(parentDirection, newRoot);
        } else {
            this->m_root = newRoot;
        }
        this->m_nodes[node].m_parent = newRoot;
        // The tree is a BST.
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array for storing the tree nodes
    Nodes m_nodes = {};

    //! The stack of indices of free nodes. The indices point into m_nodes.
    FreeNodes m_freeNodes = {};

    //! The index of the root node
    Index m_root = Node::NONE;
};

}  // namespace Fw

#endif
