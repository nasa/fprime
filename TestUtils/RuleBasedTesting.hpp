// ======================================================================
// \title  RuleBasedTesting.hpp
// \brief  Shared macros for Rule-Based Testing (RBT) in F Prime
//
// Background
// ----------
// F Prime rule-based testing uses STest::Rule<State>, where State is a
// concrete TestState class.  The STest infrastructure selects rules
// whose precondition() returns true and then calls action().
//
// The generated ASSERT_EVENTS_*, ASSERT_TLM_*, invoke_to_*, etc. macros
// all expand to `this->someMethod(...)`.  They only compile in a context
// where `this` is the component tester (or a subclass of it).
//
// Common patterns
// ---------------
// 1) Composition-first (recommended for readability)
//      class TestState { MyComponentTester tester; ... };
//    Rule methods call tester APIs explicitly and use explicit forwarding
//    helpers for event/tlm assertions.
//
// 2) Inheritance-first (macro convenience)
//      class TestState : public MyComponentTester { ... };
//    Rule methods can call generated ASSERT_* macros directly.
//
// This header is intentionally neutral and supports both patterns.
//
// Usage
// -----
// 1. Create a TestState class (composition or inheritance).
//
// 2. Declare one method pair per rule in TestState using FW_TEST_STATE_DEF_RULE:
//
//      FW_TEST_STATE_DEF_RULE(GroupName, RuleName)
//
//    This expands to:
//      bool precondition__GroupName__RuleName() const;
//      void action__GroupName__RuleName();
//
// 3. In Rules.hpp, define the STest::Rule subclass for each rule:
//
//      FW_RULES_DEF_RULE(TestState, GroupName, RuleName)
//
//    This creates:
//      namespace GroupName {
//        struct RuleName : STest::Rule<TestState> { ... };
//      }
//
// 4. Implement the method bodies in per-group .cpp files.  Inside each
//    body, `this` is a TestState pointer, so ASSERT_* macros are direct.
//
// ======================================================================

#ifndef TestUtils_RuleBasedTesting_HPP
#define TestUtils_RuleBasedTesting_HPP

#include "STest/Rule/Rule.hpp"

// -----------------------------------------------------------------------
//! \def FW_RULES_DEF_RULE
//!
//! Defines an STest::Rule<STATE_TYPE> subclass named RULE_NAME inside
//! namespace GROUP_NAME.  The subclass forwards precondition() and
//! action() calls to the matching methods on the STATE_TYPE object,
//! which must be declared with FW_TEST_STATE_DEF_RULE and implemented
//! in a per-group .cpp file.
//!
//! \param STATE_TYPE  The TestState type used by STest::Rule
//! \param GROUP_NAME  Rule group: becomes a C++ namespace and name prefix
//! \param RULE_NAME   Rule variant: becomes the struct name and name suffix
// -----------------------------------------------------------------------
#define FW_RULES_DEF_RULE(STATE_TYPE, GROUP_NAME, RULE_NAME)                                                     \
    namespace GROUP_NAME {                                                                                       \
                                                                                                                 \
    struct RULE_NAME : public STest::Rule<STATE_TYPE> {                                                          \
        RULE_NAME() : Rule<STATE_TYPE>(#GROUP_NAME "." #RULE_NAME) {}                                            \
                                                                                                                 \
        bool precondition(const STATE_TYPE& state) { return state.precondition__##GROUP_NAME##__##RULE_NAME(); } \
                                                                                                                 \
        void action(STATE_TYPE& state) { state.action__##GROUP_NAME##__##RULE_NAME(); }                          \
    };                                                                                                           \
    }

// -----------------------------------------------------------------------
//! \def FW_TEST_STATE_DEF_RULE
//!
//! Declares a precondition/action method pair in a TestState class.
//! The precondition is const; the action is non-const.
//! Use this inside the TestState class body; implement both bodies in a
//! per-group .cpp file.
//!
//! \param GROUP_NAME  Must match the GROUP_NAME in FW_RULES_DEF_RULE
//! \param RULE_NAME   Must match the RULE_NAME in FW_RULES_DEF_RULE
// -----------------------------------------------------------------------
#define FW_TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)       \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

#endif
