---
name: agent-skill-authoring
description: Use when adding or modifying rules, finding classes, or agent definitions in the F Prime multi-agent review system. Ensures changes are concise, consistent, and correctly wired across all files.
---

# Skill: Authoring concise agent and skill updates

Guidelines for adding or modifying rules, agents, and skills in the
F Prime multi-agent review system. The goal is minimal, consistent
changes that AI reviewers can interpret without verbose explanation.

---

## 1. Writing rules (fprime-cpp-design.skill.md and similar)

**Be terse.** The rule title carries most of the meaning. AI agents
are capable of inferring rationale from a well-named rule. Aim for
2-6 lines of prose per rule — comparable to CPP-6 or CPP-15, not
CPP-1 or CPP-25 (which are longer because they have genuinely
complex scoping).

Checklist for a new rule:

- **Title** (`#### CPP-N — <imperative phrase>`): self-explanatory
  in isolation. A reader should understand the rule from the title
  alone.
- **Body**: 2-6 lines. State what is required or forbidden, one
  exception if any, and a cross-reference to a related rule if
  relevant. Omit motivation the title already conveys.
- **Code examples**: include only when the correct pattern is
  non-obvious. One "avoid / prefer" pair, max 6 lines each. Skip
  examples when the rule is self-evident from the title (e.g.,
  "use `nullptr`", "mark overrides with `override`").
- **Finding-class row** (§2 table): one row, stable string name.
  Use the `cpp-<description>` naming convention.
- **Triage hint** (§3): one sub-bullet under the appropriate
  cluster. State default tag and upgrade/downgrade condition.

**Anti-patterns to avoid:**

- Restating the title in the body ("This rule requires X" when the
  title already says "Require X").
- Multi-paragraph rationale — put rationale in the PR description,
  not the rule text.
- Multiple code examples showing the same point.

## 2. Updating agent files (e.g., fprime-code-review.agent.md)

Agent files reference the skill as the source of truth. When adding
a rule to a skill, update the agent file with **minimal additions**:

- **Scope range**: bump "CPP-1 through CPP-N" to the new max.
- **Finding-class list**: add one line for the new finding class.
- **Heuristics**: append a short clause ("; <pattern> (CPP-N)") to
  the relevant scan-order item. Do not duplicate the rule body.
- **Triage rules of thumb**: append a short clause to the relevant
  cluster bullet. Mirror the skill's §3 hint verbatim or shorter.

**Do not** re-explain the rule in the agent file. The skill is the
source of truth; the agent file is a lookup index.

## 3. Updating the registry (agent-registry.yml)

Bump the scope range in `scope_summary` (e.g., "CPP-33" → "CPP-34").
No other changes needed for a new rule — the registry describes
agent scope, not individual rules.

## 4. Cross-file consistency checklist

Before committing a new rule, verify all references are updated:

| Location | What to update |
|---|---|
| `fprime-cpp-design.skill.md` §1 | Rule text |
| `fprime-cpp-design.skill.md` §2 | Finding-class row |
| `fprime-cpp-design.skill.md` §3 | Triage hint + cluster header |
| `fprime-code-review.agent.md` scope | "CPP-1 through CPP-N" (2 occurrences) |
| `fprime-code-review.agent.md` finding classes | New entry |
| `fprime-code-review.agent.md` heuristics | Clause in scan item |
| `fprime-code-review.agent.md` triage | Clause in cluster bullet |
| `agent-registry.yml` | `scope_summary` range |
