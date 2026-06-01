# F Prime Copilot Instructions

When reviewing a pull request, invoke the F Prime multi-agent PR
review flow at
[`.github/agents/review-orchestrator.agent.md`](agents/review-orchestrator.agent.md).

The orchestrator's behavior is governed by the shared review
contract
([`.github/agents/_shared/review-contract.md`](agents/_shared/review-contract.md))
and the agent registry
([`.github/agents/_shared/agent-registry.yml`](agents/_shared/agent-registry.yml)),
which lists the six reviewer agents (security, supply chain / runner
safety, F Prime C/C++ design, stale documentation, design, test
quality) and the summary aggregator.

## Writing unit tests

When writing or modifying F Prime component unit tests, follow the
procedure in
[`.github/agents/_shared/skills/fprime-unit-testing.skill.md`](agents/_shared/skills/fprime-unit-testing.skill.md).
It covers scaffold generation (`fprime-util impl --ut`), the
Tester / GTestBase pattern, helper-function design, rules-based
testing with STest, and `CMakeLists.txt` registration.
