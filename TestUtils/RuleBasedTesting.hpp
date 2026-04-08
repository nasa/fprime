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
// 2. Define each rule inside a ComponentTester with FW_RBT_DEFINE_RULE:
//
//      FW_RBT_DEFINE_RULE(MyComponentTester, GroupName, RuleName)
//
//    This creates:
//      bool GroupName__RuleName__precondition() const;
//      void GroupName__RuleName__action();
//      struct GroupName__RuleName : STest::Rule<TestState> { ... };
//
// 3. Implement the method bodies in per-group .cpp files. Inside each
//    body, `this` is a TestState pointer, so ASSERT_* macros are direct.
//
// ======================================================================

#ifndef TestUtils_RuleBasedTesting_HPP
#define TestUtils_RuleBasedTesting_HPP

#include "STest/Rule/Rule.hpp"

// -----------------------------------------------------------------------
//! \def FW_RBT_DEFINE_RULE
//!
//! Defines everything needed for one rule inside a TestState class:
//! 1) precondition declaration
//! 2) action declaration
//! 3) a nested STest::Rule<STATE_TYPE> subclass named GROUP_NAME__RULE_NAME
//!
//! This allows users to keep all rule declarations and rule types in one
//! place (the tester header) without creating a separate Rules.hpp file.
//!
//! \param STATE_TYPE  The TestState type used by STest::Rule (usually a ComponentTester class)
//! \param GROUP_NAME  Rule group: used in method/rule names and rule label
//! \param RULE_NAME   Rule variant: used in method/rule names and rule label
// -----------------------------------------------------------------------
#define FW_RBT_DEFINE_RULE(STATE_TYPE, GROUP_NAME, RULE_NAME)                                    \
    bool GROUP_NAME##__##RULE_NAME##__precondition() const;                                      \
    void GROUP_NAME##__##RULE_NAME##__action();                                                  \
    struct GROUP_NAME##__##RULE_NAME : public STest::Rule<STATE_TYPE> {                          \
        GROUP_NAME##__##RULE_NAME() : STest::Rule<STATE_TYPE>(#GROUP_NAME "." #RULE_NAME) {}     \
                                                                                                 \
        bool precondition(const STATE_TYPE& state) override {                                    \
            return state.GROUP_NAME##__##RULE_NAME##__precondition();                            \
        }                                                                                        \
                                                                                                 \
        void action(STATE_TYPE& state) override { state.GROUP_NAME##__##RULE_NAME##__action(); } \
    }

// -----------------------------------------------------------------------
//! \def FW_RBT_IMPLEMENT_RULE
//!
//! Defines an STest::Rule<STATE_TYPE> subclass named RULE_NAME inside
//! namespace GROUP_NAME.  The subclass forwards precondition() and
//! action() calls to the matching methods on the STATE_TYPE object,
//! which must be declared with FW_RBT_DECLARE_RULE and implemented
//! in a per-group .cpp file.
//!
//! \param STATE_TYPE  The TestState type used by STest::Rule
//! \param GROUP_NAME  Rule group: becomes a C++ namespace and name prefix
//! \param RULE_NAME   Rule variant: becomes the struct name and name suffix
// -----------------------------------------------------------------------
#define FW_RBT_IMPLEMENT_RULE(STATE_TYPE, GROUP_NAME, RULE_NAME)                                                 \
    namespace GROUP_NAME {                                                                                       \
                                                                                                                 \
    struct RULE_NAME : public STest::Rule<STATE_TYPE> {                                                          \
        RULE_NAME() : Rule<STATE_TYPE>(#GROUP_NAME "." #RULE_NAME) {}                                            \
                                                                                                                 \
        bool precondition(const STATE_TYPE& state) { return state.GROUP_NAME##__##RULE_NAME##__precondition(); } \
                                                                                                                 \
        void action(STATE_TYPE& state) { state.GROUP_NAME##__##RULE_NAME##__action(); }                          \
    };                                                                                                           \
    }

// -----------------------------------------------------------------------
//! \def FW_RBT_DECLARE_RULE
//!
//! Declares a precondition/action method pair in a TestState class.
//! The precondition is const; the action is non-const.
//! Use this inside the TestState class body; implement both bodies in a
//! per-group .cpp file.
//!
//! \param GROUP_NAME  Must match the GROUP_NAME in FW_RBT_IMPLEMENT_RULE
//! \param RULE_NAME   Must match the RULE_NAME in FW_RBT_IMPLEMENT_RULE
// -----------------------------------------------------------------------
#define FW_RBT_DECLARE_RULE(GROUP_NAME, RULE_NAME)          \
    bool GROUP_NAME##__##RULE_NAME##__precondition() const; \
    void GROUP_NAME##__##RULE_NAME##__action();

#endif
