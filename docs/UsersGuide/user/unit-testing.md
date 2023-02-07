# Unit Testing in F´

Testing is an important part of flight software (FSW) development.
Testing is divided into two phases: i) unit testing, and ii) integration
testing. Unit testing tests the individual units, such as F′ components,
while integration testing tests the integrated system. Test framework
classes include the auto-generated *TesterBase*, the auto-generated
*GTestBase*, and the developer-written *Tester*. The testing phases and
test framework classes are discussed in further detail below.

## Unit Testing

Thorough unit testing is critical. It provides unit-level regression
tests, and makes integration easier since localized errors are caught
early and system-level issues only appear during integration.

F′ provides the support for unit testing at the component level. The
overall framework for unit testing is shown in Figure 1.

![Test](../media/test1.png)

**Figure 1.** Unit testing framework overview.

The goal of unit testing is to cover all component-level requirements and achieve close to 100% code coverage with a reasonable amount of
system state and path coverage. These requirements should drive the
tests. A record of how the tests cover the requirements should be
maintained. Mapping the tests to requirements can be recorded on a
spreadsheet, or in the actual test using comments or in the console
output mechanism.

To start, generate the test classes and add public test methods to the
tester. The *TesterBase* and the *GTestBase* are auto-generated, while
the *Tester* is developer-written from a generated template.

***TesterBase*** is the base class for testing a component and provides
a harness for unit tests. The *TesterBase* interface is the mirror image
of the component (*C*) under test. For each output port in *C* there is
an input port called a “from port,” and for each input port in *C* there
is an output port called a “to port.” For each “from port” there is a
history (*H*) of data received through a virtual input handler that
stores its argument into *H*. The *TesterBase* provides utility methods
for writing tests for the component. These include sending commands,
sending invocations onto ports, and getting and setting parameters and
time.

***GTestBase*** is derived from the *TesterBase* and includes headers
for the Google Test framework with F′ specific macros. It supports test
assertions, such as *ASSERT\_EQ(3, x)* to check that two values are
equal when writing tests. The F′ specific macros check the telemetry
received from the ports, the events received from the ports, and the
data received (user-defined) from the ports. The *GTestBase* is factored
into a separate class so its use is optional on systems that do not
support it.

***Tester*** is derived from the *GTestBase* and contains the component
under test as a member. The autocoder provides a template where the user
then adds tests as a public method, and also writes tests in a derived
class of the *Tester*.

Once the test class is generated, the user can begin to send commands,
check events and telemetry, check user-defined output ports, set
parameters and the time, build and run the unit tests from the
*component directory*, and finally analyze the code coverage from the
*component directory* after building and running the test. However, be
sure to review the analysis from the *test/ut directory*.

A standard approach to writing unit tests is to write a complete test
that covers the requirement. If there is overlap, refactoring into
functions is the preferred approach to avoid code duplication. A more
disciplined approach would be to write functions that test individual
behaviors. When writing test code treat unit testing as a programming
problem by applying similar style guidelines as the flight code. This
approach avoids less code duplication and provides more readable,
maintainable, and modifiable tests.

When writing unit tests for a component be sure to test against the
interface, such as the send commands and the send data on the output
ports. Read the internal component state to verify it is good, and only
modify the state through the interface; do not update the state of the
component. This approach leads to a more structured test. If there is a
requirement to test a function in a component implementation that has a
complex algorithm, then write a test against the function interface.

When unit testing a component, model the external behavior to receive
commands and send responses by writing a test harness. This approach
supports modularity testing that can be used for many tests.

To check event and telemetry histories the user first sends a command,
and then checks events and telemetry by writing the following code.

**Sending Commands:**

```
// Send command
this->sendCOMMAND_NAME(
    cmdSeq, // Command sequence number
    arg1, // Argument 1
    arg2 // Argument 2
);

this->component.doDispatch();
// Assert command response
ASSERT_CMD_RESPONSE\_SIZE(1);
ASSERT_CMD_RESPONSE(
    0, // Index in the history
    Component::OPCODE\_COMMAND\_NAME, // Expected command opcode
    cmdSeq, // Expected command sequence number
    Fw::CmdResponse::OK // Expected command response
}
```

**Checking Events:**

```
// Send command and check response
…

// Assert total number of events in history
ASSERT_EVENTS_SIZE(1);

// Assert number of a particular event
ASSERT_EVENTS_EventName_SIZE(1);

// Assert arguments for a particular event
ASSERT_EVENTS_EventName(
    0, // Index in history
    arg1, // Expected value of argument 1
    arg2 // Expected value of argument 2
);
```

**Checking Telemetry:**
```
// Send command and check response
…

// Assert total number of telemetry entries in history
ASSERT_TLM_SIZE(1);

// Assert number of entries on a particular channel
ASSERT_TLM_ChannelName_SIZE(1);

// Assert value for a particular entry
ASSERT_TLM_ChannelName(
    0, // Index in history
    value // Expected value
);
```
To check the user-defined output ports write the following code.
```
// Send command and check response
…

// Assert total number of entries on from ports
ASSERT_FROM_PORT_HISTORY_SIZE(1);

// Assert number of entries on a particular from port
ASSERT_from_PortName_SIZE(1);

// Assert value for a particular entry
ASSERT_from_PortName(
    0, // Index in history
    arg1, // Expected value of argument 1
    arg2 // Expected value of argument 2
);
```

To set the parameters in a test of component *C*, write the following
code. This call stores the argument in member variables of *TesterBase*,
so when *C* invokes the *ParamGet* port it receives the argument.

```
this->paramSet_ParamName(
    value, // Parameter value
    Fw::PARAM_VALID // Parameter status
);
```

Next, to set the time in a text of component *C*, write the following
code. *Time* is an *Fw::Time* object, so when *C* invokes the *TimeGet*
port it receives the value time.

`this->setTime(time)`

The F′ Prime build system provides targets for building and running
component unit tests.

To build unit tests, go to the component directory (not the *test/ut*
directory) and run `fprime-util generate --ut`.

To run unit tests, go to the component directory (not the *test/ut*
directory) and run `fprime-util check [parameter flags]`.

Unit test check parameter | Description
---|---
`--all` | Run all unit tests, combinable with `leak` or `coverage`
`--coverage` | Check for code coverage in unit tests
`--leak` | Check for memory leaks in unit tests

For example, to run all unit tests and check for code coverage, run `fprime-util check --all --coverage`.

### Choosing a test library

Components that call into libraries have two ways to write tests:

- Link against the library in the test
- Link against a mock or stub library

If you link against the library in the test, avoid linking against the
mock or stub library. Linking against only the test library proves that the component code works with the actual library.

Linking against a mock or stub library makes it easier to induce
behaviors for testing, like injecting faults. This approach may be
the only option on some platforms.

### Code coverage

Code coverage checks which lines were run at least once during
a test. Tools like *gcov* perform code coverage analysis by compiling and running the tests, then producing a report.

Generally, code coverage checks close to 80% of lines. The remaining
lines are usually off-nominal behaviors that may require additional
effort to check by reverse reasoning from the desired behavior to
synthesize the inputs, or by injecting faults into the library
behaviors.

Note that 100% code coverage does not check which system states were tested, nor which paths through the code were tested.

To review code coverage analysis, go to the component directory and review the summary output *\_gcov.txt* files. Next, go to the component directory to review the coverage annotation *.hpp.gcov* and *.cpp.gcov* source files.