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

1. Define one or more state machines, specifying their behavior.

1. Add one or more instances of the state machines defined in step 1 to a 
   component _C_.

1. In the implementation of _C_, write code that interacts with the
generated code for the instances defined in step 2.

Steps 1 and 2 are fully documented in 
[_The FPP User's Guide_](https://nasa.github.io/fpp/fpp-users-guide.html).
Here we focus on the generated code for state machines and for
components that include state machine instances.

## 4. State Machine Definitions

In this section we describe the generated code for a state machine definition 
_D_ with name _M_.
This code is generated into files _M_ `StateMachineAc.hpp` and _M_ 
`StateMachineAc.cpp`
when you run `fpp-to-cpp` on an FPP model that includes _D_.

In the ordinary way of programming with F Prime, it is unlikely
that you will directly interact with the code described here.
Instead, you will use the component interface to state machine instances
described in the next section.
Therefore, if your primary interest is to program with F Prime state
machines, you can skip this section.

### 4.1. The State Machine Base Class

Each state machine definition _D_ in the FPP model becomes a C++ base class _M_ 
`StateMachineBase`, where _M_ is the unqualified name of the definition.
This class is enclosed in the namespaces, if any, given by the qualified
name of _D_.
For example, a state machine definition whose qualified name is `A.B.M` in FPP 
becomes a class `A::B::MStateMachineBase` in C++.
The base class provides a partial implementation which is completed when
the state machine is [instantiated](#state-machine-instances).

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


<a name="state-machine-instances"></a>
## 5. State Machine Instances in Components

In this section we describe the generated code for instances of state machines
that are part of a component _C_.
This code is part of t of the auto-generated base class for _C_.
Note that in general there may be any number of instances of any number
of state machines.

### 5.1. State Machine Identifiers

There is an enumeration `SmId` with numeric type `FwEnumStoreType`
that represents the state machine identifiers.
There is one enumerated constant for each state machine instance
in _C_.

<a name="state-machine-impl"></a>
### 5.2. State Machine Implementation Classes

There is one implementation class for each state machine definition
_M_ that is the type of a state machine instance in _C_.
For example, if a state machine instance
```
state machine instance m: M
```
appears in _C_, then _C_ contains an implementation class
for _M_.
This class has the following properties:

* It is a protected inner class of the auto-generated base class for _C_.

* Its name is the fully qualified name of the state machine,
with the dots replaced by underscores.
For example, if a state machine has name `A.B.M` in FPP,
the C++ name of its implementation class is `A_B_M`.

* It is a public derived class of the state machine base
class for _M_ that we described in the previous section.

Each state machine implementation class has the following
elements in its interface.

**Member variables:**
There is a member `m_component` that is a reference to
the enclosing component instance.
This way the state machine instance can call into
the interface of the component instance.

**Construction:** There is a public constructor 
that takes a reference `*this` to the enclosing component
as an argument.
It initializes the member variable described above.

**Initialization:** There is a public function
`init` with one formal parameter `smId` of type `SmId`.
This function casts its argument to `FwEnumStoreType`
and calls the function `initBase` defined in the base class.
Thus it provides a type-safe way to initialize the state
machine ID.

**State ID:** There is a public function `getId`
that returns the state machine ID.
It gets the value out of the `m_id` field defined
in the base class and casts it to `SmId`.
Thus it provides a type-safe way to get the state
machine ID.

**Actions:** For each action _a_ of _M_, there is one private function that 
implements the pure virtual function for _a_ defined in the base class.
The implementation calls the pure virtual function in the interface of _C_
that corresponds to to _M_ and _a_ (see below).
It passes in the state machine ID of _m_.

**Guards:** For each guard _g_ of _M_, there is one private function that 
implements the pure virtual function for _g_ defined in the base class.
The implementation calls the pure virtual function in component _C_ that 
corresponds to to _M_ and _g_ (see below).
It passes in the state machine ID of _m_ and returns the Boolean value returned 
by that function.

### 5.3. State Machine Instances

For each state machine _m_ in the FPP component model, there is
one private member variable `m_stateMachine_` _m_.
Its type is the [state machine implementation class](#state-machine-impl)
corresponding to the type of _m_.

### 5.4. Protected Member Functions

#### 5.4.1. Implemented Functions

The following functions have complete implementations and are available for you 
to call in the derived class that implements _C_.

**State getter functions:**
For each state machine instance _m_ in _C_, there is a `const` function
_m_ `_getState` that gets the current state of _m_.

**Signal send functions:**
For each state machine instance _m_, and for each signal _s_ defined
in the state machine _M_ instantiated by _m_, there is a function
_m_ `_sendSignal_` _s_ for sending signal _s_ to state machine instance _m_.
If _s_ carries data of type _T_, then this function has a single
formal parameter of type _paramType(T)_; otherwise it has no
formal parameters.

Calling a signal send function puts a message on the queue of the
current instance of the component _C_.
When the message is dispatched, the auto-generated code calls
the function that sends the signal to the state machine.
This way state machines can safely send signals when they
are doing actions.

#### 5.4.2. Pure Virtual Functions

The following functions are pure virtual in the generated base class.
You must implement them in the derived class that implements _C_.
When you generate a C++ component implementation template for _C_, you get
a stub for each of these functions that you can fill in.

**Action functions:**
TODO

**Guard functions:**
TODO

**Overflow hook functions:**
TODO

### 5.5. Private Member Functions

TODO
