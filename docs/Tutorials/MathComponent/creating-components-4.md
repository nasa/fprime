# Creating Components Part 4: Implementing MathReceiver Behavior

## In this Section

In this section you will complete the implementation of the `MathReciever` by filling in the `MathReceiver.cpp`. 


Fill in the mathOpIn handler: In MathReceiver.cpp, complete the implementation of mathOpIn_handler so that it looks like this:

```cpp
// In: MathReceiver.cpp
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
    }//end switch 

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

}//end MathOpIn_handler 
```

**Explanation:**
This code does the following:

1. Compute an initial result based on the input values and the requested operation.

2. Get the value of the factor parameter. Check that the value is a valid value from the parameter database or a default parameter value.

3. Multiply the initial result by the factor to generate the final result.

4. Emit telemetry and events.

5. Emit the result.

Note that in step 1, `op` is an enum (a C++ class type), and `op.e` is the corresponding numeric value (an integer type). Note also that in the `default` case we deliberately fail an assertion. This is a standard pattern for exhaustive case checking. We should never hit the assertion. If we do, then a bug has occurred: we missed a case.


Fill in the schedIn handler in `MathReceiver.cpp`, complete the implementation of `schedIn_handler` so that it looks like this:

```cpp
// In: MathReceiver.cpp
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
**Explanation:** 
This code dispatches all the messages on the queue. Note that for a queued component, we have to do this dispatch explicitly in the schedIn handler. For an active component, the framework auto-generates the dispatch code.

Fill in the `CLEAR_EVENT_THROTTLE` command handler: In `MathReceiver.cpp`, complete the implementation of `CLEAR_EVENT_THROTTLE_cmdHandler` so that it looks like this:

```cpp
// In: MathReceiver.cpp
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
**Explanation:**
The call to `log_ACTIVITY_HI_FACTOR_UPDATED_ThrottleClear` clears the throttling of the `FACTOR_UPDATED` event. The next two lines send a notification event and send a command response.

Add the following function to `MathReceiver.cpp`. You will need to add the corresponding function header to `MathReceiver.hpp`. Note: this function is completely new, there is no prexisting stub for this function.

```cpp
// In: MathReceiver.cpp
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

```cpp
// In: MathReceiver.hpp
// As a Private under: Handler implementations for user-defined typed input ports
void parameterUpdated(FwPrmIdType id);
```

**Explanation**
This code implements an optional function that, if present, is called when a parameter is updated by command. The parameter identifier is passed in as the id argument of the function. Here we do the following:

    If the parameter identifier is PARAMID_FACTOR (the parameter identifier corresponding to the FACTOR parameter), then get the parameter value and emit an event report.

    Otherwise fail an assertion. This code should never run, because there are no other parameters.

## Summary

In this section you implemented the component behavior of MathReceiver. 

**Next:** [Developing Deployments](./developing-deployments.md)