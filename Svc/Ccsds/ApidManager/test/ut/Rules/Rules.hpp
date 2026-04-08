// ======================================================================
// \title  Rules.hpp
// \author thomas-bc
// \brief  Rule definitions for ApidManager rule-based testing
//
// FW_RBT_IMPLEMENT_RULE(ApidManagerTester, GROUP, RULE) creates a
// STest::Rule<ApidManagerTester> subclass in namespace GROUP named RULE.
// Its precondition() and action() delegate to the matching methods on the
// TestState object.  Those methods are declared in TestState.hpp via
// FW_RBT_DECLARE_RULE and implemented in the per-group .cpp files
// listed below.
//
// Rule groups
// -----------
//   GetSeqCount      — exercises getApidSeqCountIn     (GetSeqCount.cpp)
//   ValidateSeqCount — exercises validateApidSeqCountIn (ValidateSeqCount.cpp)
// ======================================================================

#ifndef Svc_Ccsds_ApidManager_Rules_HPP
#define Svc_Ccsds_ApidManager_Rules_HPP

#include "Svc/Ccsds/ApidManager/test/ut/ApidManagerTester.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

namespace Ccsds {

namespace Rules {

// ----------------------------------------------------------------------
// GetSeqCount rules
// ----------------------------------------------------------------------

//! Get the sequence count for an APID that is already tracked
FW_RBT_IMPLEMENT_RULE(ApidManagerTester, GetSeqCount, Existing)

//! Register and get the sequence count for a new APID (table not full)
FW_RBT_IMPLEMENT_RULE(ApidManagerTester, GetSeqCount, NewOk)

//! Attempt to get the sequence count for a new APID when the table is full
FW_RBT_IMPLEMENT_RULE(ApidManagerTester, GetSeqCount, NewTableFull)

// ----------------------------------------------------------------------
// ValidateSeqCount rules
// ----------------------------------------------------------------------

//! Validate the correct (expected) sequence count — no event expected
FW_RBT_IMPLEMENT_RULE(ApidManagerTester, ValidateSeqCount, Ok)

//! Validate a wrong sequence count — UnexpectedSequenceCount event expected
FW_RBT_IMPLEMENT_RULE(ApidManagerTester, ValidateSeqCount, Failure)

}  // namespace Rules

}  // namespace Ccsds

}  // namespace Svc

#endif
