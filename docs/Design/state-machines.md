# State Machines

## 1. Introduction

A **state machine** is a software subsystem whose behavior is
described by states and transitions, together with related concepts
such as signals, actions, and guards.
State machines are important in flight software and embedded programming.
In this section, we document the features of F Prime that support
programming with state machines.

## 2. External and Internal State Machines

F Prime supports two kinds of state machines:
**external state machines** and **internal state machines**.
An external state machine is specified by an external tool,
typically the
[State Autocoding for Real-Time Systems (STARS) 
tool](https://github.com/JPLOpenSource/STARS/tree/main).
An internal state machine is specified in FPP, the modeling language
for F Prime.

To program with external state machines, you typically do the following:

1. Use an external tool, such as the Quantum Modeler or Plant UML,
to express the state machine.
1. Use the STARS autocoder to generate an implementation from the model.
1. Write a small amount of code to make the FPP model aware of
the implementation.
1. Instantiate the state machine in one or more F Prime components.
1. In the component implementations, write code that interacts with
the state machine instances.

Steps 1,2, and 5 are described in the STARS documentation.
Steps 3 and 4 are described in the
[_The FPP User's Guide_](https://nasa.github.io/fpp/fpp-users-guide.html).
In the rest of this document, we will focus on the design of internal
state machines.

## 3. FPP Modeling and Code Generation

To work with internal state machines in FPP, you do the following:

1. Define a state machine _M_, specifying its behavior.
1. Add an instance _I_ of _M_ to a component _C_.
1. In the implementation of _C_, write code that interacts with the
generated code for _I_.

Steps 1 and 2 are fully documented in 
[_The FPP User's Guide_](https://nasa.github.io/fpp/fpp-users-guide.html).
Here we focus on the generated code for state machines and for
components that include state machine instances.

## 3.1. State Machines

TODO

## 3.2. State Machine Instances
