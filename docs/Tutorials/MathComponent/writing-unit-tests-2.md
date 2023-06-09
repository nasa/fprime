# Writing Unit Tests Part 2: Completing the Stub & Running the Test

## In this Section 

In this section of the tutorial, you will fill in the stub implementation you created in the last section and run the unit test. 

Write a generic helper function so you can reuse code while writing unit tests. In Tester.cpp add the following:

Start by writing a function signature in `Tester.hpp`:

```cpp 
// In: Tester.hpp
void testDoMath(MathOp op); 
```

Fill out the corrosponding function body in `Tester.cpp`:

```cpp
// In: Tester.cpp

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

**Explanation:** 
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


You will now create a function to test the `ADD` command. Add a function signature to Tester.hpp:

```cpp
// In: Tester.hpp
void TestAddCommand(); 
``` 

Write the corrosponding tester function using the helper funtion you just wrote:

```cpp 
// In: Tester.cpp 
void Tester ::
    testAddCommand()
  {
      this->testDoMath(MathOp::ADD);
  }
```

Write a Google test macro in TestMain.cpp and make sure the test macro goes before main:

```cpp 
TEST(Nominal, AddCommand) {
    MathModule::Tester tester;
    tester.testAddCommand();
}
```
**Explanation:**
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



Run the test you have written. Make sure to execute the following in ```MathSender```. 

```shell 
# In: MathSender
fprime-util check 
``` 

As an exercise, try the following:

1. Change the behavior of the component
so that it does something incorrect.
For example, try adding one to a telemetry
value before emitting it.

1. Rerun the test and observe what happens.



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

## Summary 

In this section you filled out your unit test implemenation stub and ran your unit test. 

**Next:** [Writing Unit Tests 3](./writing-unit-tests-3.md)
