// ======================================================================
// \title  ExternalRedBlackTreeSetTest.cpp
// \author bocchino
// \brief  cpp file for ExternalRedBlackTreeSet tests
// ======================================================================

#include "Fw/DataStructures/ExternalRedBlackTreeSet.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ExternalRedBlackTreeSet.hpp"
#include "Fw/DataStructures/test/ut/ExternalStackTester.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalRedBlackTreeSetTester {
  public:
    ExternalRedBlackTreeSetTester<T>(const ExternalRedBlackTreeSet<T>& set) : m_set(set) {}

    const RedBlackTreeSetOrMapImpl<T, Nil>& getImpl() const { return this->m_set.m_impl; }

  private:
    const ExternalRedBlackTreeSet<T>& m_set;
};

namespace SetTest {

using Impl = RedBlackTreeSetOrMapImpl<State::ElementType, Nil>;
using ImplTester = RedBlackTreeSetOrMapImplTester<State::ElementType, Nil>;
using Set = ExternalRedBlackTreeSet<State::ElementType>;
using SetTester = ExternalRedBlackTreeSetTester<State::ElementType>;
using StackTester = ExternalStackTester<ImplTester::Index>;

TEST(ExternalRedBlackTreeSet, ZeroArgConstructor) {
    Set set;
    ASSERT_EQ(set.getCapacity(), 0);
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalRedBlackTreeSet, TypedStorageConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    SetTester setTester(set);
    ImplTester implTester(setTester.getImpl());
    ASSERT_EQ(implTester.getNodes().getElements(), nodes);
    StackTester stackTester(implTester.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalRedBlackTreeSet, UntypedStorageConstructor) {
    constexpr auto alignment = Set::getByteArrayAlignment();
    constexpr auto byteArraySize = Set::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Set set(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    SetTester setTester(set);
    ImplTester implTester(setTester.getImpl());
    ASSERT_EQ(implTester.getNodes().getElements(), reinterpret_cast<Impl::Node*>(bytes));
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalRedBlackTreeSet, CopyConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    Set set1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::ElementType e = 42;
    const auto status = set1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Set set2(set1);
    SetTester setTester1(set1);
    SetTester setTester2(set2);
    ImplTester implTester1(setTester1.getImpl());
    ImplTester implTester2(setTester2.getImpl());
    ASSERT_EQ(implTester2.getNodes().getElements(), nodes);
    ASSERT_EQ(implTester2.getNodes().getSize(), FwSizeType(State::capacity));
    StackTester stackTester(implTester2.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(set2.getSize(), 1);
}

TEST(ExternalRedBlackTreeSet, CopyAssignmentOperator) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    Set set1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::ElementType e = 42;
    const auto status = set1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Set set2;
    ASSERT_EQ(set2.getSize(), 0);
    // Call the copy assignment operator
    set2 = set1;
    ASSERT_EQ(set2.getSize(), 1);
}

TEST(ExternalRedBlackTreeSet, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    ImplTester::Node nodes1[maxSize];
    ImplTester::Node nodes2[maxSize];
    ImplTester::Index freeNodes1[maxSize];
    ImplTester::Index freeNodes2[maxSize];
    Set s1(nodes1, freeNodes1, maxSize);
    // size1 < capacity2
    {
        Set s2(nodes2, freeNodes2, maxSize);
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == size2
    {
        Set s2(nodes2, freeNodes2, maxSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > size2
    {
        Set s2(nodes2, freeNodes2, smallSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(ExternalRedBlackTreeSetScenarios, Clear) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::clear(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Find) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::find(state);
}

TEST(ExternalRedBlackTreeSetScenarios, FindExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::findExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::insertExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertFull) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::insertFull(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertNotFull) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::insertNotFull(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Remove) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::remove(state);
}

TEST(ExternalRedBlackTreeSetScenarios, RemoveExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::removeExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Random) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Set set(nodes, freeNodes, State::capacity);
    State state(set);
    Scenarios::random(Fw::String("ExternalRedBlackTreeSetRandom"), state, 1000);
}

}  // namespace SetTest
}  // namespace Fw
