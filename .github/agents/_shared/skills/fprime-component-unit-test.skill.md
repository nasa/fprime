---
name: fprime-component-unit-test
description: Unit testing phase of F Prime component development. Provides lifecycle context, goals, and decision criteria for the unit testing phase. Defers to fprime-unit-testing.skill.md for implementation details. Trigger when implementation is ready for testing or when following TDD (tests before implementation). Keywords: F Prime, unit test, GTest, TesterBase, GTestBase, Tester, fprime-util check, STest, rule-based testing.
---

# Skill: F Prime Component Unit Testing

Unit tests verify each component requirement in isolation using the
autocoded test harness. This skill provides lifecycle context and
goals; for scaffolding, assertion macros, code patterns, and the full
implementation procedure, see `fprime-unit-testing.skill.md` and the
[unit testing guide](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md).

---

## Goals

- **Verify all requirements** with unit tests if possible.
- **Verify all interfaces**: ports, events, telemetry, commands.
- **Prefer rule-based testing** where possible — it provides broader
  coverage than traditional tests. See
  `fprime-unit-testing.skill.md` §3 for criteria and constructs, and
  the [rules-based testing guide](https://github.com/nasa/fprime/blob/devel/docs/how-to/rule-based-testing.md)
  for the full walkthrough.
- **Target 95% code coverage.**
- You should be able to do most of this without user input, since
  requirements and the FPP model are already defined. However, **ask
  the user for help** with predicting edge cases you may have missed
  or for clarification on expected behavior.

---

## Process

Follow `fprime-unit-testing.skill.md` for the step-by-step procedure:
scaffold → implement → register → run. That skill covers Tester
class structure, TestMain layout, CMakeLists registration, assertion
macros, helper functions, and rules-based testing.

Key decisions within the lifecycle:

1. **Traditional vs. rule-based**: Use the criteria table in
   `fprime-unit-testing.skill.md` §3. Default to rule-based for
   components with multiple ports or stateful behavior.
2. **TDD**: If following test-driven development (tests before
   implementation), see
   [TDD guide](https://github.com/nasa/fprime/blob/devel/docs/how-to/test-driven-development.md).
   The FPP model is sufficient to write tests — implementation is not
   required.
3. **Coverage**: Run `fprime-util check --coverage` and review
   `*_gcov.txt`. Target 95%.

---

## Anti-Patterns

- Testing only happy paths — off-nominal coverage is critical
- Forgetting `doDispatch()` for async commands/ports
- Asserting only on count (`SIZE`) without checking values
- Modifying component internals directly (test through the
  interface only)
- Guessing expected values — derive from requirements
