# Error Handling 1: Critical Thinking 

## Background 
On a flight mission, even a short timeout, let alone a system crash, can be mission critical. It is imparitive that programmer account for as many possible error or faults as possible so avoidable errors are prevented. 


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

2. Should the error be caught in `MathSender` or `MathReceiver`?

3. Suppose the design says that `MathSender` catches the error,
and so never sends requests to `MathReceiver` to divide by zero.
What if anything should `MathReceiver` do if it receives
a divide by zero request?
Carry out the operation normally?
Emit a warning?
Fail a FSW assertion?

4. If the error is caught by `MathReceiver`, does the
interface between the components have to change?
If so, how?
What should `MathSender` do if `MathReceiver`
reports an error instead of a valid result?

Try to revise the MathSender and MathReceiver components to implement your
ideas.
Challenge yourself to add unit tests covering the new behavior.

The next section gives one idea of how to do some error handling for the divide by zero case. Before looking at it, try to solve the problem on your own and compare against the method shown in this tutorial. 


**Next:** [Error Handling 2](./error-handling-2.md)