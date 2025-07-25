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

    //! The node index type
    using Index = typename Node::Index;

    //! The direction type
    using Direction = typename Node::Direction;

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
            // TODO: Set m_node to point to the initial node
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
            // TODO
        }

        //! Check whether the iterator is in range
        bool isInRange() const override {
            FW_ASSERT(this->m_impl != nullptr);
            return this->m_node < this->m_impl->getCapacity();
        }

        //! Set the iterator to the end value
        void setToEnd() { this->m_node = Index::NONE; }

      private:
        //! The implementation over which to iterate
        const RedBlackTreeSetOrMapImpl<KE, VN>* m_impl = nullptr;

        //! The current node
        Index m_node = Index::NONE;
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
        for (FwSizeType i = 0; i < this->getCapacity(); i++) {
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
        // TODO
        this->clear();
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        // TODO
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
        return ExternalArray<Entry>::getByteArraySize(capacity);
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

    //! This function inserts node into the tree as a left or right child of parent,
    //! according to direction. It rebalances the tree as needed to maintain the
    //! red-black invariant.
    //!
    //! It is permissible for parent to be NONE. In this case we are inserting
    //! at the root of the tree, and direction is ignored.
    //!
    //! It is not permissible for node to be NONE.
    void insertNode(Index node,    //!< The node to insert
                    Index parent,  //!< The new parent
                    Direction      //!< The direction under the new parent
    ) {
        // TODO
    }

    //! This function removes a node of the tree. On entry, node stores the key
    //! and value to be removed. It must not be NONE. On return, removedNode
    //! stores the node that was actually removed.
    void removeNode(Index node,         //!< The node to remove (input)
                    Index& removedNode  //!< The node actually removed (output)
    ) {
        // TODO
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
