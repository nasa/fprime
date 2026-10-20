---
name: fprime-requirements-review
description: >-
  Adversarial review of an F Prime requirements set before design or
  code exists. Checks each requirement for objective verifiability,
  clarity, rationale, and the strongest feasible verification method
  (unit test, then integration/system test, then inspection), and
  checks the set for breadth of the request and fault/failure
  coverage. Trigger when a requirements table is drafted and needs
  review, or when reviewing a PR that adds or changes requirements.
  Keywords: F Prime, requirements, review, verifiable, rationale,
  verification method, fault coverage, shall statement.
---

# Skill: F Prime Requirements Review

A requirement is the contract everything downstream is verified
against (`fprime-component-requirements`). This skill is the review
side of that contract: it asks of every requirement **can it be
objectively verified?** and of the set **does it cover the request?**
It is adversarial — look for the requirement that would be "met" by a
wrong component and the request element no requirement covers.

Inputs: the requirements table, the request it serves (issue, human
goals, parent requirements), and the SDD if one exists. Review the
requirements against the request, never against an implementation.

---

## 1. Per-requirement checks

Every requirement is one row with ID, shall statement, rationale and
verification method (§4 table). Check each row for all four:

#### RQ-1 — Objectively verifiable

One behaviour per statement, with an observable outcome a test or an
inspector can decide without judgement. Numbers carry units and
bounds; "appropriately", "quickly", "robust", "as needed", "handle"
are not verifiable. Two outcomes joined by "and" are two requirements.

**Finding-class:** `req-not-verifiable`.

#### RQ-2 — Easily understood

One sentence, active voice, `shall`, defined terms only, readable by
someone who has not seen the design. No design or implementation
content (port kinds, FPP constructs, data structures, algorithms) —
those are design decisions (`fprime-component-requirements` §3).

**Finding-class:** `req-unclear` / `req-contains-design`.

#### RQ-3 — Rationale

States *why*: the request element, parent requirement, fault scenario
or constraint that motivates it. A restatement of the shall statement
is not a rationale. A requirement with no rationale is a candidate for
deletion, not for keeping.

**Finding-class:** `req-no-rationale`.

#### RQ-4 — Verification method, strongest feasible

Each requirement names exactly one of, in order of preference:

| Method | Use when |
|---|---|
| **Unit test** | The behaviour is observable at the component boundary (ports, commands, events, telemetry, parameters) with a `TesterBase` harness. Default. |
| **Integration test** (system test) | The behaviour needs more than one component, a deployment, real timing, or real hardware to observe. |
| **Inspection** | The behaviour is not observable at run time (coding constraints, documentation, resource budgets fixed at build time). Last resort. |

A weaker method than the behaviour allows is a finding ("inspection"
on a behaviour a unit test could observe; "integration test" on a
single-component behaviour). A missing method, or "TBD", is a finding.
The method must be able to decide RQ-1's outcome — if it cannot, the
requirement is not verifiable as written.

**Finding-class:** `req-no-verification` / `req-weak-verification`.

---

## 2. Set-level checks

#### RS-1 — Breadth of the request

List every element of the request — each goal sentence, each named
command, telemetry item, event, parameter, interface, mode, and
constraint — and map each to the requirement(s) that cover it (§4
trace table). An element with no requirement is a gap unless the
request explicitly excludes it.

**Finding-class:** `req-set-gap`.

#### RS-2 — Fault and failure scenarios

For each interface, input, resource and external dependency the
request implies, ask what happens on: invalid or out-of-range input;
no response or timeout; error return from a driver or OS call; full
queue or buffer; unexpected state at startup or after reset; loss of
the thing it depends on. Each scenario has a requirement, or an
explicit "not applicable — <reason>" line in the set. Silence is a
finding.

**Finding-class:** `req-set-no-fault-coverage`.

#### RS-3 — Consistency

Unique IDs, no two requirements that can both be met only by
contradicting one, no duplicates with different wording, consistent
terms for the same thing.

**Finding-class:** `req-set-inconsistent`.

---

## 3. Tags

Use the review contract's tags (`_shared/review-contract.md` §tags),
with a concrete rewrite for every finding:

- `**must fix**` — the requirement cannot be verified as written
  (RQ-1, RQ-4 missing), contains design that forecloses the design
  phase (RQ-2), or the set leaves a request element or a fault
  scenario with neither a requirement nor a waiver (RS-1, RS-2), or
  contradicts itself (RS-3).
- `**could fix**` — weaker-than-feasible verification method, missing
  rationale where the motivation is evident, wording that is clear
  but not crisp, duplicate wording.

Do not downgrade a must-fix because the author "will cover it in
testing": what is not a requirement is not verified.

---

## 4. Output

`review-req-round-N.md`:

```md
## Per-requirement
| ID | RQ-1 verifiable | RQ-2 understood | RQ-3 rationale | RQ-4 method | Finding (tag, class, rewrite) |

## Request trace (RS-1)
| Request element (quoted) | REQ-IDs | Gap? |

## Fault scenarios (RS-2)
| Interface / input / resource | Scenario | REQ-ID or waiver | Gap? |

## Consistency (RS-3)
...

## Totals
must fix: N   could fix: N

## Disagreement log
| Round | Finding | Author position | Reviewer position | Status |
```

Three rounds maximum; on the fourth, hand the log to the human.

The requirements table under review is expected in the
`fprime-component-requirements` form plus a rationale column:

| ID | Shall Statement | Rationale | Verification Method |
|---|---|---|---|

---

## Anti-patterns

- Reviewing against the code or FPP instead of the request.
- Rewriting the requirements yourself instead of proposing a rewrite
  in the finding.
- Accepting "inspection" because no test exists yet — the method
  describes what *can* verify it, not what has been written.
- Inferring request elements the request does not state; the gap list
  is only what the request says.
