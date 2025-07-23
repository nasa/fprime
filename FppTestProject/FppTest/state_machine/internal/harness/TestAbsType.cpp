// ======================================================================
//
// \title  TestAbsType.cpp
// \author R. Bocchino
// \brief  An abstract type for testing (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/state_machine/internal/harness/TestAbsType.hpp"

namespace FppTest {

namespace SmHarness {

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const TestAbsType& obj) {
    Fw::String s;
    obj.toString(s);
    os << s;
    return os;
}
#endif

#if FW_SERIALIZABLE_TO_STRING
void TestAbsType::toString(Fw::StringBase& sb) const {
    static const char* formatString = "TestAbsType(%" PRIu32 ")";
    sb.format(formatString, this->m_data);
}
#endif

}  // namespace SmHarness

}  // namespace FppTest
