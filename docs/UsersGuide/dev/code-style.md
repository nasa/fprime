# Code and Style Guidelines

When contributing to F´ it is important to understand the style guidelines that F´ uses across the various products
provided by the team. The information in this guide may not be complete and will evolve as the project continues. When
unclear, follow reasonable conventions for the language at hand.

**Note:** These guidelines are for submissions to the F´ framework. Projects using F´ should adapt them as needed.

## F´ and Flight Software

F´ is a flight software framework. Flight software is embedded software intended to run on "flight systems" like
satellites, spacecraft, instruments, and more. Reliability is one of the most important goals of flight software since
these systems are often operating for long durations without the option to service failing software.  As such,
contributions should be easy to read and easy to review. Remember, complexity and optimization sometimes come at the
cost of readability or maintainability and that can mean simpler solutions are more appropriate.

## C++

C++ in F´ is written primarily following the [embedded C++](https://en.wikipedia.org/wiki/Embedded_C%2B%2B) style with
some notable deviations. In addition, F´ follows the JPL Coding Standards as adapted from C. C++ is written under the
[C++11 standard](https://en.cppreference.com/w/cpp/11), however; some C++11 features are avoided to support some
incomplete C++11 implementations. Code format is based on the clang-format Chromium style.  There are also a number of
"rules-of-thumb" that C++ implementors should know.

### Embedded C++ Usage

F´ is written with a modified embedded C++ style. Please note, unit test implementations often disregard these
requirements as they run off-platform in a development context. The core points of embedded C++ that F´ uses are:

1. No exceptions, code must compile with `-fno-exceptions`
2. No uses of templates nor the Standard Template Library
3. No typeid or run-time type information

F´ has a few rules that deviate from the embedded C++ standard. This is done to increase safety, increase readability,
and enable modern C++ development. These deviations are:

1. Use namespaces to reduce naming conflicts.
2. Use `std::numeric_limits` for mix/max values. Template implementations of `std::numeric_limits` have been approved.
3. **Use** `static_cast`, `reinterpret_cast`, and `const_cast` instead of dangerous C-style and `dynamic_cast` casting.
4. Limited use of multiple inheritance and virtual base classes are permitted. See rules of thumb.

### JPL Coding Standards

There are additional rules from the software's heritage as NASA/JPL Flight Software (see 
[Power of 10 Rules](https://en.wikipedia.org/wiki/The_Power_of_10:_Rules_for_Developing_Safety-Critical_Code) ) and the
framework itself. The guidelines were adapted from C guidelines and are summarized below:

1. No recursion; No GOTOs
2. Loops must have a fixed-bound
3. No dynamic memory allocation after initialization
4. Use `FW_ASSERT` to validate function inputs and computation
5. Restrict data to the smallest necessary scope
6. Check function return values or explicit discard with `(void)`
7. Avoid the preprocessor and especially complex uses of the preprocessor
8. Prefer `Fw` and `Os` implementations. e.g. use `FW_ASSERT` and `Os::Mutex` over `cassert` and `std::mutex`
9. Compile without warnings, errors, and static analysis failures (e.g. pass continuous integration)
10. Do not use `Os::Task::delay` to synchronize between threads
11. Explicit enumeration values should be specified for all values or none at all

### C++11 Usage

F´ has been updated to support C++11. Some C++11 implementations are incomplete requiring some deviations from C++11.
Thus, there are some things that developers should be aware of when using C++11 features:

1. Many C++11 features use templates under the hood.  Prefer `Fw` and `Os` implementations for these features.
2. Use of `std::atomic` is known to not work in some cases. Use is avoided.

### Clang Format

F´ uses clang format based on Chromium. Currently, clang-format does not properly handle `PRIVATE`, `PROTECTED`, and
`STATIC` access modifiers used to enable white-box unit testing. Several solutions are being considered to
fix this and, once complete, the full code base will be formatted. New submissions should attempt to format
appropriately and edit around these modifiers as necessary. A `.clang-format` file is supplied at the root of the
repository.

### Rules of Thumb

This section will document rules of thumb when developing F´ C++.  These augment the formal guidelines to help users
develop code that fits well with the entirety of F´. 

#### Documentation

Components need to be documented through an Software Description Documents (SDD) placed in a `docs` subfolder. This SDD
should capture:

1. Requirements
2. Design
3. Implementation and Usage Notes

C++ documentation is generated using doxygen. [Doxygen-style comments](https://www.doxygen.nl/manual/docblocks.html)
should be used to annotate functions and variables. C++ APIs and libraries must be documented to produces usable doxygen
documentation. Components should be documented in a similar fashion but the SDD captures the primary documentation for
components.

### Asserts, WARNING Events, and FATAL Events

Software should operate correctly in the event of off-nominal behavior. Asserts and events can be used to handle and/or
report these occurrences. Runtime asserts (i.e. `FW_ASSERT`) are used to ensure software integrity and assumptions.
These are used to ensure that arguments passed to functions are within bounds, return values make since, and other
confidence checks. Runtime assertions should never be used to validate user inputs nor results of hardware interaction
as these may cause errors outside software control.

In these cases, an event should be emitted to report the error, and the software should appropriately continue execution
even in the case of erroneous input. The severity of the event emitted should be determined by the following rules:

1. `WARNING_LO` events reflect issues localized to this component where recovery is possible.
2. `WARNING_HI` events reflect issues affecting the larger system where recovery is possible.
3. `FATAL` events reflect issues where recovery is not possible. The system is typically restarted in this case.
4. `WARNING_HI` and `FATAL` events must be documented in the SDD and approved as part of the submission review.

#### Use of Multiple-Inheritance and Virtual Base Classes

Many of F´ constructs are required to inherit from an autocoded class that then inherits from framework base classes.
This can present a problem as this precludes inheritance from other structures. Thus limited multiple inheritance is
permitted. Basic guidelines are presented here, however; the use of multiple-inheritance will likely be discussed during
the submission review.

1. Only use multiple-inheritance when absolutely required
2. Avoid inheritance from more than one hand-coded class
3. Avoid inheritance from hand-coded descendants of autocoded classes and parents (e.g. `Fw::PortBase` and
   `Fw::PassiveComponentBase`) not. i.e avoid inheritance from other Components, Ports, or classes that do.
4. Avoid multiple inheritance paths that derive from a common base class a.k.a
   [the diamond problem](https://www.cprogramming.com/tutorial/virtual_inheritance.html)

Normal usage for multiple and virtual types of inheritance in F´ is to set up
[interfaces](https://en.wikipedia.org/wiki/Interface_(Java)) as is often done in Java. This consists of a virtual C++
base class defining only functions. An F´ class or component would then use multiple-inheritance to inherit those
function definitions with the intent of implementing them. This can be used to bridge autocoded F´ functionality and C++
functionality. For example, a component can supply F´ port call functionality to supporting classes through this style of interface.

## Python

Python code is much easier as it is provided by the black formatter. Python code must pass a black format check. To
format code using black, install black per the [Python Development Guide](./py-dev.md) and run `black <path to code>`.