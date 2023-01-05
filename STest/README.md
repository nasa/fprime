STest is a framework that sets the stage for [rule](#rule)-based unit testing within a certain [scenario](#scenario).

- [Rule](#rule)
- [State](#state)
- [Scenario](#scenario)
- [Use in Fprime](#use-in-fprime)

<hr>

### Rule
An interface by [definition](./STest/Rule/Rule.hpp), requiring three functions in the class or struct that implements it:
- A constructor for initializing the name of the rule.
- A definition for the pure virtual function `action` which contains an implementation of the logic that aligns with the purpose of the rule.
- A definition for the pure virtual function `precondition` which would act as a binary switch (returning a boolean) in determining whether the `action` associated with the rule should be applied or not, based on a defined condition. 

> Both `precondition` and `action` take as input the [state](#state) of the system by reference.

### State
A user-defined type implemented as a class representing a property of the component or system under test. State modifications are made through the rule interface as the state of components shouldn't be altered directly.

### Scenario
A recipe for using a set of rules to construct one or more tests. All scenarios are implemented as a derived class of the base class [Scenario](./STest/Scenario/Scenario.hpp). Examples of scenarios include the following: 
- Applying a set of rules in a sequence which is either fixed ([RuleSequenceScenario](./STest/Scenario/RuleSequenceScenario.hpp)) or randomized ([RandomScenario](./STest/Scenario/RandomScenario.hpp))
- Repeatedly applying a rule within a scenario ([RepeatedRuleScenario](./STest/Scenario/RepeatedRuleScenario.hpp))
- Running a scenario while a condition holds ([ConditionalScenario](./STest/Scenario/ConditionalScenario.hpp))
- Creating a sequence of scenarios ([SequenceScenario](./STest/Scenario/SequenceScenario.hpp)) and randomly selecting one of them to run ([SelectedScenario](./STest/Scenario/SelectedScenario.hpp))
- Randomly interleaving a set of scenarios ([InterleavedScenario](./STest/Scenario/InterleavedScenario.hpp))
- Iterating over a collection of scenarios ([IteratedScenario](./STest/Scenario/IteratedScenario.hpp)) or over a particular one till a certain condition holds ([ConditionalIteratedScenario](./STest/Scenario/ConditionalIteratedScenario.hpp))
- Running an iterated scenario with a specific bound on the number of iterations ([BoundedIteratedScenario](./STest/Scenario/BoundedIteratedScenario.hpp))
- Running a scenario for a certain bound, with the range being either fixed ([BoundedScenario](./STest/Scenario/BoundedScenario.hpp)) or randomly selected ([RandomlyBoundedScenario](./STest/Scenario/RandomlyBoundedScenario.hpp))

### Use in Fprime
At present, F' uses STest to construct rules specific to components (examples include [Svc/GroundInterface](https://github.com/nasa/fprime/blob/master/Svc/GroundInterface/test/ut/GroundInterfaceRules.cpp) and [Fw/Logger](https://github.com/nasa/fprime/blob/master/Fw/Logger/test/ut/LoggerRules.cpp)) with successive use of them within test cases that either test for the execution of each rule individually or in a combined setup through scenarios that build upon them.

The scenarios used here are [RandomScenario](./STest/Scenario/RandomScenario.hpp) and [BoundedScenario](./STest/Scenario/BoundedScenario.hpp) (objects of which are instantiated respectively within the test runner), constituting a random hopper where the execution of an array of rules in a randomly generated sequence (source of randomness<sup>1</sup> being [this function](./STest/Random/bsd_random.c#L394)) takes place over a specified number of times. (Since this repetitive action of executing a set of rules in an *arbitrary arrangement* is what's being achieved as the final testing scenario, this process can be referred to as 'rule-based *random* testing'.)

> <sup>1</sup>for generating the array indices in a random order

The rule-based testing in F Prime is integrated into the standard F Prime unit tests methodology, which uses the the GoogleTest framework.
