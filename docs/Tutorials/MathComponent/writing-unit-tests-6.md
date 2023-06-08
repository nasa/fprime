# Writing Unit Tests Part 6: Writing Helper Functions
## In this Section 

In this section of the tutorial, you will write helper
functions to tests various function of `MathReceiver`.

**Add a ThrottleState enum class:**
Add the following code to the beginning of the
`Tester` class in `Tester.hpp`:

```c++
// In: Tester.hpp
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
// In: Tester.cpp
F32 Tester ::
  pickF32Value()
{
  const F32 m = 10e6;
  return m * (1.0 - 2 * STest::Pick::inUnitInterval());
}
```
> Remember to add a function signature in `Tester.hpp`. 

This function picks a random `F32` value in the range
_[ -10^6, 10^6 ]_.


At this point, it is a good to check the build. Use 
the following to check the build:

```shell
# In: MathReceiver
fprime-util build --ut -j4
```

Add a `setFactor` function.

```c++
// In Tester.cpp  
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
> Make sure that set factor is below where you defined `ThrottleSate` and remember to add a function signature in `Tester.hpp`.

This function does the following:

1. Clear the test history.

1. Send a command to the component to set the `FACTOR` parameter
to the value `factor`.

1. If `throttleState` is `NOT_THROTTLED`, then check
that the event was emitted.
Otherwise check that the event was throttled (not emitted).

Build to make sure everything is working. 

Add a function `computeResult` to `Tester.cpp`.

```c++
// In: Tester.cpp
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
> Don't forget to add a function signature in `Tester.hpp`.

This function carries out the math computation of the
math component.
By running this function and comparing, we can
check the output of the component.

Build to make sure everything is working. 

Add a `doMathOp` function to `Tester.cpp`.

```c++
// In: Tester.cpp
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
    // if you're dviding by zero, there may be two events ;) 
    ASSERT_EVENTS_SIZE(1);
    // check that it was the op event
    ASSERT_EVENTS_OPERATION_PERFORMED_SIZE(1);
    // check that the event has the correct argument
    ASSERT_EVENTS_OPERATION_PERFORMED(0, op);

    // verify telemetry

    // check that one channel was written
    ASSERT_TLM_SIZE(2);
    // check that it was the op channel
    ASSERT_TLM_OPERATION_SIZE(1);
    // check for the correct value of the channel
    ASSERT_TLM_OPERATION(0, op);

}
```
> Don't forget to add a function signature in `Tester.hpp`.

This function is similar to the `doMath` helper function that
we wrote for the `MathSender` component.
Notice that the method for invoking a port is different.
Since the component is queued, we don't call `doDispatch`
directly.
Instead we invoke `schedIn`.

Build before moving onto the next section.


**Next:** [Writing Unit Tests 7](./writing-unit-tests-7.md)