// ======================================================================
// \title  ArraySetOrMapImplTestRules.cpp
// \author Rob Bocchino
// \brief  cpp file for ArraySetOrMapImpl test rules
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/ArraySetOrMapImplTestRules.hpp"

namespace Fw {

namespace ArraySetOrMapImplTest {

namespace Rules {

InsertNotFull insertNotFull;

InsertFull insertFull;

At at;

RemoveExisting removeExisting;

#if 0
PopEmpty popEmpty;

Clear clear;
#endif

};  // namespace Rules

}  // namespace ArraySetOrMapImplTest

}  // namespace Fw
