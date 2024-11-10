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

## 4. State Machine Definitions

In this section we describe the generated code for state machine definitions.
This code is generated into files _M_ `StateMachineAc.hpp` and _M_ 
`StateMachineAc.cpp`
when you run `fpp-to-cpp` on an FPP model that includes a definition
of a state machine _M_.

In the ordinary way of programming with F Prime, it is unlikely
that you will directly interact with the code described here.
Instead, you will use the component interface to state machine instances
described in the next section.

### 4.1. The State Machine Base Class

Each state machine definition _M_ in the FPP model becomes a C++ base class _M_ 
`StateMachineBase`.
This class is enclosed in the namespaces, if any, corresponding to the modules
that enclose the definition in FPP.
For example, a state machine whose qualified name is `A.B.M` in FPP becomes a 
class `A::B::MStateMachineBase` in C++.
The base class provides a partial implementation which is completed when
the state machine is instantiated.

### 4.2. The Public Interface

Each generated state machine has the following public interface.

**Types:**

* There is an enumeration representing the states of the state machine.
These are the leaf states specified in the FPP model
together with a special uninitialized state.

* There is an enumeration representing the signals of the state machine.
These are the signals specified in the FPP model together with a
special signal that represents the initial transition on startup.

**Member functions:**

* There is a function `getState` for getting the current state
of the state machine.

* There is one function `sendSignal` _s_ for each signal _s_
specified in the FPP model.
If the signal _s_ carries a value of type _T_, then this function has one
formal parameter of type _paramType(T)_; otherwise it has no formal parameters.
Here _paramType(T)_ means (1) _T_ if _T_ is a primitive type; otherwise (2)
`const` reference to `Fw::StringBase` if _T_ is a string type; otherwise
(3) `const` reference to _T_.

### 4.3. The Protected Interface

**Constructors and destructors:**
There is a zero-argument constructor and a destructor.

**Initialization:**
There is a function `initBase` with a single formal parameter `id`
of type `FwEnumStoreType`.
This function must be called on a state machine instance before
any signals are sent to the instance.
The parameter `id` represents the state machine identifier.
The type is `FwEnumStoreType` because the state machine identifier
type is an enumeration defined in the subclass.

**Actions:**
There is one pure virtual function `action_` _a_ for each action
_a_ specified in the FPP model.
Each action returns `void` has a formal parameter `signal` of type `Signal`.
If the action has a type _T_, then there is a second
formal parameter of type _paramType(T)_.

**Guards:**
There is one pure virtual `const` function `guard_` _g_ for each guard
_g_ specified in the FPP model.
Each guard returns `bool` and has a formal parameter `signal` of type `Signal`.
If the guard has a type _T_, then there is a second
formal parameter of type _paramType(T)_.

**Member variables:**
Each state machine base class has the following member variables:

* A member `m_id` of type `FwEnumStoreType`.
This variable records the current state of the state machine,
represented as `FwEnumStoreType`.
The initial value is zero.

* A member `m_state` of type `State`.
This variable records the current state of the state machine.
The initial value is `State::__FPRIME_AC_UNINITIALIZED`.

### 4.4. The Private Interface

For each state _S_ and choice _C_ in the state machine there is one
entry function for _S_ or _C_.
This function implements the entry behavior for _S_ or _C_
as specified in 
[_The FPP Language Specification_](https://nasa.github.io/fpp/fpp-spec.html).

## 5. State Machine Instances in Components

TODO
