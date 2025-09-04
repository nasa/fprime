// ======================================================================
// \title  SetTestState.hpp
// \author bocchino
// \brief  hpp file for set test state
// ======================================================================

#ifndef SetTestState_HPP
#define SetTestState_HPP

#include <gtest/gtest.h>

#include <set>

#include "Fw/DataStructures/SetBase.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace SetTest {

struct State {
    //! The element type
    using ElementType = U32;
    //! The set capacity
    static constexpr FwSizeType capacity = 1024;
    //! The SetBase type
    using SetBaseType = SetBase<ElementType>;
    //! Constructor
    State(SetBaseType& a_set) : set(a_set) {}
    //! The set under test
    SetBaseType& set;
    //! The set for modeling correct behavior
    std::set<ElementType> modelSet;
    //! Whether to use the stored element
    bool useStoredElement = false;
    //! The stored element
    ElementType storedElement = 0;
    //! Get an element
    ElementType getElement() const {
        return useStoredElement ? storedElement : static_cast<ElementType>(STest::Pick::any());
    }
    //! Check whether the model set contains the specified element
    bool modelSetContains(ElementType e) const { return modelSet.count(e) != 0; }
    //! Test copy data from
    static void testCopyDataFrom(SetBaseType& m1, FwSizeType size1, SetBaseType& m2) {
        m1.clear();
        for (FwSizeType i = 0; i < size1; i++) {
            const auto status = m1.insert(static_cast<ElementType>(i));
            ASSERT_EQ(status, Success::SUCCESS);
        }
        m2.copyDataFrom(m1);
        const auto capacity2 = m2.getCapacity();
        const FwSizeType size = FW_MIN(size1, capacity2);
        for (FwSizeType i = 0; i < size; i++) {
            const auto e = static_cast<ElementType>(i);
            const auto status = m2.find(e);
            ASSERT_EQ(status, Success::SUCCESS);
        }
    }
};

}  // namespace SetTest

}  // namespace Fw

#endif
