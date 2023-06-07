# Error Handling 2: One Solution


Below is a basic and incomplete solution to the divide 
by zero problem presented in the previous section.

The solution works as follows: Use an if statement 
to catch the case that `val2` (the denominator) is zero. 
In the case that `val2` is zero, do nothing with the opperands
and report the error through an event. 

Use an if statement in `MathReceiver.cpp` to catch
when the denominator is zero: 

```cpp
// In: MathReceiver.cpp
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
            //step 2 
            if ( val2 == 0 ){

              break; 
            }
            res = val1 / val2;
            break;
        default:
            FW_ASSERT(0, op.e);
            break;
    }
```
> Technically speaking, this solution will prevent the error,
but it would be good to output some error message before
throwing away the opperands and returning the default `res`.

Create an event to notify that a divide by zero command 
was received by the `MathReceiver`:

```fpp
# In: MathRecevier.fpp
@ Commanded to divide by zero 
event DIVIDE_BY_ZERO   \
    severity activity high \
    id 3 \
    format "ERROR: Received zero as denominator. Opperands dropped."
```
> Write your own error message between the quotes after `format`! 

Add your even into the case where `MathOp::DIV` and `val2` is 0:

```cpp
case MathOp::DIV:
    //step 2 
    if ( val2 == 0 ){
        this->log_ACTIVITY_HI_DIVIDE_BY_ZERO(); 
        break; 
    }
```

## Summary 
You just created a way to not only handle the case where 
`MathReceiver` is asked to divide by 0, you also created 
an event to report that an error has occured. As a challenge, 
try to handle more of the cases and problems discussed in
[Error handling 1](./error-handling-1.md)

