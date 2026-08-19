---
name: fprime-component-integration-test
description: Integration testing phase of F Prime component development. Guides the agent through writing reusable pytest-based integration tests in the component's test/int/ folder using the GDS Integration Test API. These tests ship with the component and can be run against any deployment that includes it. Trigger when unit tests pass and the component is ready for integration testing. Keywords: F Prime, integration test, pytest, GDS, fprime_test_api, reusable test, component test.
---

# Skill: F Prime Reusable Component Integration Testing

Reusable integration tests live in the **component's own
`test/int/` folder** and verify that the component works correctly
in a running deployment. They exercise the component through the
Ground Data System (GDS) — sending commands, checking events, and
reading telemetry over the actual communication stack.

For the full implementation guide, see
[Reusable Integration Tests](https://github.com/nasa/fprime/blob/devel/docs/user-manual/gds/reusable-integration-tests.md).
For the complete API reference (send commands, assert events/telemetry,
predicates, histories), see the
[GDS Integration Test API Guide](https://github.com/nasa/fprime/blob/devel/docs/user-manual/gds/gds-test-api-guide.md).

> **Note**: This skill covers **component-level** reusable integration
> tests. System-wide integration tests (testing cross-component
> workflows across an entire deployment) are a separate concern.

---

## Prerequisites

The component must be added to a topology before integration tests
can run. If it has not been integrated yet, see
`docs/user-manual/overview/development-practice.md` § "Assemble
Topology".

The deployment **must be running** (via `fprime-gds`) for integration
tests to execute.

---

## Process

### Step 1 — Set Up Test Directory

Create `test/int/test_<ComponentName>.py` in the component folder.
See
[Reusable Integration Tests](https://github.com/nasa/fprime/blob/devel/docs/user-manual/gds/reusable-integration-tests.md)
for the expected file layout and `int_config.json` configuration.

### Step 2 — Write Reusable Tests

Tests use `pytest` with the `fprime_test_api` fixture. Use
`fprime_test_api.get_mnemonic()` to resolve instance names from a
configuration file, making tests portable across deployments. See the
[GDS Test API Guide](https://github.com/nasa/fprime/blob/devel/docs/user-manual/gds/gds-test-api-guide.md)
for the full API (sending commands, asserting events/telemetry,
predicates, sequences).

Key patterns:

- `send_and_assert_command()` — send + verify command success events
- `assert_event()` / `assert_telemetry()` — verify component output
- `get_mnemonic()` — resolve qualified names for portability
- Map each test to a requirement in the docstring (`Covers: REQ-*`)

### Step 3 — Draft Requirements Coverage

Draft which requirements should be covered by integration tests.
Aim for reasonable coverage of the component's requirements,
especially those that exercise inter-component behavior.

### Step 4 — Run Integration Tests

```bash
# Start the deployment + GDS (in separate terminal)
fprime-gds --dictionary <path-to-dictionary>

# Run the component's reusable tests against a deployment
pytest <Component>/test/int/ \
       --dictionary <path-to-dictionary> \
       --deployment-config <path>/int_config.json
```

See [Reusable Integration Tests § Running the tests](https://github.com/nasa/fprime/blob/devel/docs/user-manual/gds/reusable-integration-tests.md#running-the-tests)
for CI integration using `fprime-actions/run-integration-tests`.

---

## Anti-Patterns

- Hardcoding instance mnemonics — use `get_mnemonic()` for portability
- Writing tests that depend on test execution order
- Ignoring `max_delay` / `timeout` (tests will hang or be flaky)
- Not mapping tests to requirements
- Testing unit-level behavior in integration tests (use unit tests
  for that)
