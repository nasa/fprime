---
name: fprime-cpp-design
description: Authoritative C/C++ design rules and idioms for F Prime flight software. Use this skill in two directions — as a *developer-side* reference when writing or modifying F Prime C++ code (what idioms to follow, what features to avoid, which F Prime types to prefer), and as a *reviewer-side* reference for finding-class names and severity hints when flagging C/C++ design violations. Trigger on any work that touches F Prime C/C++ source: component implementations, drivers, services, framework types, OSAL code, or unit-test infrastructure. Keywords: F Prime, C++14, FW_ASSERT, Fw::Buffer, Fw::String, JPL coding standard, dynamic memory, RTTI, exceptions.
---

# Skill: F Prime C/C++ design rules

This skill is the **single source of truth** for the C/C++ design rules
that F Prime flight software is held to. It is consumed in two
directions:

- **Developer-side.** When writing or modifying F Prime C++ code, read
  §2 as positive guidance: what to do, what to prefer, what to avoid.
  Each rule states the F Prime idiom first; the rationale and the
  "don't" form follow.
- **Reviewer-side.** When reviewing C/C++ changes, §3 gives the
  finding-class name to use on each rule (so every reviewer agent
  speaks the same finding-class vocabulary), and §4 gives the severity
  hints for the triage classifier.

The rules below are F Prime's house style. The full external
references — the F Prime style guide and the JPL C coding standard —
are linked in §5; this skill summarizes the rules the agents enforce
on PRs. Where this skill and an external reference disagree, this
skill wins for review purposes (the multi-agent flow's source of
truth).

---

## 1. How to use this skill

### Developer audience

Read §2 cluster-by-cluster. Each numbered rule is a positive guideline
("prefer X", "use Y") with the anti-pattern form spelled out so you
can recognize and avoid it. Use the rule numbers as shorthand when
discussing F Prime code with a teammate or with another agent
("CPP-7, no lambdas").

### Reviewer audience

When flagging a C/C++ design issue:

1. Identify which rule in §2 the offending code violates.
2. Look up the rule's finding-class in §3 (`cpp-<short-name>`) and use
   that in the inline comment's HTML footer per the review contract §7.
3. Pick a triage tag using the severity hints in §4 plus
   `_shared/skills/triage-classifier.skill.md`.

Rule numbers in this skill are stable and may be cited in PR comments
and in other agent files.

---

## 2. The rules

Rules are grouped by theme. The numbering matches the original
inline list in `fprime-code-review.agent.md` so any code already
referencing "CPP-N" keeps its meaning.

### A. Memory & lifetime

#### CPP-1 — No dynamic memory after initialization

F Prime is a flight-software framework: deterministic memory behavior
matters more than the convenience of a heap. **Allocate all dynamic
storage during component initialization or boot.** After
initialization, the steady-state should make no `new` / `delete` /
`malloc` / `free` calls, and no implicit allocation through STL
containers, `std::string`, or exception machinery (see CPP-25).

Allowed: pre-sized buffers / arrays sized at compile or init time,
`Fw::Buffer` references to memory owned elsewhere (see CPP-2), object
pools allocated once at boot.

Anti-pattern:

```cpp
void MyComp_handler::doWork() {
    char* scratch = new char[len];      // allocation post-init
    process(scratch);
    delete[] scratch;
}
```

Preferred form:

```cpp
static constexpr U32 SCRATCH_BYTES = 256;
U8 m_scratch[SCRATCH_BYTES];            // sized at init / declaration

void MyComp_handler::doWork() {
    FW_ASSERT(len <= SCRATCH_BYTES, len);
    process(m_scratch);
}
```

#### CPP-2 — `Fw::Buffer` ownership transfers must close in every branch

`Fw::Buffer` is F Prime's unit of borrowed-memory ownership. Any
function that receives an `Fw::Buffer` must, in every branch (success,
failure, error, early-return), either **transfer ownership out**
(pass it to another component, store it in a tracked slot) or
**return it to the sender** (the port marked for return). A dropped
`Fw::Buffer` is a leak; a double-transferred `Fw::Buffer` is a
use-after-free.

When reading code, follow the buffer through every branch of the
handler. When writing code, keep the buffer's disposition obvious
syntactically — early-returns must close the buffer first.

#### CPP-17 — Follow Rule of Three / Rule of Five

If a class manages a resource (raw pointer, file handle, externally-
owned memory), implement (or delete) all of: destructor, copy
constructor, copy assignment, and — under Rule of Five — move
constructor and move assignment. Don't leave one defaulted and the
other defined.

Most F Prime component classes are not intended to be copied; the
corresponding copy operations are typically `= delete`d explicitly.
Make the deletion visible rather than relying on implicit deletion.

#### CPP-19 — Initialize all variables

Every variable — member, local, global — has an explicit initializer.
Uninitialized memory reads are undefined behavior, and several flight
defects in F Prime history have traced to this rule.

Prefer member-initializer lists for class members; use
brace-initialization (`Type x{};` or `Type x{value};`) at declaration
for locals.

#### CPP-20 — Destructors are virtual or protected non-virtual

A class meant to be inherited from (any base class) declares either a
`virtual` destructor or a `protected` non-virtual destructor. The
protected non-virtual form prevents deletion through the base
pointer when polymorphic deletion is not intended; the virtual form
makes polymorphic deletion safe.

### B. Asserts and untrusted inputs

#### CPP-4 — `FW_ASSERT` is for programming errors only

`FW_ASSERT(predicate)` is the assertion mechanism for **programmer-
controllable invariants** — a value the programmer guarantees by
construction. It is **not** an input validation mechanism. Never use
`FW_ASSERT` on a value that can be reached from:

- A ground operator's command argument (see
  `_shared/skills/fprime-ground-input-tracing.skill.md`).
- A hardware input (a packet, a register read, a port from a driver;
  see `_shared/skills/fprime-hardware-input-tracing.skill.md`).
- Any off-device data crossing a hub, bridge, or driver boundary.

Untrusted inputs are validated and the error returned through the
component's contract — typically a command response of
`VALIDATION_ERROR`, a sized return, or a soft-failure event. An
assert on an untrusted input is a denial-of-service vector; the
security-review agent (`security-review.agent.md`) flags this class
of finding as `ground-reachable-assert` /
`hardware-reachable-assert`. CPP-4 is the C++-side mirror of that
finding class.

### C. F Prime types and idioms

#### CPP-3 — Prefer configurable `Fw*` types

For sizes, status codes, time, and other framework concerns, use the
F Prime configurable types (`U32`, `FwSizeType`, `Fw::CmdResponse`,
`Fw::Time`, etc.) rather than bare C/C++ types. The configurable
types let the framework retarget bit widths for a platform without
diff-touching every component.

#### CPP-21 — No C-style arrays in interfaces; pair array + length in a struct

`void f(U8* buf, U32 len);` is brittle (lose `len`, lose memory
safety). Use `Fw::Buffer`, an `Fw::ByteArray`-style struct, or an
F Prime container type (CPP-22) that bundles the data and its length
together.

#### CPP-22 — Prefer `Fw/DataStructures` containers

For F Prime arrays, queues, ring buffers, and so on, use the
container types under `Fw/DataStructures/` rather than rolling a
bespoke C array or a `std::*` container (which violates CPP-25). The
F Prime containers are bounded, deterministic, and have known
allocation behavior.

#### CPP-23 — Use FPP modeled types on ground-facing interfaces

Commands, events, parameters, and telemetry channels — anything the
ground operator sees — are declared in `.fpp` files and autocoded into
strongly-typed C++ bindings. Use those autocoded types directly; do
not hand-roll C structs and serialize them yourself. The FPP
modeled types are the ground-system contract.

#### CPP-24 — Prefer `Fw::String` over `char*`

`Fw::String` is a fixed-size, bounded string type with explicit
length tracking. Prefer it for all string-bearing members and
parameters. `char*` is acceptable only for:

- String literals in source (e.g., `static const char* TAG = "MyComp";`).
- External API boundaries (OSAL calls, POSIX adapters) that take
  `char*` and where `Fw::String::toChar()` is used at the boundary.

### D. C++ language subset

#### CPP-5 — Code compiles cleanly as C++14

F Prime targets C++14. Newer features (C++17 `if constexpr`, C++20
ranges, concepts, modules) are not portable to every supported
toolchain. Verify on at least the project's reference toolchain.

#### CPP-6 — `nullptr` only

`NULL` and `0` are not null-pointer constants in F Prime code. Use
`nullptr` so the type system disambiguates pointer vs. integer.

#### CPP-7 — No lambdas; keep templates simple

Lambdas are forbidden in F Prime code: they implicitly synthesize
classes (CPP-25 concerns about RTTI and STL machinery), they can
allocate on capture (CPP-1), and they obscure ownership of captured
state. Use a named functor or a free function.

Templates are allowed, but **keep them simple**: function templates
with one or two type parameters, or simple class templates with
straightforward instantiations. Complex template machinery — heavy
meta-programming, substitution-failure-based dispatch, and deep
parameter-pack manipulation — is not acceptable in F Prime flight
code.

#### CPP-8 — Prefer typed constants over `#define`

```cpp
// Avoid
#define MAX_RETRIES 5

// Prefer
static constexpr U32 MAX_RETRIES = 5;
```

`#define` constants have no type, no scope, no debugger visibility,
and their macro-expansion can mis-paste. CPP-11 covers the
`constexpr` / `const` preference. Complex macros (multi-statement,
token-pasting, stringification beyond simple logging) need
justification.

#### CPP-9 — No C-style or function-style casts

```cpp
// Avoid
U32 x = (U32) y;
U32 z = U32(y);

// Prefer
U32 x = static_cast<U32>(y);
```

C-style casts silently change category (static / const / reinterpret /
dynamic). Spell out the cast category so the reviewer sees what's
happening.

#### CPP-10 — Avoid `reinterpret_cast` and `const_cast`

`reinterpret_cast` and `const_cast` are escape hatches. Their use
should be rare and justified inline with a `//` comment explaining
why (typically: a vendor API requires a non-const pointer, or a
hardware register access requires reinterpreting a buffer). A
`reinterpret_cast` without a justification comment is a finding.

#### CPP-11 — Prefer `constexpr`, then `const`

A value that the compiler can evaluate at compile time is
`constexpr`. A value that doesn't change after initialization is
`const`. Mutable storage is the last resort.

#### CPP-12 — No `using namespace`

```cpp
// Avoid (in header or implementation)
using namespace Os;

// Acceptable (in implementation, in narrow scope)
void MyComp_handler::doWork() {
    using Os::File;
    File f("/tmp/x");
    ...
}
```

`using namespace` at file scope pollutes the lookup space. A `using`
declaration inside a function body is acceptable.

#### CPP-13 — Prefer references over pointers

References are non-null by construction, can't be rebound, and read
better in signatures. Use a pointer only when null is a valid value
(an optional argument) or when the lifetime semantics genuinely
require it.

#### CPP-14 — Avoid multiple inheritance (except pure virtual interfaces)

Diamond inheritance, mix-in classes with state, and similar patterns
are not used in F Prime. The only acceptable multiple inheritance
shape is inheriting from multiple **pure-virtual interface** classes
(no state, no implementation in the interface).

#### CPP-15 — Mark overrides with `override`; only override virtual functions

```cpp
class Derived : public Base {
public:
    void doThing() override;       // signal intent + let compiler check
};
```

The `override` keyword catches signature drift across the
inheritance hierarchy. Don't accidentally introduce a new function
that looks like an override but isn't.

#### CPP-16 — `friend` only for unit-test access

`friend` declarations are confined to letting a unit-test fixture
reach private members of the class under test. Productive code does
not use `friend` for inter-component coupling.

#### CPP-18 — `explicit` constructors; call base constructors explicitly

```cpp
class MyComp : public MyCompComponentBase {
public:
    explicit MyComp(const char* compName)
      : MyCompComponentBase(compName)        // explicit base call
    {}
};
```

`explicit` on single-argument constructors prevents implicit
conversions that hide intent. Base constructors are called by name
in the member-initializer list, not relied on to default-construct.

#### CPP-25 — No exceptions, RTTI, STL, `std::string`

F Prime flight code is compiled with exceptions and RTTI disabled
(`-fno-exceptions` and the corresponding no-RTTI flag) and
deliberately avoids the STL.

- `try` / `catch` / `throw`: forbidden.
- `dynamic_cast` and `typeid` (which require RTTI): forbidden.
- `std::vector`, `std::map`, `std::list`, etc.: forbidden. Use
  `Fw/DataStructures` (CPP-22).
- `std::string`: forbidden. Use `Fw::String` (CPP-24).
- Other implicitly-allocating STL components: forbidden.

Trivial `<algorithm>` helpers (`std::min`, `std::max`,
`std::numeric_limits`) and `<cstdint>` types are acceptable. When in
doubt, prefer the F Prime type or a hand-rolled minimal helper over
pulling STL in.

### E. External authoritative references

#### CPP-26 — F Prime style guidelines

The project-wide F Prime style guidelines are the wiki page linked
below in §5. Where this skill and the wiki agree, this skill
shorthands the rule; where the wiki has more detail (naming,
formatting, header guards, include ordering), use the wiki.

#### CPP-27 — JPL C coding standard

For C-language considerations that bleed into C++ (loops with
bounded iteration counts, side-effect-in-condition, the "rule of one
return" debate), the JPL C coding standard linked in §5 is the
authoritative reference. F Prime adopts it where applicable to C++.

---

## 3. Reviewer finding-class mapping

When a reviewer agent flags a violation of one of the rules above,
it uses the following finding-class names. Other agents (security,
supply-chain, design, test-quality, stale-documentation) reference
these names when their work overlaps the C/C++ design surface.

| Rule | Finding-class | Notes |
|---|---|---|
| CPP-1  | `cpp-dynamic-memory-post-init`    | |
| CPP-2  | `cpp-fw-buffer-ownership`         | Branch-coverage; closely related to memory-safety findings flagged by `security-review` as `general-vulnerability/use-after-free`. |
| CPP-3  | `cpp-bare-type-where-fw-type-exists` | |
| CPP-4  | `cpp-fw-assert-on-untrusted-input` | Mirror of `security-review`'s `ground-reachable-assert` / `hardware-reachable-assert`. Either agent may flag; security-review carries the DoS framing, this skill carries the C++ idiom framing. |
| CPP-5  | `cpp-cxx14-violation`             | |
| CPP-6  | `cpp-null-vs-nullptr`             | |
| CPP-7  | `cpp-lambda` / `cpp-template-complexity` | Two sub-classes — one for any lambda, one for non-simple templates. |
| CPP-8  | `cpp-define-instead-of-constexpr` | |
| CPP-9  | `cpp-c-style-cast`                | |
| CPP-10 | `cpp-reinterpret-or-const-cast-unjustified` | |
| CPP-11 | `cpp-missing-constexpr-or-const`  | |
| CPP-12 | `cpp-using-namespace-file-scope`  | |
| CPP-13 | `cpp-pointer-where-reference-fits` | |
| CPP-14 | `cpp-multiple-inheritance-with-state` | |
| CPP-15 | `cpp-missing-override`            | |
| CPP-16 | `cpp-friend-in-production`        | |
| CPP-17 | `cpp-rule-of-three-or-five`       | |
| CPP-18 | `cpp-non-explicit-ctor` / `cpp-implicit-base-ctor` | Two sub-classes. |
| CPP-19 | `cpp-uninitialized-variable`      | |
| CPP-20 | `cpp-non-virtual-public-dtor`     | |
| CPP-21 | `cpp-c-style-array-in-interface`  | |
| CPP-22 | `cpp-bare-container-not-fw-data-structure` | |
| CPP-23 | `cpp-non-fpp-modeled-ground-interface` | |
| CPP-24 | `cpp-char-pointer-where-fw-string-fits` | |
| CPP-25 | `cpp-banned-cxx-feature` (suffix with the specific feature, e.g., `/exceptions`, `/RTTI`, `/STL`, `/std-string`) | |
| CPP-26 | `cpp-style-guide-violation`       | Catch-all for style-guide hits not covered above; cite the wiki section. |
| CPP-27 | `cpp-jpl-c-standard-violation`    | Catch-all for JPL C standard hits; cite the section. |

Finding-class names are stable strings. They appear in the inline
comment HTML footer (`finding-key` hash inputs) and in agent
documentation. Renaming a finding-class is a breaking change to the
re-review state mechanism (`_shared/skills/re-review-state.skill.md`)
because the hash changes — coordinate any rename across all agents
that reference it.

---

## 4. Triage hints for reviewer use

The triage classifier in `_shared/skills/triage-classifier.skill.md`
is authoritative; this section narrows the decision per rule
cluster so the code-review agent doesn't have to re-derive severity
from scratch for every finding.

### Cluster: Memory & lifetime (CPP-1, CPP-2, CPP-17, CPP-19, CPP-20)

- Default: `**must fix**`. These rules guard memory safety and
  deterministic behavior; a violation can manifest as a flight
  defect.
- Downgrade to `**suggestion**` only when the violation is in
  obviously test-only code (no flight-path reach) AND the violation
  is easily expressible as a one-line diff.
- Never `**could fix**`.

### Cluster: Asserts on untrusted inputs (CPP-4)

- Always `**must fix**`. CPP-4 mirrors the security-review
  ground/hardware-reachable-assert finding; both agents may flag and
  that is acceptable. The C++ comment carries the C++ idiom
  framing; the security comment carries the DoS framing.

### Cluster: F Prime type idioms (CPP-3, CPP-21, CPP-22, CPP-23, CPP-24)

- Default: `**suggestion**` with a fenced suggestion block that
  substitutes the correct F Prime type.
- Upgrade to `**must fix**` when the violation is on a ground-
  facing interface (CPP-23) — an autocoded FPP type is the
  ground-system contract and substituting a bare struct breaks it.

### Cluster: Language subset (CPP-5 through CPP-16, CPP-18, CPP-25)

- Default: `**suggestion**` or `**could fix**` per the severity
  ladder in `triage-classifier.skill.md`.
- Upgrade to `**must fix**` when the violation is one of:
  - CPP-25 introducing an exception or RTTI dependency (changes the
    build configuration).
  - CPP-10 introducing an unjustified `reinterpret_cast` /
    `const_cast` on a flight path.
  - CPP-14 introducing multiple inheritance with state.

### Cluster: External references (CPP-26, CPP-27)

- Default: `**could fix**` for cosmetic style hits; `**suggestion**`
  when a concrete fix is expressible. The external references are
  authoritative for the specific clause — cite the section.

### Preexisting violations

Any rule violation that the PR did not introduce or widen (per
`_shared/skills/pr-diff-scoping.skill.md`) is `**future work**`,
never `**must fix**`. This applies uniformly across all CPP rules.

---

## 5. External references

- F Prime style guidelines:
  <https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines>
- JPL C coding standard:
  <https://yurichev.com/mirrors/C/JPL_Coding_Standard_C.pdf>
- F Prime user manual (memory management, ground interface,
  state machines): `docs/user-manual/framework/`.
- F Prime data-structure types: `Fw/DataStructures/`.

When the agent cites one of these, it links to the section / file
rather than restating the source verbatim — the goal is to keep this
skill compact and let the source documents be the long form.

---

## 6. One-line summary

`F Prime flight C++ is a tight C++14 subset: no dynamic allocation
post-init, no exceptions / RTTI / STL, prefer F Prime types
(Fw::Buffer, Fw::String, Fw/DataStructures), prefer references and
constexpr, no lambdas, no using-namespace, FW_ASSERT only for
programming errors. The rules in §2 are the long form; the
finding-classes in §3 are the reviewer's vocabulary; the triage
hints in §4 narrow severity per cluster.`
