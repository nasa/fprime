# F Prime Copilot Instructions

General repository guidance — layout, build and test commands, coding
conventions, and available skills — is in
[`AGENTS.md`](../AGENTS.md) at the repository root.

When reviewing a pull request, invoke the F Prime multi-agent PR
review flow at
[`.github/agents/review-orchestrator.agent.md`](agents/review-orchestrator.agent.md).

The orchestrator's behavior is governed by the shared review
contract
([`.github/agents/_shared/review-contract.md`](agents/_shared/review-contract.md))
and the agent registry
([`.github/agents/_shared/agent-registry.yml`](agents/_shared/agent-registry.yml)),
which lists the ten reviewer agents (security, supply chain / runner
safety, F Prime C/C++ design, stale documentation, design,
architecture, test quality, correctness, operational consequences,
maintainability) and the summary aggregator.

## Skills

Reusable procedures live in `.github/skills/<skill-name>/SKILL.md`,
following the [Agent Skills specification](https://agentskills.io/specification).
Agents and skills reference each other by skill name. To add or change
one, follow the `agent-skill-authoring` skill.

## Writing unit tests

When writing or modifying F Prime component unit tests, follow the
procedure in
[`.github/skills/fprime-unit-testing/SKILL.md`](skills/fprime-unit-testing/SKILL.md).
It covers scaffold generation (`fprime-util impl --ut`), the
Tester / GTestBase pattern, helper-function design, rules-based
testing with STest, and `CMakeLists.txt` registration.
