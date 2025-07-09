// ======================================================================
// \title  StackTestState.hpp
// \author bocchino
// \brief  hpp file for Stack test state
// ======================================================================

#ifndef StackTestState_HPP
#define StackTestState_HPP

#include <vector>

#include "Fw/DataStructures/ExternalStack.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace StackTest {

struct State {
    //! The stack item type
    using ItemType = U32;
    //! The stack capacity
    static constexpr FwSizeType capacity = 1024;
    //! The ExternalStack type
    using ExternalStack = ExternalStack<ItemType>;
    //! THe StackBase type
    using StackBase = StackBase<ItemType>;
    //! Constructor
    State(StackBase& a_stack) : stack(a_stack) {}
    //! The stack under test
    StackBase& stack;
    //! The stack for modeling correct behavior
    std::vector<ItemType> modelStack;
    //! Get a random item
    static ItemType getRandomItem() { return STest::Pick::any(); }
    //! Test copy data from
    static void testCopyDataFrom(StackBase& s1, FwSizeType size1, StackBase& s2) {
        s1.clear();
        for (FwSizeType i = 0; i < size1; i++) {
            const auto status = s1.push(static_cast<U32>(i));
            ASSERT_EQ(status, Success::SUCCESS);
        }
        s2.copyDataFrom(s1);
        const auto capacity2 = s2.getCapacity();
        const FwSizeType size = FW_MIN(size1, capacity2);
        for (FwSizeType i = 0; i < size; i++) {
            ASSERT_EQ(s1.at(i), s2.at(i));
        }
    }
};

}  // namespace StackTest

}  // namespace Fw

#endif
