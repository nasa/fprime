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

template <typename KE, typename VN>
class RedBlackTreeSetOrMapImpl final {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV>
    friend class RedBlackTreeSetOrMapImplTester;

  public:
    // ----------------------------------------------------------------------
    // The Entry type
    // ----------------------------------------------------------------------

    //! The type of an entry in the set or map
    using Entry = SetOrMapImplEntry<KE, VN>;

  private:
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
        ) {
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

  private:
    // ----------------------------------------------------------------------
    // The Nodes and FreeNodes types
    // ----------------------------------------------------------------------

    //! The color type
    using Color = typename Node::Color;

    //! The direction type
    using Direction = typename Node::Direction;

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
            if (this->m_node != Node::NONE) {
                const auto rightChild = this->m_impl->m_nodes[this->m_node].getChild(Direction::RIGHT);
                if (rightChild != Node::NONE) {
                    // There is a right child. Go to the leftmost node under that child.
                    this->m_node = this->m_impl->getOuterNodeUnder(rightChild, Direction::LEFT);
                } else {
                    // There is no right child. Go upwards until we pass through a left child
                    // or we hit the root.
                    const auto capacity = this->m_impl->getCapacity();
                    bool done = false;
                    for (FwSizeType i = 0; i < capacity; i++) {
                        const auto previousNode = this->m_node;
                        this->m_node = this->m_impl->m_nodes[this->m_node].parent;
                        if ((this->m_node == Node::NONE) or (this->m_node.getChild(Direction::LEFT) == previousNode)) {
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
    RedBlackTreeSetOrMapImpl(Node* nodes,            //!< The nodes
                             FwSizeType* freeNodes,  //!< The free nodes
                             FwSizeType capacity     //!< The capacity
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
        // Clear the free node stack
        this->m_freeNodes.clear();
        // Push all the nodes on the free node stack
        const auto capacity = this->getCapacity();
        for (FwSizeType i = 0; i < capacity; i++) {
            const auto status = this->m_freeNodes.push(i);
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
        auto direction = Node::LEFT;
        const auto status = this->findNode(keyOrElement, node, direction);
        if (status == Success::SUCCESS) {
            valueOrNil = this->m_nodes[node].entry.getValue();
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
            this->m_nodes[node].setValue(valueOrNil);
            status = Success::SUCCESS;
        } else {
            const auto parent = node;
            status = this->m_freeNodes.pop(node);
            if (status == Success::SUCCESS) {
                this->m_nodes[node] = Node();
                this->m_nodes[node].entry.setKey(keyOrElement);
                this->m_nodes[node].entry.setValue(valueOrNil);
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
            valueOrNil = this->m_nodes[node].entry.getValue();
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
    void setStorage(Node* nodes,            //!< The nodes
                    FwSizeType* freeNodes,  //!< The free nodes
                    FwSizeType capacity     //!< The capacity
    ) {
        this->m_nodes.setStorage(nodes, capacity);
        this->m_freeNodes.setStorage(freeNodes, capacity);
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_nodes.setStorage(data, capacity);
        const auto nodesSize = Nodes::getByteArraySize();
        // Compute the nearest offset at or after nodesSize that is aligned for FreeNodes
        const auto freeNodesAlignment = FreeNodes::getByteArrayAlignment();
        const U8 modulus = nodesSize % freeNodesAlignment;
        const auto freeNodesOffset = (modulus == 0) ? 0 : freeNodesAlignment - modulus;
        FW_ASSERT(freeNodesOffset % freeNodesAlignment == 0, static_cast<FwSizeType>(freeNodesOffset),
                  static_cast<FwSizeType>(freeNodesAlignment));
        const auto freeNodesSize = FreeNodes::getByteArraySize(capacity);
        // Make sure that data has enough room
        FW_ASSERT(freeNodesOffset + freeNodesSize <= data.size, static_cast<FwSizeType>(freeNodesOffset),
                  static_cast<FwSizeType>(freeNodesSize), static_cast<FwSizeType>(data.size));
        ByteArray freeNodesData(&data.bytes[freeNodesOffset], freeNodesSize);
        // Set the storage and clear freeNodes
        this->m_freeNodes.setStorage(freeNodesData, capacity);
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
        // TODO
        return result;
    }

    //! Get the color of a node
    //! \return The color
    Color getNodeColor(Index index  //!< The node index
    ) {
        return (index == Node::NONE) ? Color::BLACK : this->m_nodes[index].color;
    }

    //! Get the outer node under node in the specified direction. If node has
    //! no child in that direction, then the result is node.
    Index getOuterNodeUnder(Index node,          //!< The node index
                            Direction direction  //!< The direction
    ) {
        auto child = (node != Node::NONE) ? this->m_nodes[node].getChild(direction) : Node::NONE;
        bool done = false;
        const auto capacity = this->getCapacity();
        for (FwSizeType i = 0; i < capacity; i++) {
            if (child == Node::NODE) {
                done = true;
                break;
            }
            node = child;
            child = this->m_nodes[child].getChild(direction);
        }
        FW_ASSERT(done == true);
        return node;
    }

    //! Get the parent direction for a node, i.e., the direction (left or
    //! right) to follow from the parent of node to get to node. node must not be
    //! NONE. The parent of node must not be NONE.
    Direction getParentDirection(Index node  //!< The node index
    ) const {
        const auto parent = this->m_nodes[node].parent;
        const auto parentRight = m_nodes[parent].right;
        return (node == parentRight) ? Color::RIGHT : Color::LEFT;
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
        // We assume that the tree is a red-black tree, that the child of
        // parent in the direction direction is NONE, and that both children of
        // node are NONE.
        const auto oppositeDirection = Node::getOppositeDirection(direction);
        this->m_nodes[node].color = Color::RED;
        this->m_nodes[node].parent = parent;
        if (parent == Index::NONE) {
            this->m_root = node;
            // The tree was empty, and now it consists of a single red node.
        } else {
            // Set the parent
            bool done = false;
            this->m_nodes[parent].setChild(direction, node);
            const auto capacity = this->getCapacity();
            for (FwSizeType i = 0; i < capacity; i++) {
                // The following invariants hold: (1) node is colored red; (2)
                // there may be a red child violation at parent; and (3) there
                // are no other violations at any nodes.
                if (this->getNodeColor(parent) == Color::BLACK) {
                    // There is no red child violation at parent, because parent is black.
                    done = true;
                    break;
                }
                const auto grandparent = this->m_nodes[parent].parent;
                if (grandparent == Node::NONE) {
                    this->m_nodes[parent].color = Color::BLACK;
                    // This step removes the red child violation at parent.
                    // It preserves all other invariants.
                    done = true;
                    break;
                }
                const auto parentDirection = this->getParentDirection(parent);
                const auto parentOppositeDirection = Node::getOppositeDirection(parentDirection);
                const auto uncle = this->m_nodes[grandparent].getChild(parentOppositeDirection);
                if (this->getNodeColor(uncle) == Color::BLACK) {
                    if (this->m_nodes[parent].getChild(parentOppositeDirection) == node) {
                        // The subtree rooted at grandparent has the following
                        // shape, assuming that parentDirection is RIGHT.
                        // There is a red violation at parent.
                        //
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
                        //
                        this->rotateSubtree(parent, parentDirection);
                        parent = this->m_nodes[grandparent].getChild(parentDirection);
                    }
                    // TODO
                    done = true;
                    break;
                }
                // TODO
            }
            FW_ASSERT(done);
        }
        // The tree is a red-black tree.
    }

    //! This function removes a node of the tree. On entry, node stores the key
    //! and value to be removed. It must not be NONE. On return, removedNode
    //! stores the node that was actually removed.
    void removeNode(Index node,         //!< The node to remove (input)
                    Index& removedNode  //!< The node actually removed (output)
    ) {
        // TODO
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
        const auto parent = this->m_nodes[node].parent;
        const auto oppositeDirection = Node::getOppositeDirection(direction);
        const auto newRoot = this->m_nodes[node].getChild(oppositeDirection);
        const auto newChild = this->m_nodes[newRoot].getChild(direction);
        this->m_nodes[node].setChild(oppositeDirection, newChild);
        if (newChild != Node::NONE) {
            this->m_nodes[newChild].parent = node;
        }
        this->m_nodes[newRoot].setChild(direction, node);
        this->m_nodes[newRoot].parent = parent;
        this->m_nodes[node].parent = newRoot;
        if (parent != Node::NONE) {
            const auto parentDirection = getParentDirection(node);
            this->m_nodes[parent].setChild(parentDirection, newRoot);
        } else {
            this->m_root = newRoot;
        }
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
