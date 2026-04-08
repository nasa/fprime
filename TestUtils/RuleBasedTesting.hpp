// ======================================================================
// \title  RuleBasedTesting.hpp
// \brief  Shared macros for Rule-Based Testing (RBT) in F Prime
//
// Background
// ----------
// F Prime rule-based testing (RBT) uses STest::Rule<Tester>, where Tester is
// usually the component tester class.  The STest infrastructure selects
// rules whose precondition() returns true and then calls action().
//
// While rules can be declared manually, the macro has several benefits:
// - It standardizes the naming convention for rule methods and labels.
// - It reduces boilerplate
// - It makes the F Prime test asserts (e.g. ASSERT_EVENTS_*, ASSERT_TLM_* etc.)
//   available inside rule method bodies, which is not straightforward manually.
//   This is because those asserts expand to `this->...`, where `this` needs
//   to be the tester instance, which is not the case in a classical STest::Rule
//
// Usage
// -----
// 1. Inside a ComponentTester class, declare each rule with FW_RBT_DEFINE_RULE:
//
//      FW_RBT_DEFINE_RULE(MyComponentTester, GroupName, RuleName);
//
//    This creates inside the tester class:
//      bool GroupName__RuleName__precondition() const;
//      void GroupName__RuleName__action();
//      struct GroupName__RuleName : STest::Rule<MyComponentTester> { ... };
//
// 2. Implement the method bodies in .cpp files (and add *.cpp to CMakeLists.txt !!)
//
// 3. In the test main, instantiate and use rule types as nested types of
//    the tester class:
//
//      MyComponentTester tester;
//      MyComponentTester::GroupName__RuleName rule;
//      rule.apply(tester);
//
// ======================================================================

#ifndef TestUtils_RuleBasedTesting_HPP
#define TestUtils_RuleBasedTesting_HPP

#include "STest/Rule/Rule.hpp"

// -----------------------------------------------------------------------
//! \def FW_RBT_DEFINE_RULE
//!
//! Defines everything needed for one rule inside a ComponentTester class:
//! 1) GroupName__RuleName__precondition() const — declaration
//! 2) GroupName__RuleName__action()             — declaration
//! 3) struct GroupName__RuleName : STest::Rule<TESTER_TYPE> — definition
//!
//! Place inside the tester class body; implement both method bodies in a
//! per-group .cpp file.
//!
//! \param TESTER_TYPE  The ComponentTester class (state passed to STest::Rule)
//! \param GROUP_NAME   Rule group: appears in method names and the rule label
//! \param RULE_NAME    Rule variant: appears in method names and the rule label
// -----------------------------------------------------------------------
#define FW_RBT_DEFINE_RULE(TESTER_TYPE, GROUP_NAME, RULE_NAME)                                      \
    bool GROUP_NAME##__##RULE_NAME##__precondition() const;                                         \
    void GROUP_NAME##__##RULE_NAME##__action();                                                     \
    struct GROUP_NAME##__##RULE_NAME : public STest::Rule<TESTER_TYPE> {                            \
        GROUP_NAME##__##RULE_NAME() : STest::Rule<TESTER_TYPE>(#GROUP_NAME "." #RULE_NAME) {}       \
                                                                                                    \
        bool precondition(const TESTER_TYPE& tester) override {                                     \
            return tester.GROUP_NAME##__##RULE_NAME##__precondition();                              \
        }                                                                                           \
                                                                                                    \
        void action(TESTER_TYPE& tester) override { tester.GROUP_NAME##__##RULE_NAME##__action(); } \
    }

#endif
