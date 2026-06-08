---
name: fprime-cpp-design
description: Authoritative C/C++ design rules and idioms for F Prime flight software. Use this skill in two directions — as a *developer-side* reference when writing or modifying F Prime C++ code (what idioms to follow, what features to avoid, which F Prime types to prefer), and as a *reviewer-side* reference for finding-class names and severity hints when flagging C/C++ design violations. Trigger on any work that touches F Prime C/C++ source: component implementations, drivers, services, framework types, OSAL code, or unit-test infrastructure. Keywords: F Prime, C++14, FW_ASSERT, Fw::Buffer, Fw::String, JPL coding standard, dynamic memory, RTTI, exceptions.
---

# Skill: F Prime C/C++ design rules

The **single source of truth** for the C/C++ design rules F Prime
flight software is held to. Used in two directions:

- **Developer-side.** §1 is positive guidance for writing or
  modifying F Prime C++ code.
- **Reviewer-side.** §2 gives the finding-class name to use per
  rule; §3 gives severity hints. Combine with
  `_shared/skills/triage-classifier.skill.md`.

External references in §4 are authoritative for their specific
clauses; this skill summarizes the rules the agents enforce. Where
this skill and an external reference disagree, this skill wins for
review purposes.

---

## 1. The rules

Rules are grouped by theme. Numbers are stable and may be cited in
PR comments and in other agent files.

### A. Memory & lifetime

#### CPP-1 — No dynamic memory after initialization

Allocate all dynamic storage during component initialization or
boot. After init, no `new` / `delete` / `malloc` / `free`, and no
implicit allocation through STL containers, `std::string`, or
exception machinery (see CPP-25).

Allowed: pre-sized arrays sized at compile or init time, `Fw::Buffer`
references to memory owned elsewhere (CPP-2), object pools allocated
once at boot.

```cpp
// Avoid (allocation post-init)
char* scratch = new char[len];

// Prefer (sized at declaration)
static constexpr U32 SCRATCH_BYTES = 256;
U8 m_scratch[SCRATCH_BYTES];
```

#### CPP-2 — `Fw::Buffer` ownership transfers must close in every branch

Any function receiving an `Fw::Buffer` must, in every branch
(success, failure, error, early-return), either **transfer ownership
out** (pass to another component, store in a tracked slot) or
**return it to the sender**. A dropped `Fw::Buffer` leaks; a
double-transferred one is a use-after-free.

#### CPP-17 — Follow Rule of Three / Rule of Five

If a class manages a resource (raw pointer, file handle, externally-
owned memory), implement (or `= delete`) all of: destructor, copy
constructor, copy assignment, and — under Rule of Five — move
constructor and move assignment. Most F Prime components are not
intended to be copied; mark the copy ops `= delete` explicitly.

#### CPP-19 — Initialize all variables

Every variable — member, local, global — has an explicit
initializer. Uninitialized reads are undefined behavior. Prefer
member-initializer lists for class members; use brace-initialization
(`Type x{};` or `Type x{value};`) for locals.

#### CPP-20 — Destructors are virtual or protected non-virtual

A class meant to be inherited from declares either a `virtual`
destructor (polymorphic deletion is safe) or a `protected`
non-virtual destructor (deletion through the base pointer is
prevented).

### B. Asserts and untrusted inputs

#### CPP-4 — `FW_ASSERT` is for programming errors only

`FW_ASSERT` is for **programmer-controllable invariants** —
guaranteed by construction. It is **not** input validation. Never
`FW_ASSERT` on a value reachable from:

- A ground command argument (see
  `_shared/skills/fprime-ground-input-tracing.skill.md`).
- A hardware input — packet, register read, port from a driver (see
  `_shared/skills/fprime-hardware-input-tracing.skill.md`).
- Any off-device data crossing a hub / bridge / driver boundary.

Validate untrusted inputs and return the error through the
component's contract (typically a `VALIDATION_ERROR` command
response, a sized return, or a soft-failure event). The
security-review agent mirrors this as `ground-reachable-assert` /
`hardware-reachable-assert`; CPP-4 is the C++-side framing of the
same finding.

### C. F Prime types and idioms

#### CPP-3 — Always use fixed-size numerical types

For numerical fields (integers, floats), always use a fixed-size
type unless an external API forces a non-fixed-size type. Fixed-size
types are either:

- The explicit fixed-size types: `U8`, `I8`, `U16`, `I16`, `U32`,
  `I32`, `U64`, `I64`, `F32`, `F64`.
- The configurable `Fw*` types whose width the project pins to a
  known fixed-size type at config time (e.g., `FwSizeType`,
  `FwIndexType`).

Bare C/C++ numerical types (`int`, `unsigned`, `long`, `size_t`,
`float`, `double`) are forbidden in F Prime code except at external
API boundaries that require them.

#### CPP-28 — Prefer configurable `Fw*` types when range may vary across projects

When the appropriate width for a value depends on the project (e.g.,
buffer sizes, container indices, time tick counts), use the
configurable `Fw*` type rather than a bare fixed-size type. The
configurable type lets the framework retarget bit widths for a
platform without diff-touching every component.

```cpp
// Avoid (fixed at U32; doesn't follow the project's size config)
U32 m_capacity;

// Prefer (project-configurable)
FwSizeType m_capacity;
```

CPP-3 and CPP-28 apply only to **numerical** fields. Non-numerical F
Prime types (`Fw::Time`, `Fw::CmdResponse`, `Fw::Buffer`,
`Fw::String`) are governed by their own rules in this section
(CPP-2, CPP-21, CPP-23, CPP-24).

#### CPP-21 — No C-style arrays in interfaces; pair array + length

`void f(U8* buf, U32 len);` is brittle (lose `len`, lose memory
safety). Use `Fw::Buffer`, `Fw::ByteArray` /
`Fw::ConstByteArray` (in `Fw/Types/`), or an `Fw/DataStructures`
container (CPP-22) that bundles the data and its length together.

#### CPP-22 — Prefer `Fw/DataStructures` containers

For F Prime arrays, queues, ring buffers, etc., use the bounded,
deterministic container types under `Fw/DataStructures/` rather
than rolling a bespoke C array or pulling in a `std::*` container
(CPP-25 forbids the latter).

#### CPP-23 — Use FPP modeled types on ground-facing interfaces

Commands, events, parameters, and telemetry channels — anything the
ground operator sees — are declared in `.fpp` files and autocoded
into strongly-typed C++ bindings. Use those autocoded types
directly; do not hand-roll C structs and serialize them yourself.
The FPP modeled types are the ground-system contract.

#### CPP-24 — Prefer `Fw::String` over `char*`

`Fw::String` is fixed-size, bounded, and tracks length explicitly.
Prefer it for all string-bearing members and parameters. `char*` is
acceptable only for:

- String literals in source (e.g., `static const char* TAG = "MyComp";`).
- External API boundaries (OSAL, POSIX) that take `char*`, with
  `Fw::String::toChar()` used at the boundary.

### D. C++ language subset

#### CPP-5 — Code compiles cleanly as C++14

F Prime targets C++14. Newer features (C++17 `if constexpr`, C++20
ranges / concepts / modules) are not portable to every supported
toolchain. Verify on the project's reference toolchain.

#### CPP-6 — `nullptr` only

`NULL` and `0` are not null-pointer constants in F Prime code. Use
`nullptr` so the type system disambiguates pointer vs. integer.

#### CPP-7 — No lambdas; keep templates simple

Lambdas are forbidden: they implicitly synthesize classes (CPP-25
concerns), can allocate on capture (CPP-1), and obscure ownership of
captured state. Use a named functor or free function.

Templates are allowed but **simple**: one or two type parameters,
straightforward instantiations. Heavy meta-programming,
substitution-failure dispatch, and deep parameter-pack manipulation
are not acceptable.

#### CPP-8 — Prefer typed constants over `#define`

```cpp
// Avoid
#define MAX_RETRIES 5

// Prefer
static constexpr U32 MAX_RETRIES = 5;
```

`#define` constants have no type, no scope, no debugger visibility.
Complex macros (multi-statement, token-pasting, stringification
beyond simple logging) need justification.

#### CPP-9 — No C-style or function-style casts

```cpp
U32 x = (U32) y;   // avoid
U32 x = U32(y);    // avoid
U32 x = static_cast<U32>(y);   // prefer
```

C-style casts silently change category (static / const /
reinterpret / dynamic). Spell out the cast category.

#### CPP-10 — Avoid `reinterpret_cast` and `const_cast`

These are escape hatches. Rare and justified inline with a `//`
comment explaining why (typically: a vendor API requires a non-const
pointer, or a hardware register access requires reinterpreting a
buffer). An unjustified `reinterpret_cast` is a finding.

#### CPP-11 — Prefer `constexpr`, then `const`

Compile-time-evaluable → `constexpr`. Doesn't change after init →
`const`. Mutable storage is the last resort.

#### CPP-12 — No `using namespace` at file scope

File-scope `using namespace` pollutes the lookup space. A `using`
declaration inside a function body is acceptable.

#### CPP-13 — Prefer references over pointers

References are non-null by construction and can't be rebound. Use a
pointer only when null is a valid value or when lifetime semantics
require it.

#### CPP-14 — Avoid multiple inheritance except pure-virtual interfaces

Diamond inheritance and mix-ins with state are not used. The only
acceptable shape is inheriting from multiple **pure-virtual
interface** classes (no state, no implementation in the interface).

#### CPP-15 — Mark overrides with `override`

The keyword catches signature drift across the inheritance
hierarchy. Don't accidentally introduce a new function that looks
like an override but isn't.

#### CPP-16 — `friend` only for unit-test access

`friend` is for letting a unit-test fixture reach private members of
the class under test. Productive code does not use `friend` for
inter-component coupling.

#### CPP-18 — `explicit` constructors; call base constructors explicitly

`explicit` on single-argument constructors prevents implicit
conversions that hide intent. Base constructors are called by name
in the member-initializer list, not relied on to default-construct.

#### CPP-25 — No exceptions, RTTI, STL, `std::string`

F Prime flight code is compiled with exceptions and RTTI disabled
(`-fno-exceptions` and the no-RTTI flag) and deliberately avoids the
STL.

- `try` / `catch` / `throw`: forbidden.
- `dynamic_cast`, `typeid` (require RTTI): forbidden.
- `std::vector`, `std::map`, `std::list`, etc.: forbidden — use
  `Fw/DataStructures` (CPP-22).
- `std::string`: forbidden — use `Fw::String` (CPP-24).
- Other implicitly-allocating STL components: forbidden.

Trivial `<algorithm>` helpers (`std::min`, `std::max`,
`std::numeric_limits`) and `<cstdint>` types are acceptable.

### E. Correctness and robustness

#### CPP-29 — FW_ASSERT predicates must be side-effect-free

The expression inside `FW_ASSERT(...)` must have no observable side-effects.
`FW_ASSERT` can be compiled away entirely (`FW_ASSERT_LEVEL == 0`), so any
side-effect in the predicate — function calls that mutate state, I/O
operations, increments, assignments — would silently disappear from the
production binary.

Patterns to flag:

```cpp
FW_ASSERT(file.read(...) == OK);      // read() is side-effecting
FW_ASSERT(counter++ < MAX);           // increment is side-effecting
FW_ASSERT(init() == SUCCESS);         // init() mutates state
FW_ASSERT(queue.pop(item) == OK);     // pop() modifies queue
```

Correct pattern — separate the effect from the check:

```cpp
Status s = file.read(...);
FW_ASSERT(s == OK);
```

Pure accessors (const methods, getters, sizeof, static queries) are
acceptable inside `FW_ASSERT`.

#### CPP-30 — Numeric literals must not replace named constants without derivation

When a configuration value was previously expressed as a named constant or
derived from one, replacing it with a bare numeric literal is a finding.
The replacement must either:

- (a) derive from named constants with visible arithmetic
  (e.g., `FwAssertTextSize - TIMESTAMP_OVERHEAD`), or
- (b) be itself a new named constant with a documenting comment explaining
  how the value was chosen.

Bare numeric literals in configuration are acceptable only for truly
universal values (0, 1, nullptr) or when accompanied by an inline
derivation comment showing how the number was computed.

```cpp
// Avoid
constant AssertFatalAdapterEventFileSize = 150

// Prefer
@ Derived: FwAssertTextSize(256) - max_timestamp(40) - max_args(66) = 150
constant AssertFatalAdapterEventFileSize = FwAssertTextSize - 106
```

#### CPP-31 — No silent truncation on string/data copy

When copying variable-length data into a fixed-size destination, the code
must handle potential truncation explicitly. Silent truncation — where
the stored value differs from the input with no indication to the caller
— is a finding.

Acceptable handling:

- (a) Check the return value of the copy/format function and propagate
  an error or emit a warning event if truncation occurred.
- (b) Validate the input length BEFORE the copy and reject/error on
  oversized inputs.
- (c) Document (with an inline comment AND a note in the PR description)
  why truncation is acceptable in this specific case (e.g., the
  truncated value is only used for display/logging).

Patterns to flag:

```cpp
// Truncation with no check
m_path_storage = filepath;  // FileNameString silently truncates if |filepath| > max
Fw::StringUtils::string_copy(dst, src, dstSize);  // return value ignored
```

Common F Prime APIs that can truncate and whose return values must be
checked or whose preconditions must be validated:

- `Fw::String::operator=(const CHAR*)` — truncates to StringBase capacity
- `Fw::StringUtils::string_copy()` — returns actual bytes copied
- `Fw::StringBase::format()` — truncates if result exceeds buffer
- `Fw::ExternalString::operator=()` — truncates to external buffer size

#### CPP-32 — Check return values of fallible operations

Functions that return a status, size, or success indicator must have their
return value checked — or explicitly cast to `(void)` with an inline
comment explaining why the result is intentionally discarded.

Patterns to flag:

```cpp
str.format("...", args);                    // format() returns bool/size — ignored
Fw::StringUtils::string_copy(d, s, n);     // returns actual length — ignored
file.read(buf, size);                       // returns Status — ignored
serialBuffer.serialize(value);             // returns SerializeStatus — ignored
```

Correct patterns:

```cpp
bool ok = str.format("...", args);
FW_ASSERT(ok);  // or handle truncation

FwSizeType copied = Fw::StringUtils::string_copy(d, s, n);
// Use copied or assert it equals expected

// Intentional discard (acceptable when documented):
(void)str.format("...", args);  // truncation acceptable for log-only string
```

This rule does NOT apply to functions documented as infallible
(pure setters, void returns, trivial accessors). It applies specifically
to operations that can fail or produce partial results.

#### CPP-33 — Extract general-purpose logic to shared utilities

If a PR introduces logic in a component implementation that:

- (a) operates on general data types (strings, buffers, numeric arrays), AND
- (b) has no dependency on the component's specific state or ports, AND
- (c) would be useful in more than one component,

then it should be implemented in (or proposed for) the appropriate utility
module (`Fw::StringUtils`, `Fw/DataStructures`, `Utils/`, `Os/`) rather
than inlined in the component.

Examples:

- String truncation (leading or trailing) → `Fw::StringUtils`
- CRC computation → `Utils/Hash`
- Bounded queue logic → `Fw/DataStructures`
- Path manipulation (dirname, basename) → `Os` or `Fw::StringUtils`

#### CPP-34 — Prefer bounded `for` loops; `while` is discouraged

All loops must have a provable upper bound (per JPL C coding
standard, CPP-27). Use `for` for counted iteration so init, bound,
and increment are collocated. `while` scatters loop control and is
discouraged except for the program main loop (`while (true)` /
`for (;;)` in a task entry point or dispatcher).

### F. External authoritative references

#### CPP-26 — F Prime style guidelines

The project-wide F Prime style guidelines are the wiki page linked
in §4. Where this skill and the wiki agree, this skill shorthands
the rule; where the wiki has more detail (naming, formatting,
header guards, include ordering), use the wiki.

#### CPP-27 — JPL C coding standard

For C-language considerations that bleed into C++ (loop bound
verification, side-effects in conditions), the JPL C coding standard
linked in §4 is authoritative. F Prime adopts it where applicable.

---

## 2. Reviewer finding-class mapping

| Rule | Finding-class | Notes |
|---|---|---|
| CPP-1  | `cpp-dynamic-memory-post-init` | |
| CPP-2  | `cpp-fw-buffer-ownership` | Related to `security-review`'s `general-vulnerability/use-after-free`. |
| CPP-3  | `cpp-non-fixed-size-numerical-type` | Numerical fields only. |
| CPP-4  | `cpp-fw-assert-on-untrusted-input` | Mirror of `security-review`'s `ground-reachable-assert` / `hardware-reachable-assert`. |
| CPP-5  | `cpp-cxx14-violation` | |
| CPP-6  | `cpp-null-vs-nullptr` | |
| CPP-7  | `cpp-lambda` / `cpp-template-complexity` | Two sub-classes. |
| CPP-8  | `cpp-define-instead-of-constexpr` | |
| CPP-9  | `cpp-c-style-cast` | |
| CPP-10 | `cpp-reinterpret-or-const-cast-unjustified` | |
| CPP-11 | `cpp-missing-constexpr-or-const` | |
| CPP-12 | `cpp-using-namespace-file-scope` | |
| CPP-13 | `cpp-pointer-where-reference-fits` | |
| CPP-14 | `cpp-multiple-inheritance-with-state` | |
| CPP-15 | `cpp-missing-override` | |
| CPP-16 | `cpp-friend-in-production` | |
| CPP-17 | `cpp-rule-of-three-or-five` | |
| CPP-18 | `cpp-non-explicit-ctor` / `cpp-implicit-base-ctor` | Two sub-classes. |
| CPP-19 | `cpp-uninitialized-variable` | |
| CPP-20 | `cpp-non-virtual-public-dtor` | |
| CPP-21 | `cpp-c-style-array-in-interface` | |
| CPP-22 | `cpp-bare-container-not-fw-data-structure` | |
| CPP-23 | `cpp-non-fpp-modeled-ground-interface` | |
| CPP-24 | `cpp-char-pointer-where-fw-string-fits` | |
| CPP-25 | `cpp-banned-cxx-feature` (suffix with the specific feature, e.g., `/exceptions`, `/RTTI`, `/STL`, `/std-string`) | |
| CPP-26 | `cpp-style-guide-violation` | Catch-all; cite the wiki section. |
| CPP-27 | `cpp-jpl-c-standard-violation` | Catch-all; cite the section. |
| CPP-28 | `cpp-bare-fixed-size-where-configurable-fits` | Numerical fields where range varies across projects. |
| CPP-29 | `cpp-assert-side-effect` | `**must fix**` always. |
| CPP-30 | `cpp-magic-number-replacing-constant` | `**must fix**` when behavioral; `**could fix**` for new code. |
| CPP-31 | `cpp-silent-truncation` | `**must fix**` for paths/keys/identifiers; `**could fix**` for display. |
| CPP-32 | `cpp-ignored-return-value` | `**must fix**` for I/O/serialization; `**could fix**` for display. |
| CPP-33 | `cpp-inlined-utility` | `**suggestion**` for one-liners; `**could fix**` for multi-line. |
| CPP-34 | `cpp-while-loop-for-counted-iteration` / `cpp-unbounded-loop` | Two sub-classes. |

Finding-class names are stable strings: they appear in the inline
comment HTML footer (`finding-key` hash inputs). Renaming a class
breaks the re-review state mechanism
(`_shared/skills/re-review-state.skill.md`) — coordinate any rename
across all agents referencing it.

---

## 3. Triage hints

The triage classifier in `_shared/skills/triage-classifier.skill.md`
is authoritative; this section narrows the decision per cluster.

- **Memory & lifetime (CPP-1, 2, 17, 19, 20):** default `**must
  fix**`. Downgrade to `**suggestion**` only when the violation is
  in obviously test-only code AND expressible as a one-line diff.
  Never `**could fix**`.
- **Asserts on untrusted inputs (CPP-4):** always `**must fix**`.
  Mirrors `security-review`'s framing.
- **F Prime type idioms (CPP-3, 21, 22, 23, 24, 28):** default
  `**suggestion**` with a fenced suggestion block. Upgrade to
  `**must fix**` when the violation is on a ground-facing interface
  (CPP-23) — the autocoded FPP type is the ground-system contract.
- **Language subset (CPP-5–16, 18, 25):** default `**suggestion**`
  or `**could fix**`. Upgrade to `**must fix**` when:
  - CPP-25 introduces an exception or RTTI dependency.
  - CPP-10 introduces an unjustified `reinterpret_cast` /
    `const_cast` on a flight path.
  - CPP-14 introduces multiple inheritance with state.
- **External references (CPP-26, 27):** default `**could fix**` for
  cosmetic style hits; `**suggestion**` when a concrete fix is
  expressible.
- **Correctness and robustness (CPP-29, 30, 31, 32, 33, 34):**
  - CPP-29 (assert side-effects): always `**must fix**`.
  - CPP-30 (magic numbers): `**must fix**` when replacing a named
    constant with a behavioral change; `**could fix**` for new code.
  - CPP-31 (silent truncation): `**must fix**` when the truncated
    value is used for logic (paths, keys, identifiers); `**could
    fix**` when used only for human-readable display.
  - CPP-32 (ignored return values): `**must fix**` for I/O,
    serialization, and path-building operations; `**could fix**`
    for display-only formatting.
  - CPP-33 (inlined utilities): `**suggestion**` for small
    one-liners; `**could fix**` for multi-line logic blocks.
  - CPP-34 (while / unbounded loops): `**suggestion**` when a
    `while` is used for simple counted iteration and the fix is a
    mechanical conversion to `for`. `**must fix**` when a loop
    has no provable upper bound and is not a program main loop.

**Preexisting violations.** Any violation the PR did not introduce
or widen (per `_shared/skills/pr-diff-scoping.skill.md`) is
`**future work**`, never `**must fix**`.

---

## 4. External references

- F Prime style guidelines:
  <https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines>
- JPL C coding standard:
  <https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf>
- F Prime user manual (memory management, ground interface, state
  machines): `docs/user-manual/framework/`.
- F Prime data-structure types: `Fw/DataStructures/`.
- F Prime byte-array types: `Fw/Types/ByteArray.hpp`,
  `Fw/Types/ConstByteArray.hpp`.

Cite the section / file rather than restating the source verbatim.
