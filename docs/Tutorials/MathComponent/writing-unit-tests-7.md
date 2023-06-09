# Writing Unit Tests Part 7: Writing the Tests

## Summary 
 In this section of the tutorial, you will write 
 tests that make use of the helper functions you
 wrote in the last section of the tutorial.


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
// In: Tester.cpp
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
> Don't forget to add a function signature in `Tester.hpp`.

This function calls the `setFactor` helper function
to set the factor parameter.
Then it calls the `doMathOp` function to
do a math operation.

**Write a SUB test:**
Add the following function to the "Tests" section of `Tester.cpp`:

```c++
// In: Tester.cpp
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

As before, after setting the parameter you call `doMathOp`
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
// In: Tester.cpp
#define CMD_SEQ 42
```

Then add the following function to the "Tests" section of `Tester.cpp`:

```c++
// In: Tester.cpp
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
    this->sendCmd_CLEAR_EVENT_THROTTLE(TEST_INSTANCE_ID, CMD_SEQ);
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

**Explanation:**
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

Add your tests to `TestMain.cpp` so that the tests run when
`fprime-util check' is called. 

Here is how to include `testAdd` to `TestMain.cpp`. Follow this patttern to 
inlcude any other unit tests you wrote: 

```cpp
// In: TestMain.cpp
TEST(Nominal, AddCommand) {
    MathModule::Tester tester;
    tester.testAdd();
}
```

See if your tests are working and trouble shoot any errors: 

```shell
# In: MathReceiver
fprime-util check 
```


