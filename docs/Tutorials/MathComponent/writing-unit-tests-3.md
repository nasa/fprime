# Writing Unit Tests Part 3: Testing the Results

## In this Section 
In this section of the tutorial, you will add another test into `MathSender/UnitTests` and run the new test.
**Add a result test:**
Add a test for exercising the scenario in which the `MathReceiver`
component sends a result back to `MathSender`.

Add the following function signature in the "Tests" section of `Tester.hpp`:

```c++
// In: MathSender/UnitTests/Tester.hpp
//! Test receipt of a result
void testResult();
```

Add the corresponding function body in `Tester.cpp`:

```c++
// In: Tester.cpp
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
**Explanation:**
This code is similar to the helper function in the previous section.
The main difference is that it invokes a port directly
(the `mathResultIn` port) instead of sending a command.

Add the following test macro to `main.cpp`:

```c++
TEST(Nominal, Result) {
    Ref::Tester tester; @TODO
    tester.testResult();
}
```

Run the tests:

```shell 
# In: MathSender
fprime-util check 
``` 

Again you can try altering something in the component code
to see what effect it has on the test output.

## Summary 

In this section, you created another helper function used to look test the received results as seen by the `MathSender`. You ran the test that you should wrote to ensure that it worked. 

**Next:** [Writing Unit Tests 4](./writing-unit-tests-4.md)