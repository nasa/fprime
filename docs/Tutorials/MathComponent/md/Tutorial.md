# F' Math Component Tutorial

!include toc.md

## Introduction

This tutorial shows how to develop, test, and deploy a simple topology
consisting of two components:

1. `MathSender`: A component that receives commands and forwards work to
   `MathReceiver`.

1. `MathReceiver`: A component that carries out arithmetic operations and
   returns the results to `MathSender`.

See the diagram below.

<a name="math-top"></a>
![A simple topology for arithmetic computation](png/top.png)

**What is covered:** The tutorial covers the following concepts:

1. Using the [FPP modeling language](https://fprime-community.github.io/fpp) to
   specify the types and ports used by the components.

1. Using the F Prime build system to build the types and ports.

1. Developing the `MathSender` component: Specifying the component, building
   the component, completing the C++ component implementation, and writing
   component unit tests.

1. Developing the `MathReceiver` component.

1. Adding the new components and connections to the F Prime `Ref` application.

1. Using the F Prime Ground Data System (GDS) to run the updated `Ref`
   application.

**Prerequisites:** This tutorial assumes the following:

1. Basic knowledge of Unix: How to navigate in a shell and execute programs.

1. Basic knowledge of git: How to create a branch.

1. Basic knowledge of C++, including class declarations, inheritance,
and virtual functions.

If you have not yet installed F Prime on your system, do so now.
Follow the installation guide at `INSTALL.md`
in the [F Prime git repository](https://github.com/nasa/fprime).
You may also wish to work through the Getting Started tutorial at
`docs/GettingStarted/Tutorial.md`.

**Version control:**
Working on this tutorial will modify some files under version control
in the F Prime git repository.
Therefore it is a good idea to do this work on a new branch.
For example:

```bash
git checkout -b math-tutorial
```

If you wish, you can save your work by committing to this branch.

## The MathOp Type

In F Prime, a **type definition** defines a kind of data that you can pass
between components or use in commands and telemetry.

For this tutorial, we need one type definition.
It defines an enumeration called `MathOp`, which
represents a mathematical operation.

We will add the specification for the `MathOp` type to the
`Ref` topology.
We will do this in three stages:

1. Construct the FPP model.

1. Add the model to the project.

1. Build the model.

### Construct the FPP Model

**Create the MathTypes directory:**
Go to the directory `Ref` at the top-level of the
F Prime repository and run `mkdir MathTypes`.
This step creates a new directory `Ref/MathTypes`.
This directory will contain our new type.

**Create the FPP model file:**
Now go into the directory `Ref/MathTypes`.
In that directory, create a file `MathTypes.fpp` with the following contents:

```fpp
!include ../MathTypes/MathTypes.fpp
```

You can do this by typing, or by copy-paste.

This file defines an enumeration or **enum** with enumerated constants `ADD`,
`SUB`, `MUL`, and `DIV`.
These four constants represent the operations of addition, subtraction,
multiplication, and division.
The enum also defines a type `MathOp`; the enumerated constants are the values
of this type.
For more information on enums, see [_The FPP User's 
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Enums).

The enum `MathTypes` resides in an FPP module `Ref`.

An FPP module is like a C++ namespace: it encloses several definitions, each of
which is qualified with the name of the module.
For more information on FPP modules, see [_The FPP User's
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Modules).

The text following a symbol `@` or `@<` is called an **annotation**.
These annotations are carried through the parsing and become comments in the
generated code.
For more information, see [_The FPP User's
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Writing-Comments-and-Annotations).

<a name="types_add"></a>
### Add the Model to the Project

**Create Ref/MathTypes/CMakeLists.txt:**
Create a file `Ref/MathTypes/CMakeLists.txt` with the following contents:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathTypes.fpp"
)

register_fprime_module()
```

This code will tell the build system how to build the FPP model.

**Update Ref/CMakeLists.txt:**
Now we need to add the new directory to the `Ref` project.
To do that, open the file `Ref/CMakeLists.txt`.
This file should already exist; it was put there by the developers
of the `Ref` topology.
In this file, you should see several lines starting with `add_fprime_subdirectory`.
Immediately after the last of those lines, add the following new line:

```cmake
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathTypes/")
```

<a name="types_build"></a>
### Build the Model

**Run the build:**
Do the following:

1. Go to the directory `Ref/MathTypes`.

1. If you have not already run `fprime-util generate`, then do so now.

1. Run the command `fprime-util build`.

The output should indicate that the model built without any errors.
If not, try to identify and correct what is wrong,
either by deciphering the error output, or by going over the steps again.
If you get stuck, you can look at the
<a href="#types_ref">reference implementation</a>.

**Inspect the generated code:**
Now go to the directory `Ref/build-fprime-automatic-native/Ref/MathTypes`
(you may want to use `pushd`, or do this in a separate shell,
so you don't lose your current working directory).
The directory `build-fprime-automatic-native` is where all the
generated code lives for the "automatic native" build of the `Ref`
project.
Within that directory is a directory tree that mirrors the project
structure.
In particular, `Ref/build-fprime-automatic-native/Ref/MathTypes`
contains the generated code for `Ref/MathTypes`.

Run `ls`.
You should see something like this:

```bash
CMakeFiles            MathOpEnumAc.cpp      MathOpEnumAi.xml.prev cmake_install.cmake
Makefile              MathOpEnumAc.hpp      autocoder
```

The files `MathOpEnumAc.hpp` and
`MathOpEnumAc.cpp` are the auto-generated C++ files
corresponding to the `MathOp` enum.
You may wish to study the file `MathOpEnumAc.hpp`.
This file gives the interface to the C++ class `Ref::MathOp`.
All enum types have a similar auto-generated class
interface.

<a name="types_ref"></a>
### Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/MathTypes`.
To build this implementation from a clean repository,
do the following:

1. Go to the `Ref` directory.

1. Run `cp -R ../docs/Tutorials/MathComponent/MathTypes .`

1. Update `Ref/CMakeLists.txt` as stated <a href="#types_add">above</a>.

1. Follow the steps for <a href="#types_build">building the model</a>.

If you have modified the repo, revise the steps accordingly.
For example, switch git branches, use `git stash` to stash
your changes, or move `MathTypes` to another directory such
as `MathTypes-saved`.

<a name="ports"></a>
## The MathOp and MathResult Ports

A **port** is the endpoint of a connection between
two components.
A **port definition** is like a function signature;
it defines the type of the data carried on a port.

For this tutorial, we need two port definitions:

* `MathOp` for sending an arithmetic operation request from
`MathSender` to `MathReceiver`.

* `MathResult` for sending the result of an arithmetic
operation from `MathReceiver` to `MathSender`.

We follow the same three steps as in the previous section.

### Construct the FPP Model

**Create the MathPorts directory:**
Go to the directory `Ref` at the top-level of the
F Prime repository and run `mkdir MathPorts`.
This directory will contain our new ports.

**Create the FPP model file:**
Now go into the directory `Ref/MathPorts`.
Create a file `MathPorts.fpp` with the following contents:

```fpp
!include ../MathPorts/MathPorts.fpp
```

This file defines the ports `MathOp` and `MathResult`.
`MathOp` has three formal parameters: a first operand, an
operation, and a second operand.
The operands have type `F32`, which represents a 32-bit
floating-point number.
The operation has type `MathOp`, which is the enum type
we defined in the previous section.
`MathResult` has a single formal parameter, the value of type `F32`
returned as the result of the operation.

For more information about port definitions, see
[_The FPP User's Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Ports).

### Add the Model to the Project

Add add the model
`Ref/MathPorts/MathPorts.fpp` to the `Ref` project.
Carry out the steps in the
<a href="#types_add">previous section</a>, after
substituting `MathPorts` for `MathTypes`.

### Build the Model

Carry out the steps in the
<a href="#types_build">previous section</a>,
in directory `MathPorts` instead of `MathTypes`.
The generated code will go in
`Ref/build-fprime-automatic-native/Ref/MathPorts`.
For port definitions, the names of the auto-generated C++
files end in `PortAc.hpp` and `PortAc.cpp`.
You can look at this code if you wish.
However, the auto-generated C++ port files are used
by the autocoded component implementations (described below);
you won't ever program directly against their interfaces.

### Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/MathPorts`.
To build this implementation, follow the steps
described for <a href="#types_ref">`MathTypes`</a>.

<a name="math-sender"></name>
## The MathSender Component

Now we can build and test the `MathSender` component.
There are five steps:

1. Construct the FPP model.
1. Add the model to the project.
1. Build the stub implementation.
1. Complete the implementation.
1. Write and run unit tests.

### Construct the FPP Model

**Create the MathSender directory:**
Go to the directory `Ref` at the top-level of the
F Prime repository.
Run `mkdir MathSender` to create a directory for the new component.

**Create the FPP model file:**
Now go into the directory `Ref/MathSender`.
Create a file `MathSender.fpp` with the following contents:

```fpp
!include ../MathSender/MathSender.fpp
```

This code defines a component `Ref.MathSender`.
The component is **active**, which means it has its
own thread.

Inside the definition of the `MathSender` component are
several specifiers.
We have divided the specifiers into five groups:

1. **General ports:** These are user-defined ports for
application-specific functions.
There are two general ports: an output port `mathOpOut`
of type `MathOp` and an input port `mathResultIn` of
type `MathResult`.
Notice that these port specifiers use the ports that
we defined <a href="#ports">above</a>.
The input port is **asynchronous**.
This means that invoking the port (i.e., sending
data on the port) puts a message on a queue.
The handler runs later, on the thread of this component.

1. **Special ports:** These are ports that have a special
meaning in F Prime.
There are ports for registering commands with the dispatcher,
receiving commands, sending command responses, emitting
event reports, emitting telemetry, and getting the time.

1. **Commands:** These are commands sent from the ground
or from a sequencer and dispatched to this component.
There is one command `DO_MATH` for doing a math operation.
The command is asynchronous.
This means that when the command arrives, it goes on a queue
and its handler is later run on the thread of this component.

1. **Events:** These are event reports that this component
can emit.
There are two event reports, one for receiving a command
and one for receiving a result.

1. **Telemetry:** These are **channels** that define telemetry
points that the this component can emit.
There are four telemetry channels: three for the arguments
to the last command received and one for the last
result received.

For more information on defining components, see
[_The FPP User's Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Components).

<a name="math-sender_add-model"></a>
### Add the Model to the Project

**Create Ref/MathSender/CMakeLists.txt:**
Create a file `Ref/MathSender/CMakeLists.txt` with the following contents:

```cmake
# Register the standard build
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSender.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MathSender.fpp"
)
register_fprime_module()
```

This code will tell the build system how to build the FPP model
and component implementation.

**Update Ref/CMakeLists.txt:**
Add `Ref/MathSender` to `Ref/CMakeLists.txt`, as we did
for <a href="#types_add">`Ref/MathTypes`</a>.

<a name="math-sender_build-stub"></a>
### Build the Stub Implementation

**Run the build:**
Go into the directory `Ref/MathSender`.
Run the following commands:

```bash
touch MathSender.cpp
fprime-util impl
```

The first command creates an empty file `MathSender.cpp`.
The build rules we wrote in the previous section expect
this file to be there.
After the second command, the build system should
run for a bit.
At the end there should be two new files
in the directory:
`MathSender.cpp-template` and
`MathSender.hpp-template`.

Run the following commands:

```bash
mv MathSender.cpp-template MathSender.cpp
mv MathSender.hpp-template MathSender.hpp
```

These commands produce a template, or stub implementation,
of the `MathSender` implementation class.
You will fill in this implementation class below.

Now run the command `fprime-util build --jobs 4`.
The model and the stub implementation should build.
The option `--jobs 4` says to use four cores for the build.
This should make the build go faster.
You can use any number after `--jobs`, up to the number
of cores available on your system.

**Inspect the generated code:**
The generated code resides in the directory
`Ref/fprime-build-automatic-native-ut/Ref/MathSender`.
You may wish to look over the file `MathSenderComponentAc.hpp`
to get an idea of the interface to the auto-generated
base class `MathSenderComponentBase`.
The `MathSender` implementation class is a derived class
of this base class.

### Complete the Implementation

Now we can complete the stub implementation.
In an editor, open the file `MathSender.cpp`.

**Fill in the DO_MATH command handler:**
You should see a stub handler for the `DO_MATH`
command that looks like this:

```c++
void MathSender ::
  DO_MATH_cmdHandler(
      const FwOpcodeType opCode,
      const U32 cmdSeq,
      F32 val1,
      MathOp op,
      F32 val2
  )
{
  // TODO
  this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}
```

The handler `DO_MATH_handler` is called when the `MathSender`
component receives a `DO_MATH` command.
This handler overrides the corresponding pure virtual
function in the auto-generated base class.
Fill in the handler so that it looks like this:

```c++
void MathSender ::
  DO_MATH_cmdHandler(
      const FwOpcodeType opCode,
      const U32 cmdSeq,
      F32 val1,
      MathOp op,
      F32 val2
  )
{
  this->tlmWrite_VAL1(val1);
  this->tlmWrite_OP(op);
  this->tlmWrite_VAL2(val2);
  this->log_ACTIVITY_LO_COMMAND_RECV(val1, op, val2);
  this->mathOpOut_out(0, val1, op, val2);
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
```

The first two arguments to the handler function provide
the command opcode and the command sequence number
(a unique identifier generated by the command dispatcher).
The remaining arguments are supplied when the command is sent,
for example, from the F Prime ground data system (GDS).
The implementation code does the following:

1. Emit telemetry and events.

1. Invoke the `mathOpOut` port to request that `MathReceiver`
perform the operation.

1. Send a command response indicating success.
The command response goes out on the special port
`cmdResponseOut`.

In F Prime, every execution of a command handler must end by
sending a command response.
The proper behavior of other framework components (e.g., command
dispatcher, command sequencer) depends upon adherence to this rule.

**Check the build:**
Run `fprime-util build` again to make sure that everything still builds.

**Fill in the mathResultIn handler:**
You should see a stub handler for the `mathResultIn`
port that looks like this:

```c++
void MathSender ::
  mathResultIn_handler(
      const NATIVE_INT_TYPE portNum,
      F32 result
  )
{
  // TODO
}
```

The handler `mathResultIn_handler` is called when the `MathReceiver`
component code returns a result by invoking the `mathResultIn` port.
Again the handler overrides the corresponding pure virtual
function in the auto-generated base class.
Fill in the handler so that it looks like this:

```c++
void MathSender ::
  mathResultIn_handler(
      const NATIVE_INT_TYPE portNum,
      F32 result
  )
{
    this->tlmWrite_RESULT(result);
    this->log_ACTIVITY_HI_RESULT(result);
}
```

The implementation code emits the result on the `RESULT`
telemetry channel and as a `RESULT` event report.

**Check the build:**
Run `fprime-util build`.

<a name="math-sender_unit"></a>
### Write and Run Unit Tests

**Unit tests** are an important part of FSW development.
At the component level, unit tests typically invoke input ports, send commands,
and check for expected values on output ports (including telemetry and event
ports).

We will carry out the unit testing for the `MathSender` component
in three steps:

1. Set up the unit test environment

1. Write and run one unit test

1. Write and run additional unit tests

<a name="math-sender_unit_setup"></a>
#### Set Up the Unit Test Environment

**Create the stub Tester class:**
In the directory `Ref/MathSender`, run `mkdir -p test/ut`.
This will create the directory where
the unit tests will reside.

**Update Ref/MathSender/CMakeLists.txt:**
Go back to the directory `Ref/MathSender`.
Add the following lines to `CMakeLists.txt`:

```cmake
# Register the unit test build
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSender.fpp"
)
register_fprime_ut()
```

**Generate the unit test stub:**
We will now generate a stub implementation of the unit tests.
This stub contains all the boilerplate necessary to write and
run unit tests against the `MathSender` component.
In a later step, we will fill in the stub with tests.

1. If you have not yet run `fprime-util generate --ut`,
   then do so now. This step generates the CMake build cache for the unit
   tests.

1. Run the command `fprime-util impl --ut`.
   It should generate files `Tester.cpp`, `Tester.hpp`, and `TestMain.cpp`.

1. Move these files to the `test/ut` directory:

   ```bash
   mv Tester.* TestMain.cpp test/ut
   ```

**Update Ref/MathSender/CMakeLists.txt:**
Open `MathSender/CMakeLists.txt` and update the definition of
`UT_SOURCE_FILES` by adding your new test files:

```cmake
# Register the unit test build
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSender.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_ut()
```

**Run the build:**
Now we can check that the unit test build is working.
Run `fprime-util build --ut`.
Everything should build without errors.

**Inspect the generated code:**
The unit test build generates some code to support unit testing.
The code is located at `Ref/build-fprime-automatic-native-ut/Ref/MathSender`.
This directory contains two auto-generated classes:

1. `MathSenderGTestBase`: This is the direct base
class of `Tester`.
It provides a test interface implemented with Google Test
macros.

1. `MathSenderTesterBase`: This is the direct base
class of `MathSenderGTestBase`.
It provides basic features such as histories of port
invocations.
It is not specific to Google Test, so you can
use this class without Google Test if desired.

You can look at the header files for these generated classes
to see what operations they provide.
In the next sections we will provide some example uses
of these operations.

#### Write and Run One Test

Now we will write a unit test that exercises the
`DO_MATH` command.
We will do this in three phases:

1. In the `Tester` class, add a helper function for sending the command and
checking the responses.
That way multiple tests can reuse the same code.

1. In the `Tester` class, write a test function that
calls the helper to run a test.

1. In the `main` function, write a Google Test macro
that invokes the test function.

1. Run the test.

**Add a helper function:**
Go into the directory `Ref/MathSender/test/ut`.
In the file `Tester.hpp`, add the following lines
to the section entitled "Helper methods":

```c++
//! Test a DO_MATH command
void testDoMath(MathOp op);
```

In the file `Tester.cpp`, add the corresponding
function body:

```c++
void Tester ::
  testDoMath(MathOp op)
{

    // Pick values

    const F32 val1 = 2.0;
    const F32 val2 = 3.0;

    // Send the command

    // pick a command sequence number
    const U32 cmdSeq = 10;
    // send DO_MATH command
    this->sendCmd_DO_MATH(0, cmdSeq, val1, op, val2);
    // retrieve the message from the message queue and dispatch the command to the handler
    this->component.doDispatch();

    // Verify command receipt and response

    // verify command response was sent
    ASSERT_CMD_RESPONSE_SIZE(1);
    // verify the command response was correct as expected
    ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, cmdSeq, Fw::CmdResponse::OK);

    // Verify operation request on mathOpOut

    // verify that one output port was invoked overall
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // verify that the math operation port was invoked once
    ASSERT_from_mathOpOut_SIZE(1);
    // verify the arguments of the operation port
    ASSERT_from_mathOpOut(0, val1, op, val2);

    // Verify telemetry

    // verify that 3 channels were written
    ASSERT_TLM_SIZE(3);
    // verify that the desired telemetry values were sent once
    ASSERT_TLM_VAL1_SIZE(1);
    ASSERT_TLM_VAL2_SIZE(1);
    ASSERT_TLM_OP_SIZE(1);
    // verify that the correct telemetry values were sent
    ASSERT_TLM_VAL1(0, val1);
    ASSERT_TLM_VAL2(0, val2);
    ASSERT_TLM_OP(0, op);

    // Verify event reports

    // verify that one event was sent
    ASSERT_EVENTS_SIZE(1);
    // verify the expected event was sent once
    ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
    // verify the correct event arguments were sent
    ASSERT_EVENTS_COMMAND_RECV(0, val1, op, val2);

}
```

This function is parameterized over different
operations.
It is divided into five sections: sending the command,
checking the command response, checking the output on
`mathOpOut`, checking telemetry, and checking events.
The comments explain what is happening in each section.
For further information about the F Prime unit test
interface, see the F Prime User's Guide.

Notice that after sending the command to the component, we call
the function `doDispatch` on the component.
We do this in order to simulate the behavior of the active
component in a unit test environment.
In a flight configuration, the component has its own thread,
and the thread blocks on the `doDispatch` call until another
thread puts a message on the queue.
In a unit test context, there is only one thread, so the pattern
is to place work on the queue and then call `doDispatch` on
the same thread.

There are a couple of pitfalls to watch out for with this pattern:

1. If you put work on the queue and forget to call `doDispatch`,
the work won't get dispatched.
Likely this will cause a unit test failure.

1. If you call `doDispatch` without putting work on the queue,
the unit test will block until you kill the process (e.g.,
with control-C).

**Write a test function:**
Next we will write a test function that calls
`testDoMath` to test an `ADD` operation.
In `Tester.hpp`, add the following line in the
section entitled "Tests":

```c++
//! Test an ADD command
void testAddCommand();
```

In `Tester.cpp`, add the corresponding function
body:

```c++
void Tester ::
    testAddCommand()
{
    this->testDoMath(MathOp::ADD);
}
```

This function calls `testDoMath` to test an `ADD` command.

**Write a test macro:**
Add the following code to the file `main.cpp`,
before the definition of the `main` function:

```c++
TEST(Nominal, AddCommand) {
    Ref::Tester tester;
    tester.testAddCommand();
}
```

The `TEST` macro is an instruction to Google Test to run a test.
`Nominal` is the name of a test suite.
We put this test in the `Nominal` suite because it addresses
nominal (expected) behavior.
`AddCommand` is the name of the test.
Inside the body of the macro, the first line declares a new
object `tester` of type `Tester`.
We typically declare a new object for each unit test, so that
each test starts in a fresh state.
The second line invokes the function `testAddCommand`
that we wrote in the previous section.

**Run the test:**
Go back to directory `Ref/MathSender`.
Run the command `fprime-util check`.
The build system should compile and run the unit
tests.
You should see output indicating that the test ran
and passed.

As an exercise, try the following:

1. Change the behavior of the component
so that it does something incorrect.
For example, try adding one to a telemetry
value before emitting it.

1. Rerun the test and observe what happens.

#### Write and Run More Tests

**Add more command tests:**
Try to follow the pattern given in the previous
section to add three more tests, one each
for operations `SUB`, `MUL`, and `DIV`.
Most of the work should be done in the helper
that we already wrote.
Each new test requires just a short test function
and a short test macro.

Run the tests to make sure everything compiles and
the tests pass.

**Add a result test:**
Add a test for exercising the scenario in which the `MathReceiver`
component sends a result back to `MathSender`.

1. Add the following function signature in the "Tests" section of `Tester.hpp`:

   ```c++
   //! Test receipt of a result
   void testResult();
   ```

1. Add the corresponding function body in `Tester.cpp`:

   ```c++
   void Tester ::
     testResult()
   {
       // Generate an expected result
       const F32 result = 10.0;
       // reset all telemetry and port history
       this->clearHistory();
       // call result port with result
       this->invoke_to_mathResultIn(0, result);
       // retrieve the message from the message queue and dispatch the command to the handler
       this->component.doDispatch();
       // verify one telemetry value was written
       ASSERT_TLM_SIZE(1);
       // verify the desired telemetry channel was sent once
       ASSERT_TLM_RESULT_SIZE(1);
       // verify the values of the telemetry channel
       ASSERT_TLM_RESULT(0, result);
       // verify one event was sent
       ASSERT_EVENTS_SIZE(1);
       // verify the expected event was sent once
       ASSERT_EVENTS_RESULT_SIZE(1);
       // verify the expect value of the event
       ASSERT_EVENTS_RESULT(0, result);
   }
   ```

   This code is similar to the helper function in the previous section.
   The main difference is that it invokes a port directly
   (the `mathResultIn` port) instead of sending a command.

1. Add the following test macro to `main.cpp`:

   ```c++
   TEST(Nominal, Result) {
       Ref::Tester tester;
       tester.testResult();
   }
   ```

1. Run the tests.
Again you can try altering something in the component code
to see what effect it has on the test output.

<a name="math-sender_exercise"></a>
#### Exercise: Random Testing

F Prime provides a module called `STest`
that provides helper classes and functions for writing
unit tests.
As an exercise, use the interface provided by
`STest/STest/Pick.hpp` to pick random values to use in the
tests instead of using hard-coded values such as 2.0, 3.0,
and 10.

**Modifying the code:** You will need to do the following:

1. Add `#include "STest/Pick/Pick.hpp"` to `Tester.cpp`.

1. Add the following
   line to `Ref/MathSender/CMakeLists.txt`, before `register_fprime_ut`:

   ```cmake
   set(UT_MOD_DEPS STest)
   ```

   This line tells the build system to make the unit test build
   depend on the `STest` build module.

1. Add `#include "STest/Random/Random.hpp"` to `main.cpp`.

1. Add the following line to the `main` function of `main.cpp`,
   just before the return statement:

   ```c++
   STest::Random::seed();
   ```

   This line seeds the random number generator used by STest.

**Running the tests:**
Recompile and rerun the tests.
Now go to
`Ref/build-fprime-automatic-native-ut/Ref/MathSender` and inspect the
file `seed-history`.
This file is a log of random seed values.
Each line represents the seed used in the corresponding run.

**Fixing the random seed:**
Sometimes you may want to run a test with a particular seed value,
e.g., for replay debugging.
To do this, put the seed value into a file `seed` in the same
directory as `seed-history`.
If the file `seed` exists, then STest will use the seed it contains instead
of generating a new seed.

Try the following:

1. Copy the last value _S_ of `seed-history` into `seed`.

1. In `Ref/MathSender`, re-run the unit tests a few times.

1. Inspect `Ref/build-fprime-automatic-native-ut/Ref/MathSender/seed-history`.
You should see that the value _S_ was used in the runs you just did
(corresponding to the last few entries in `seed-history`).

### Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/MathSender`.

## The MathReceiver Component

Now we will build and test the `MathReceiver` component.
We will use the same five steps as for the
<a href="#math-sender">`MathSender` component</a>.

### Construct the FPP Model

**Create the MathReceiver directory:**
Create the directory `Ref/MathReceiver`.

**Create the FPP model file:**
In directory `Ref/MathReceiver`, create a file
`MathReceiver.fpp` with the following contents:

```fpp
!include ../MathReceiver/MathReceiver.fpp
```

This code defines a component `Ref.MathReceiver`.
The component is **queued**, which means it has a queue
but no thread.
Work occurs when the thread of another component invokes
the `schedIn` port of this component.

We have divided the specifiers of this component into six groups:

1. **General ports:** There are three ports:
an input port `mathOpIn` for receiving a math operation,
an output port `mathResultOut` for sending a math result, and
an input port `schedIn` for receiving invocations from the scheduler.
`mathOpIn` is asynchronous.
That means invocations of `mathOpIn` put messages on a queue.
`schedIn` is synchronous.
That means invocations of `schedIn` immediately call the
handler function to do work.

1. **Special ports:**
As before, there are special ports for commands, events, telemetry,
and time.
There are also special ports for getting and setting parameters.
We will explain the function of these ports below.

1. **Parameters:** There is one **parameter**.
A parameter is a constant that is configurable by command.
In this case there is one parameter `FACTOR`.
It has the default value 1.0 until its value is changed by command.
When doing math, the `MathReceiver` component performs the requested
operation and then multiplies by this factor.
For example, if the arguments of the `mathOpIn` port
are _v1_, `ADD`, and _v2_, and the factor is _f_,
then the result sent on `mathResultOut` is
_(v1 + v2) f_.

1. **Events:** There are three event reports:

   1. `FACTOR_UPDATED`: Emitted when the `FACTOR` parameter
      is updated by command.
      This event is **throttled** to a limit of three.
      That means that after the event is emitted three times
      it will not be emitted any more, until the throttling
      is cleared by command (see below).

   1. `OPERATION_PERFORMED`: Emitted when this component
      performs a math operation.

   1. `THROTTLE_CLEARED`: Emitted when the event throttling
      is cleared.

1. **Commands:** There is one command for clearing
the event throttle.

1. **Telemetry:**
There two telemetry channels: one for reporting
the last operation received and one for reporting
the factor parameter.

For the parameters, events, commands, and telemetry, we chose
to put in all the opcodes and identifiers explicitly.
These can also be left implicit, as in the `MathSender`
component example.
For more information, see
[_The FPP User's Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Components).

### Add the Model to the Project

Follow the steps given for the
<a href="#math-sender_add-model">`MathSender` component</a>.

### Build the Stub Implementation

Follow the same steps as for the
<a href="#math-sender_build-stub">`MathSender` component</a>.

### Complete the Implementation

**Fill in the mathOpIn handler:**
In `MathReceiver.cpp`, complete the implementation of
`mathOpIn_handler` so that it looks like this:

```cpp
void MathReceiver ::
  mathOpIn_handler(
      const NATIVE_INT_TYPE portNum,
      F32 val1,
      const MathOp& op,
      F32 val2
  )
{

    // Get the initial result
    F32 res = 0.0;
    switch (op.e) {
        case MathOp::ADD:
            res = val1 + val2;
            break;
        case MathOp::SUB:
            res = val1 - val2;
            break;
        case MathOp::MUL:
            res = val1 * val2;
            break;
        case MathOp::DIV:
            res = val1 / val2;
            break;
        default:
            FW_ASSERT(0, op.e);
            break;
    }

    // Get the factor value
    Fw::ParamValid valid;
    F32 factor = paramGet_FACTOR(valid);
    FW_ASSERT(
        valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
        valid.e
    );

    // Multiply result by factor
    res *= factor;

    // Emit telemetry and events
    this->log_ACTIVITY_HI_OPERATION_PERFORMED(op);
    this->tlmWrite_OPERATION(op);

    // Emit result
    this->mathResultOut_out(0, res);

}
```

This code does the following:

1. Compute an initial result based on the input values and
the requested operation.

1. Get the value of the factor parameter.
Check that the value is a valid value from the parameter
database or a default parameter value.

1. Multiply the initial result by the factor to generate
the final result.

1. Emit telemetry and events.

1. Emit the result.

Note that in step 1, `op` is an enum (a C++ class type), and `op.e`
is the corresponding numeric value (an integer type).
Note also that in the `default` case we deliberately fail
an assertion.
This is a standard pattern for exhaustive case checking.
We should never hit the assertion.
If we do, then a bug has occurred: we missed a case.

**Fill in the schedIn handler:**
In `MathReceiver.cpp`, complete the implementation of
`schedIn_handler` so that it looks like this:

```c++
void MathReceiver ::
  schedIn_handler(
      const NATIVE_INT_TYPE portNum,
      NATIVE_UINT_TYPE context
  )
{
    U32 numMsgs = this->m_queue.getNumMsgs();
    for (U32 i = 0; i < numMsgs; ++i) {
        (void) this->doDispatch();
    }
}
```

This code dispatches all the messages on the queue.
Note that for a queued component, we have to do this
dispatch explicitly in the `schedIn` handler.
For an active component, the framework auto-generates
the dispatch code.

**Fill in the CLEAR_EVENT_THROTTLE command handler:**
In `MathReceiver.cpp`, complete the implementation of
`CLEAR_EVENT_THROTTLE_cmdHandler` so that it looks like this:

```c++
void MathReceiver ::
  CLEAR_EVENT_THROTTLE_cmdHandler(
      const FwOpcodeType opCode,
      const U32 cmdSeq
  )
{
    // clear throttle
    this->log_ACTIVITY_HI_FACTOR_UPDATED_ThrottleClear();
    // send event that throttle is cleared
    this->log_ACTIVITY_HI_THROTTLE_CLEARED();
    // reply with completion status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
```

The call to `log_ACTIVITY_HI_FACTOR_UPDATED_ThrottleClear` clears
the throttling of the `FACTOR_UPDATED` event.
The next two lines send a notification event and send
a command response.

**Add a parameterUpdated function:**
Add the following function to `MathReceiver.cpp`.
You will need to add the corresponding function header
to `MathReceiver.hpp`.

```c++
void MathReceiver ::
   parameterUpdated(FwPrmIdType id)
{
    switch (id) {
        case PARAMID_FACTOR: {
            Fw::ParamValid valid;
            F32 val = this->paramGet_FACTOR(valid);
            FW_ASSERT(
                valid.e == Fw::ParamValid::VALID || valid.e == Fw::ParamValid::DEFAULT,
                valid.e
            );
            this->log_ACTIVITY_HI_FACTOR_UPDATED(val);
            break;
        }
        default:
            FW_ASSERT(0, id);
            break;
    }
}
```

This code implements an optional function that, if present,
is called when a parameter is updated by command.
The parameter identifier is passed in as the `id` argument
of the function.
Here we do the following:

1. If the parameter identifier is `PARAMID_FACTOR` (the parameter
identifier corresponding to the `FACTOR` parameter),
then get the parameter value and emit an event report.

1. Otherwise fail an assertion.
This code should never run, because there are no other
parameters.

### Write and Run Unit Tests

#### Set up the Unit Test Environment

1. Follow the steps given for the
<a href="#math-sender_unit_setup">`MathSender` component</a>.

1. Follow the steps given under **Modifying the code**
for the
<a href="#math-sender_exercise">random testing exercise</a>,
so that you can use STest to pick random values.

#### Add Helper Code

**Add a ThrottleState enum class:**
Add the following code to the beginning of the
`Tester` class in `Tester.hpp`:

```c++
private:

  // ----------------------------------------------------------------------
  // Types
  // ----------------------------------------------------------------------

  enum class ThrottleState {
    THROTTLED,
    NOT_THROTTLED
  };
```

This code defines a C++ enum class for recording whether an
event is throttled.

**Add helper functions:**
Add each of the functions described below to the
"Helper methods" section of `Tester.cpp`.
For each function, you must add
the corresponding function prototype to `Tester.hpp`.
After adding each function, compile the unit tests
to make sure that everything still compiles.
Fix any errors that occur.

Add a `pickF32Value` function.

```c++
F32 Tester ::
  pickF32Value()
{
  const F32 m = 10e6;
  return m * (1.0 - 2 * STest::Pick::inUnitInterval());
}
```

This function picks a random `F32` value in the range
_[ -10^6, 10^6 ]_.

Add a `setFactor` function.

```c++
void Tester ::
  setFactor(
      F32 factor,
      ThrottleState throttleState
  )
{
    // clear history
    this->clearHistory();
    // set the parameter
    this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
    const U32 instance = STest::Pick::any();
    const U32 cmdSeq = STest::Pick::any();
    this->paramSend_FACTOR(instance, cmdSeq);
    if (throttleState == ThrottleState::NOT_THROTTLED) {
        // verify the parameter update notification event was sent
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_FACTOR_UPDATED_SIZE(1);
        ASSERT_EVENTS_FACTOR_UPDATED(0, factor);
    }
    else {
        ASSERT_EVENTS_SIZE(0);
    }
}
```

This function does the following:

1. Clear the test history.

1. Send a command to the component to set the `FACTOR` parameter
to the value `factor`.

1. If `throttleState` is `NOT_THROTTLED`, then check
that the event was emitted.
Otherwise check that the event was throttled (not emitted).

Add a function `computeResult` to `Tester.cpp`.

```c++
F32 Tester ::
  computeResult(
      F32 val1,
      MathOp op,
      F32 val2,
      F32 factor
  )
{
    F32 result = 0;
    switch (op.e) {
        case MathOp::ADD:
            result = val1 + val2;
            break;
        case MathOp::SUB:
            result = val1 - val2;
            break;
        case MathOp::MUL:
            result = val1 * val2;
            break;
        case MathOp::DIV:
            result = val1 / val2;
            break;
        default:
            FW_ASSERT(0, op.e);
            break;
    }
    result *= factor;
    return result;
}
```

This function carries out the math computation of the
math component.
By running this function and comparing, we can
check the output of the component.

Add a `doMathOp` function to `Tester.cpp`.

```c++
void Tester ::
  doMathOp(
      MathOp op,
      F32 factor
  )
{

    // pick values
    const F32 val1 = pickF32Value();
    const F32 val2 = pickF32Value();

    // clear history
    this->clearHistory();

    // invoke operation port with add operation
    this->invoke_to_mathOpIn(0, val1, op, val2);
    // invoke scheduler port to dispatch message
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);

    // verify the result of the operation was returned

    // check that there was one port invocation
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // check that the port we expected was invoked
    ASSERT_from_mathResultOut_SIZE(1);
    // check that the component performed the operation correctly
    const F32 result = computeResult(val1, op, val2, factor);
    ASSERT_from_mathResultOut(0, result);

    // verify events

    // check that there was one event
    ASSERT_EVENTS_SIZE(1);
    // check that it was the op event
    ASSERT_EVENTS_OPERATION_PERFORMED_SIZE(1);
    // check that the event has the correct argument
    ASSERT_EVENTS_OPERATION_PERFORMED(0, op);

    // verify telemetry

    // check that one channel was written
    ASSERT_TLM_SIZE(1);
    // check that it was the op channel
    ASSERT_TLM_OPERATION_SIZE(1);
    // check for the correct value of the channel
    ASSERT_TLM_OPERATION(0, op);

}
```

This function is similar to the `doMath` helper function that
we wrote for the `MathSender` component.
Notice that the method for invoking a port is different.
Since the component is queued, we don't call `doDispatch`
directly.
Instead we invoke `schedIn`.

#### Write and Run Tests

For each of the tests described below, you must add the
corresponding function prototype to `Tester.hpp`
and the corresponding test macro to `main.cpp`.
If you can't remember how to do it, look back at the
`MathSender` examples.
After writing each test, run all the tests and make sure
that they pass.

**Write an ADD test:**
Add the following function to the "Tests" section of `Tester.cpp`:

```c++
void Tester ::
  testAdd()
{
    // Set the factor parameter by command
    const F32 factor = pickF32Value();
    this->setFactor(factor, ThrottleState::NOT_THROTTLED);
    // Do the add operation
    this->doMathOp(MathOp::ADD, factor);
}
```

This function calls the `setFactor` helper function
to set the factor parameter.
Then it calls the `doMathOp` function to
do a math operation.

**Write a SUB test:**
Add the following function to the "Tests" section of `Tester.cpp`:

```c++
void Tester ::
  testSub()
{
    // Set the factor parameter by loading parameters
    const F32 factor = pickF32Value();
    this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
    this->component.loadParameters();
    // Do the operation
    this->doMathOp(MathOp::SUB, factor);
}
```

This test is similar to `testAdd`, but it shows
another way to set a parameter.
`testAdd` showed how to set a parameter by command.
You can also set a parameter by initialization, as follows:

1. Call the `paramSet` function as shown.
This function sets the parameter value in
the part of the test harness that mimics the behavior of the
parameter database component.

1. Call the `loadParameters` function as shown.
In flight, the function `loadParameters` is typically called at the
start of FSW to load the parameters from the database;
here it loads the parameters from the test harness.
There is no command to update a parameter, so `parameterUpdated`
is not called, and no event is emitted.

As before, after setting the parameter we call `doMathOp`
to do the operation.

**Write a MUL test:**
This test is the same as the ADD test, except that it
uses MUL instead of add.

**Write a DIV test:**
This test is the same as the SUB test, except that it
uses DIV instead of SUB.

**Write a throttle test:**
Add the following constant definition to the top of the `Tester.cpp` file:

```C++
#define CMD_SEQ 42
```

Then add the following function to the "Tests" section of `Tester.cpp`:

```c++
void Tester ::
  testThrottle()
{

    // send the number of commands required to throttle the event
    // Use the autocoded value so the unit test passes if the
    // throttle value is changed
    const F32 factor = pickF32Value();
    for (
        U16 cycle = 0;
        cycle < MathReceiverComponentBase::EVENTID_FACTOR_UPDATED_THROTTLE;
        cycle++
    ) {
        this->setFactor(factor, ThrottleState::NOT_THROTTLED);
    }

    // Event should now be throttled
    this->setFactor(factor, ThrottleState::THROTTLED);

    // send the command to clear the throttle
    this->sendCmd_CLEAR_EVENT_THROTTLE(INSTANCE, CMD_SEQ);
    // invoke scheduler port to dispatch message
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);
    // verify clear event was sent
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_THROTTLE_CLEARED_SIZE(1);

    // Throttling should be cleared
    this->setFactor(factor, ThrottleState::NOT_THROTTLED);

}
```

This test first loops over the throttle count, which is stored
for us in the constant `EVENTID_FACTOR_UPDATED_THROTTLE`
of the `MathReceiver` component base class.
On each iteration, it calls `setFactor`.
At the end of this loop, the `FACTOR_UPDATED` event should be
throttled.

Next the test calls `setFactor` with a second argument of
`ThrottleState::THROTTLED`.
This code checks that the event is throttled.

Next the test sends the command `CLEAR_EVENT_THROTTLE`,
checks for the corresponding notification event,
and checks that the throttling is cleared.

### Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/MathReceiver`.

### Exercises

#### Adding Telemetry

Add a telemetry channel that records the number of math
operations performed.

1. Add the channel to the FPP model.

1. In the component implementation class, add a member
variable `numMathOps` of type `U32`.
Initialize the variable to zero in the class constructor.

1. Revise the `mathOpIn` handler so that it increments
`numMathOps` and emits the updated value as telemetry.

1. Revise the unit tests to cover the new behavior.

#### Error Handling

Think about what will happen if the floating-point
math operation performed by `MathReceiver` causes an error.
For example, suppose that `mathOpIn` is invoked with `op = DIV`
and `val2 = 0.0`.
What will happen?
As currently designed and implemented, the `MathReceiver`
component will perform the requested operation.
On some systems the result will be `INF` (floating-point infinity).
In this case, the result will be sent back to `MathSender`
and reported in the usual way.
On other systems, the hardware could issue a floating-point exception.

Suppose you wanted to handle the case of division by zero
explicitly.
How would you change the design?
Here are some questions to think about:

1. How would you check for division by zero?
Note that `val2 = 0.0` is not the only case in which a division
by zero error can occur.
It can also occur for very small values of `val2`.

1. Should the error be caught in `MathSender` or `MathReceiver`?

1. Suppose the design says that `MathSender` catches the error,
and so never sends requests to `MathReceiver` to divide by zero.
What if anything should `MathReceiver` do if it receives
a divide by zero request?
Carry out the operation normally?
Emit a warning?
Fail a FSW assertion?

1. If the error is caught by `MathReceiver`, does the
interface between the components have to change?
If so, how?
What should `MathSender` do if `MathReceiver`
reports an error instead of a valid result?

Revise the MathSender and MathReceiver components to implement your
ideas.
Add unit tests covering the new behavior.

## Updating the Ref Deployment

The next step in the tutorial is to define instances of the
`MathSender` and `MathReceiver` components and add them
to the `Ref` topology.

### Defining the Component Instances

Go to the directory `Ref/Top` and open the file `instances.fpp`.
This file defines the instances used in the topology for the
`Ref` application.
Update this file as described below.

**Define the mathSender instance:**
At the end of the section entitled "Active component instances,"
add the following lines:

```fpp
instance mathSender: Ref.MathSender base id 0xE00 \
  queue size Default.QUEUE_SIZE \
  stack size Default.STACK_SIZE \
  priority 100
```

This code defines an instance `mathSender` of component
`MathSender`.
It has **base identifier** 0xE00.
FPP adds the base identifier to each the relative identifier
defined in the component to compute the corresponding
identifier for the instance.
For example, component `MathSender` has a telemetry channel
`MathOp` with identifier 1, so instance `mathSender`
has a command `MathOp` with identifier 0xE01.

The following lines define the queue size, stack size,
and thread priority for the active component.
Here we give `mathSender` the default queue size
and stack size and a priority of 100.

**Define the mathReceiver instance:**
At the end of the section "Queued component instances,"
add the following lines:

```fpp
instance mathReceiver: Ref.MathReceiver base id 0x2700 \
  queue size Default.QUEUE_SIZE
```

This code defines an instance `mathReceiver` of
component `MathReceiver`.
It has base identifier 0x2700 and the default queue size.

**More information:**
For more information on defining component instances,
see
[_The FPP User's Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Component-Instances).

### Updating the Topology

Go to the directory `Ref/Top` and open the file `topology.fpp`.
This file defines the topology for the `Ref` application.
Update this file as described below.

**Add the new instances:**
You should see a list of instances, each of which begins
with the keyword `instance`.
After the line `instance linuxTime`, add the following
lines:

```fpp
instance mathSender
instance mathReceiver
```

These lines add the `mathSender` and `mathReceiver`
instances to the topology.

**Check for unconnected ports:**
Run the following commands in the `Ref/Top` directory:

```bash
fprime-util fpp-check -u unconnected.txt
cat unconnected.txt
```

You should see a list of ports
that are unconnected in the `Ref` topology.
Those ports will include the ports for the new instances
`mathSender` and `mathReceiver`.

**Connect mathReceiver to rate group 1:**
Find the line that starts `connections RateGroups`.
This is the beginning of the definition of the `RateGroups`
connection graph.
After the last entry for the `rateGroup1Comp` (rate group 1) add
the following line:

```fpp
rateGroup1Comp.RateGroupMemberOut -> mathReceiver.schedIn
```

This line adds the connection that drives the `schedIn`
port of the `mathReceiver` component instance.

**Re-run the check for unconnected ports:**
You should see that `mathReceiver.schedIn` is now connected
(it no longer appears in the list).

**Add the Math connections:**
Find the Uplink connections that begin with the line
`connections Uplink`.
After the block of that definition, add the following
lines:

```fpp
connections Math {
  mathSender.mathOpOut -> mathReceiver.mathOpIn
  mathReceiver.mathResultOut -> mathSender.mathResultIn
}
```

These lines add the connections between the `mathSender`
and `mathReceiver` instances.

**Re-run the check for unconnected ports:**
When this capability exists, you will be able to see
that the `mathSender` and `mathReceiver` ports are connected.

**More information:**
For more information on defining topologies,
see
[_The FPP User's Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Topologies).

### Building the Ref Deployment

Go to the `Ref` directory.
Run `fprime-util build --jobs 4`.
The updated deployment should build without errors.
The generated files are located at
`Ref/build-fprime-automatic-native/Ref/Top`.

### Visualizing the Ref Topology

Now we will see how to create a visualization (graphical rendering)
of the Ref topology.

**Generate the layout:**
For this step, we will use the F Prime Layout (FPL) tool.
If FPL is not installed on your system, then install it how:
clone [this repository](https://github.com/fprime-community/fprime-layout)
and follow the instructions.

In directory `Ref/Top`, run the following commands in an sh-compatible
shell such as bash.
If you are using a different shell, you can run `sh`
to enter the `sh` shell, run these commands, and enter
`exit` when done.
Or you can stay in your preferred shell and adjust these commands
appropriately.

```bash
cp ../build-fprime-automatic-native/Ref/Top/RefTopologyAppAi.xml .
mkdir visual
cd visual
fpl-extract-xml < ../RefTopologyAppAi.xml
mkdir Ref
for file in `ls *.xml`
do
echo "laying out $file"
base=`basename $file .xml`
fpl-convert-xml $file | fpl-layout > Ref/$base.json
done
```

This step extracts the connection graphs from the topology XML and
converts each one to a JSON layout file.

**Render the layout:**
For this step, we will use the F Prime Visualizer (FPV) tool.
If FPV is not installed on your system, then install it now:
clone [this repository](https://github.com/fprime-community/fprime-visual)
and follow the instructions.

In directory `Ref/Top`, run the following commands in an sh-compatible
shell.
Replace `[path to fpv root]` with the path to the
root of the FPV repo on your system.

```bash
echo DATA_FOLDER=Ref/ > .fpv-env
nodemon [path to fpv root]/server/index.js ./.fpv-env
```

You should see the FPV server application start up on the
console.

Now open a browser and navigate to `http://localhost:3000`.
You should see a Topology menu at the top of the window
and a rendering of the Command topology below.
Select Math from the topology menu.
You should see a rendering of the Math topology.
It should look similar to the
<a href="#math-top">topology diagram shown above</a>.

You can use the menu to view other topology graphs.
When you are done, close the browser window and
type control-C in the console to shut down the FPV server.

### Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/Top`.
To build this implementation, copy the files
`instances.fpp` and `topology.fpp` from
that directory to `Ref/Top`.

## Running the Ref Deployment

Now we will use the F Prime Ground Data System (GDS) to run the Ref deployment.
Go to the `Ref` directory and run `fprime-gds`.
You should see some activity on the console.
The system is starting the Ref deployment executable, starting the GDS,
and connecting them over the local network on your machine.
After several seconds, a browser window should appear.

### Sending a Command

At the top of the window are several buttons, each of which corresponds to
a GDS view.
Select the Commanding button (this is the view that is selected
when you first start the GDS).
In the Mnemonic menu, start typing `mathSender.DO_MATH` in the text box.
As you type, the GDS will filter the menu selections.
When only one choice remains, stop typing and press return.
You should see three boxes appear:

1. A text box for entering `val1`.

1. A menu for entering `op`.

1. A text box for entering `val2`.

Fill in the arguments corresponding to the operation `1 + 2`.
You can use the tab key to move between the boxes.
When you have done this, click the Send Command button.
You should see a table entry at the bottom of the window
indicating that the command was sent.

### Checking Events

Now click on the Events button at the top of the window.
The view changes to the Events tab.
You should see events indicating that the command you sent was
dispatched, received, and completed.
You should also see events indicating that `mathReceiver`
performed an `ADD` operation and `mathSender`
received a result of 3.0.

### Checking Telemetry

Click on the Channels button at the top of the window.
You should see a table of telemetry channels.
Each row corresponds to the latest value of a telemetry
channel received by the GDS.
You should see the channels corresponding to the input
values, the operation, and the result.

### Setting Parameters

Go back to the Commanding tab.
Select the command `mathReceiver.FACTOR_PRM_SET`.
This is an auto-generated command for setting the
parameter `FACTOR`.
Type the value 2.0 in the `val` box and click Send Command.
Check the events to see that the command was dispatched
and executed.
You should also see the events sent by the code
that you implemented.

In the Commanding tab, issue the command `1 + 2` again.
Check the Events tab.
Because the factor is now 2.0, you should see a result
value of 6.0.

### Saving Parameters

When you set a parameter by command, the new parameter
value resides in the component that receives the command.
At this point, if you stop and restart FSW, the parameter
will return to its original value (the value before you
sent the command).

At some point you may wish to update parameters more permanently.
You can do this by saving them to non-volatile storage.
For the Ref application, "non-volatile storage" means the
file system on your machine.

To save the parameter `mathReceiver.FACTOR` to non-volatile storage,
do the following:

1. Send the command `mathReceiver.FACTOR_PRM_SAVE`.
This command saves the parameter value to the **parameter database**,
which is a standard F Prime component for storing system parameters.

1. Send the command `prmDb.PRM_SAVE_FILE`.
This command saves the parameter values in the parameter database
to non-volatile storage.

Note that saving parameters is a two-step process.
The first step copies a single parameter from a component
to the database.
The second step saves all parameters in the database
to the disk.
If you do only the first step, the parameter will not be
saved to the disk.

### GDS Logs

As it runs, the GDS writes a log into a subdirectory of `Ref/logs`.
The subdirectory is stamped with the current date.
Go into the directory for the run you just performed.
(If the GDS is still running, you will have to do this in a
different shell.)
You should see the following logs, among others:

* `Ref.log`: FSW console output.

* `command.log`: Commands sent.

* `event.log`: Event reports received.

* `channel.log`: Telemetry points received.

You can also view these logs via the GDS browser interface.
Click the Logs tab to go the Logs view.
Select the log you wish to inspect from the drop-down menu.
By default, there is no log selected.

## Conclusion

The Math Component tutorial has shown us how to create simple types, ports and
components for our application using the FPP modeling language. We have learned
how to use `fprime-util` to generate implementation stubs, the build cache, and
unit tests. We learned how to define our topology and use tools provided by
F´ to check and visualize the topology. Lastly, we learned how to use the
ground system to interact with our deployment.

The user is now directed back to the [Tutorials](../README.md) for further
reading or to the [Cross-Compilation Tutorial](../CrossCompilation/Tutorial.md)
for instructions on how to cross-compile the Ref application completed in this
tutorial for the Raspberry Pi.
