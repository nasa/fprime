## STest

STest is a framework for using **rules** and **scenarios** to construct and run unit tests.
The concepts of "rules" and "scenarios" are described below.
Using rules and scenarios provides structure to unit tests.
By automatically generating tests, scenarios can also provide more coverage than is possible
with manually-written tests.

- [State](#state)
- [Rule](#rule)
- [Scenario](#scenario)
- [Use in F Prime](#use-in-f-prime)

<hr>

### State

In rule-based testing, **state** refers to the current state of a running test. It usually includes **concrete state** (the actual state of the system under test) and **test state** or **abstract state** (state that abstracts the state of the system under test, to assist with modeling).

In the STest implementation of rule-based testing, `State` is a user-defined type. The `Rule` and `Scenario` classes discussed below are templates that are parameterized over this type.

### Rule

A **rule** is a unit of test code that specifies and checks some behavior of the system under test.
It has two elements:

1. A **precondition** that specifies when the rule may be applied.
A precondition is a predicate function (i.e., a read-only function that returns a Boolean value) on
the [system state](#state).
For example, consider a rule that exercises the nominal behavior of a buffer allocator.
The precondition for this rule might specify that a buffer is available for allocation.
On the other hand, the precondition for a rule that exercises the error case "buffer not available"
might specify that all buffers have been allocated.

2. An **action** that (a) causes the system under test to do something and (b) checks that the resulting
behavior is as expected, given the current system state.
For example, a "nominal" or "OK" rule for a buffer allocator might check that a buffer was allocated.
A "buffer not available" rule might check that the appropriate warning is emitted as an F' event.

When using STest, you specify rules as derived classes of the abstract class [Rule](./STest/Rule/Rule.hpp).
You specify the precondition and action by overriding pure virtual functions specified in the `Rule` interface.

### Scenario
A **scenario** is a recipe for using a set of rules to construct one or more tests. Examples of scenarios include the following:
- Applying a set of rules in a sequence which is either fixed ([RuleSequenceScenario](./STest/Scenario/RuleSequenceScenario.hpp)) or randomized ([RandomScenario](./STest/Scenario/RandomScenario.hpp))
- Repeatedly applying a rule within a scenario ([RepeatedRuleScenario](./STest/Scenario/RepeatedRuleScenario.hpp))
- Running a scenario while a condition holds ([ConditionalScenario](./STest/Scenario/ConditionalScenario.hpp))
- Creating a sequence of scenarios ([SequenceScenario](./STest/Scenario/SequenceScenario.hpp)) and randomly selecting one of them to run ([SelectedScenario](./STest/Scenario/SelectedScenario.hpp))
- Randomly interleaving a set of scenarios ([InterleavedScenario](./STest/Scenario/InterleavedScenario.hpp))
- Iterating over a collection of scenarios ([IteratedScenario](./STest/Scenario/IteratedScenario.hpp)) or over a particular one till a certain condition holds ([ConditionalIteratedScenario](./STest/Scenario/ConditionalIteratedScenario.hpp))
- Running an iterated scenario with a specific bound on the number of iterations ([BoundedIteratedScenario](./STest/Scenario/BoundedIteratedScenario.hpp))
- Running a scenario for a certain bound, with the range being either fixed ([BoundedScenario](./STest/Scenario/BoundedScenario.hpp)) or randomly selected ([RandomlyBoundedScenario](./STest/Scenario/RandomlyBoundedScenario.hpp))

The iterated and random scenarios allow you to construct complex tests that explore many behaviors, using relatively simple specifications. Typically the tests constructed with these scenarios are not feasible to write by hand (e.g., because they apply thousands or millions of rules).

### Use in F Prime

At present, F Prime uses STest for unit testing of components. An example is [Fw/Logger](https://github.com/nasa/fprime/blob/master/Fw/Logger/test/ut/LoggerRules.cpp). The main scenarios used here are
as follows:

* Short sequences of rules that set up the system state and then test a particular rule.
* Random scenarios for automatically generating more complex tests.
