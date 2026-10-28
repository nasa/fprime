---
name: fprime-requirements-review
description: >-
  Adversarial review of an F Prime requirements set before design or
  code exists, or when a human asks for one on a PR that changes
  requirements. Checks each requirement for objective verifiability,
  clarity, rationale, and the strongest feasible verification method
  (unit test, then integration/system test, then inspection), and
  checks the set for breadth of the request and fault/failure
  coverage. Keywords: F Prime, requirements, review, verifiable,
  rationale, verification method, fault coverage, shall statement.
---

# Skill: F Prime Requirements Review

A requirement is the contract everything downstream is verified
against (`fprime-component-requirements`). This skill is the review
side of that contract: it asks of every requirement **can it be
objectively verified?** and of the set **does it cover the request?**
It is adversarial — look for the requirement that a wrong component
would still satisfy, and the request element no requirement covers.

Inputs: the requirements table in the `fprime-component-requirements`
Step 2 form, the request it serves (issue, human goals, parent
requirements), and the SDD if one exists. Review against the request,
never against an implementation. The FPP model of an existing
component may be opened only to enumerate its interfaces for RS-2,
never to decide what a requirement means.

---

## 1. Per-requirement checks

#### RQ-1 — Objectively verifiable

From the row alone the reviewer can write three things: the stimulus
or condition, the observable a test or inspector reads (a named event,
channel, command response, return value, file state…), and the pass
criterion with units and bounds. If any of the three cannot be
written, the row fails. "Appropriately", "quickly", "robust", "as
needed", "handle", and undefined qualifiers such as "invalid" fail
by construction. Two independently testable outcomes are two
requirements; one pipeline (read, packetize, send) is one.

**Finding-class:** `req-not-verifiable`.

#### RQ-2 — Easily understood

One sentence, active voice, `shall`; terms defined in the request,
the SDD, or F Prime documentation; readable by someone who has not
seen the design. No design or implementation content (port kinds,
FPP constructs, data structures, algorithms) — those are design
decisions (`fprime-component-requirements` Step 3).

**Finding-class:** `req-unclear` (reader cannot state the behaviour,
or wording is clear but not crisp) / `req-contains-design`.

#### RQ-3 — Rationale

The rationale names an RS-1 request element, a parent REQ-ID, or an
RS-2 fault scenario. One that names none — empty, or a restatement of
the shall statement — fails. The reviewer never supplies a missing
rationale; a requirement nobody can justify is a deletion candidate.

**Finding-class:** `req-no-rationale`.

#### RQ-4 — Verification method, strongest feasible

Each row names a method; the first entry must be the strongest that
can observe RQ-1's outcome, further entries may follow:

| Method | Use when |
|---|---|
| **Unit test** | The outcome is observable at the component boundary alone (ports, commands, events, telemetry, parameters) with a component unit-test harness. |
| **Integration test** (system test) | The outcome needs more than one component, a deployment, real timing, or real hardware. |
| **Inspection** | The outcome is not observable at run time (coding constraints, documentation, build-time budgets). Last resort. |

Missing, "TBD", unqualified "Test", or a method that cannot observe the
outcome (unit test for a timing or cross-component behaviour; inspection
for a run-time behaviour) → `req-no-verification`: the requirement
stands, the method must change. A method that can observe the outcome
when a stronger one in the table also can ("integration test" for an
outcome observable at the component boundary alone) → `req-weak-
verification`. Inspection of a run-time behaviour is never "weak": it
cannot observe it and is `req-no-verification`. Absence of a written
test is never a reason to accept a weaker method.

---

## 2. Set-level checks

#### RS-1 — Breadth of the request

Before reading the rows, list every element of the request — each
goal sentence, named command, telemetry item, event, parameter,
interface, mode, constraint — then map each to the requirement(s)
that cover it. An element is covered only when a mapped requirement's
observable outcome **is** the element's outcome (the operator's
setpoint change takes effect, not "a setpoint parameter exists").
Naming the element is not coverage. An element with no covering
requirement is a gap unless the request explicitly excludes it. Only
elements the request states count here; do not infer others.

**Finding-class:** `req-set-gap`.

#### RS-2 — Fault and failure scenarios

For each interface, input, resource and external dependency named in
the request **or in any requirement's shall statement**, ask what
happens on: invalid or out-of-range input; input or command in a
mode/state that does not accept it; no response or timeout; error
return from a driver or OS call; full queue or buffer; re-entry or
concurrent invocation; unexpected state at startup or after reset;
loss of the dependency. Each scenario has a requirement, or a waiver
line in the set that names either the owner of the scenario — another
component with its REQ-ID, or a framework mechanism with the document
section that specifies it ("owned by `Svc::Health`, ping timeout,
`docs/sdd.md` §Ping") — or why it cannot arise, citing the request
constraint or parent requirement that guarantees it ("input is a
compile-time constant per REQ-SYS-012"); a design claim ("the driver
is synchronous") is not a reason before design exists. A waiver that
names neither ("out of scope", "handled elsewhere") is a gap.

**Finding-class:** `req-set-no-fault-coverage`.

#### RS-3 — Consistency

Unique IDs; no two requirements that cannot both be met; no
duplicates; one term per thing.

**Finding-class:** `req-set-inconsistent`.

---

## 3. Tags and scoping

Review-contract tags (`_shared/review-contract.md` §1), a concrete
rewrite with every finding:

| Class | Tag |
|---|---|
| `req-not-verifiable`, `req-no-verification`, `req-contains-design`, `req-unclear` (reader cannot state the behaviour), `req-no-rationale`, `req-set-gap`, `req-set-no-fault-coverage`, `req-set-inconsistent` (contradiction, duplicate ID) | `**must fix**` |
| `req-weak-verification`, `req-unclear` (clear but not crisp), `req-set-inconsistent` (duplicate wording, inconsistent terms) | `**could fix**` |

Never downgrade because "testing will catch it": what is not a
requirement is not verified.

When the input is a PR, apply `pr-diff-scoping`: rows the PR adds or
edits take the tags above; unchanged pre-existing rows that fail take
`**future work**`. RS-1..3 run over the whole set, but a gap is must
fix only when the PR's request introduces the element or dependency;
otherwise `**future work**`.

---

## 4. Output

`review-req-round-N.md`. Per-requirement cells read `pass` or
`fail → <key>`; RS-1/RS-2 `Covered?` cells read `pass`, `pass (waived)`,
or `fail → <key>`. Keys are stable across rounds:
RQ `<class>:<REQ-ID>`, RS-1 `<class>:"<quoted element>"`,
RS-2 `<class>:<interface>/<scenario>`, RS-3 `<class>:<REQ-ID>+<REQ-ID>`.

```md
## Per-requirement
| ID | RQ-1 | RQ-2 | RQ-3 | RQ-4 |

## Request trace (RS-1)
| Request element (quoted) | REQ-IDs | Covered? |

## Fault scenarios (RS-2)
| Interface / input / resource | Scenario | REQ-ID or waiver | Covered? |

## Consistency (RS-3)

## Findings
| Key | Tag | Class | Row / element | Finding | Rewrite |

## Totals
must fix: N   could fix: N   open disagreements: N

## Disagreement log
| Round | Key | Author position | Reviewer position | Status |
```

Rounds: re-run RQ-1..4 on changed rows only; re-run RS-1..3 on the
whole set (a deleted or rewritten row can open a gap elsewhere). A
finding closes only when the reviewer verifies the rewritten row
passes the check that raised it; "won't fix" goes to the disagreement
log (the file-based analogue of contract §11). Three rounds by
default; a `review rounds: N` given at kickoff overrides. Then hand
the log to the human.

---

## Anti-patterns

- Reviewing against the code or FPP instead of the request.
- Rewriting the requirements yourself instead of proposing a rewrite
  in the finding.
- Accepting "inspection" because no test exists yet — the method
  describes what *can* verify it, not what has been written.
- Inferring request elements the request does not state (RS-1); RS-2
  enumerates from the requirements as well.
- Reviewing a table you drafted in the same session: the anti-anchoring
  order of RS-1 needs a reviewer who has not seen the rows.
