// ======================================================================
// \title  RedBlackTreeSetOrMapImplTest.cpp
// \author bocchino
// \brief  cpp file for RedBlackTreeSetOrMapImpl tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/test/ut/ExternalStackTester.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestScenarios.hpp"

namespace Fw {

namespace RedBlackTreeSetOrMapImplTest {

using ImplTester = RedBlackTreeSetOrMapImplTester<State::KeyType, State::ValueType>;

TEST(RedBlackTreeSetOrMapImpl, ZeroArgConstructor) {
    State::Impl impl;
    ASSERT_EQ(impl.getCapacity(), 0U);
    ASSERT_EQ(impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImpl, TypedStorageConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    ASSERT_EQ(state.tester.getNodes().getElements(), nodes);
    ExternalStackTester<ImplTester::Index> stackTester(state.tester.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImpl, UntypedStorageConstructor) {
    constexpr auto alignment = State::Impl::getByteArrayAlignment();
    constexpr auto byteArraySize = State::Impl::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    State::Impl impl(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getNodes().getElements(), reinterpret_cast<ImplTester::Node*>(bytes));
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImpl, CopyConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    State::Impl impl1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = impl1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    State::Impl impl2(impl1);
    State::Tester tester1(impl1);
    State::Tester tester2(impl2);
    ASSERT_EQ(tester2.getNodes().getElements(), nodes);
    ASSERT_EQ(tester2.getNodes().getSize(), FwSizeType(State::capacity));
    ExternalStackTester<ImplTester::Index> stackTester(tester2.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(impl2.getSize(), 1U);
}

TEST(RedBlackTreeSetOrMapImpl, CopyAssignmentOperator) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    State::Impl impl1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = impl1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    State::Impl impl2;
    ASSERT_EQ(impl2.getSize(), 0U);
    // Call the copy assignment operator
    impl2 = impl1;
    ASSERT_EQ(impl2.getSize(), 1U);
}

TEST(ArraySetOrMapImpl, IteratorConstruction) {
    State::Impl impl;
    State::Impl::ConstIterator it(impl);
}

TEST(RedBlackTreeSetOrMapImpl, IteratorComparison) {
    // Test comparison in default case
    State::Impl::ConstIterator it1;
    State::Impl::ConstIterator it2;
    ASSERT_TRUE(it1.compareEqual(it2));
}

TEST(RedBlackTreeSetOrMapImplScenarios, Clear) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.impl.getSize(), 1U);
    Rules::clear.apply(state);
    ASSERT_EQ(state.impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImplScenarios, Find) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::find.apply(state);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, FindExisting) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, InsertExisting) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, InsertFull) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredKey = false;
    Rules::insertFull.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, InsertNotFull) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    const FwSizeType n = 30;
    for (FwSizeType i = 0; i < n; i++) {
        Rules::insertNotFull.apply(state);
    }
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.tester.printBlackHeight();
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveRoot) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveRedLeaf) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    state.storedKey = 1;
    Rules::insertNotFull.apply(state);
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveNodeWithOneChildAtRoot) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 2; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 0;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveNodeWithOneChildAwayFromRoot) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 6; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 4;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveNodeWithTwoChildren) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 3; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 1;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveBlackLeafWithRedDistantNephew) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 10; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 6;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveBlackLeafWithRedCloseNephew) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    const State::KeyType keys[] = {10313, 13047, 30597, 41133, 11108, 44775, 25209, 12493, 9954,  37137,
                                   55775, 43881, 45994, 34609, 58674, 16217, 20766, 37020, 26979, 42589,
                                   34189, 54346, 32929, 41537, 14284, 54076, 19044, 61246, 63806, 14754};

    const FwSizeType numKeys = FW_NUM_ARRAY_ELEMENTS(keys);
    for (FwSizeType i = 0; i < numKeys; i++) {
        state.storedKey = keys[i];
        Rules::insertNotFull.apply(state);
    }
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 19044;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveBlackLeafWithRedParent) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 6; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.storedKey = 4;
    Rules::remove.apply(state);
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 2;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveBlackLeafWithRedSibling) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < 6; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.storedKey = 4;
    Rules::remove.apply(state);
    std::cout << "Before:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.storedKey = 0;
    Rules::remove.apply(state);
    std::cout << "After:\n";
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveExisting) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    const FwSizeType m = 10;
    const FwSizeType n = 10;
    for (FwSizeType i = 0; i < m; i++) {
        for (FwSizeType j = 0; j < n; j++) {
            Rules::insertNotFull.apply(state);
        }
        for (FwSizeType j = 0; j < n / 2; j++) {
            Rules::removeExisting.apply(state);
        }
    }
}

TEST(RedBlackTreeSetOrMapImplScenarios, Random) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("RedBlackTreeSetOrMapImplRandom"), state, 1000);
}

}  // namespace RedBlackTreeSetOrMapImplTest
}  // namespace Fw
