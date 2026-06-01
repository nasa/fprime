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

These tests are **reusable**: they ship with the component and can be
run against any deployment that integrates it, using a configuration
file to map qualified names to topology instance names.

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

## Step-by-Step Process

### Step 1 — Set Up Test Directory

Create the integration test directory in the **component's** folder:

```
MyComponent/
├── MyComponent.fpp
├── MyComponent.hpp
├── MyComponent.cpp
├── CMakeLists.txt
├── test/
│   ├── ut/           (unit tests)
│   └── int/
│       └── test_<ComponentName>.py
```

### Step 2 — Write Reusable Integration Tests

Integration tests use `pytest` with the `fprime_test_api` fixture.
Use `fprime_test_api.get_mnemonic()` to resolve instance names from
the configuration file, making tests portable across deployments:

```python
def test_my_component_nominal(fprime_test_api):
    """Verify component responds to command.

    Covers: REQ-<Component>-001
    """
    instance = fprime_test_api.get_mnemonic("Module.ComponentName")
    fprime_test_api.send_and_assert_command(
        f"{instance}.COMMAND_MNEMONIC",
        args=["arg1_value", "arg2_value"],
        max_delay=5,
    )
```

### Step 3 — Assert on Events

```python
def test_my_component_reports_event(fprime_test_api):
    """Verify component emits expected event on trigger.

    Covers: REQ-<Component>-003
    """
    instance = fprime_test_api.get_mnemonic("Module.ComponentName")

    # Send the triggering command
    fprime_test_api.send_command(f"{instance}.COMMAND", ["arg"])

    # Wait for and assert the expected event
    fprime_test_api.assert_event(
        f"{instance}.EventName",
        args=[expected_arg],
        start="NOW",
        timeout=5,
    )
```

### Step 4 — Assert on Telemetry

```python
def test_my_component_telemetry(fprime_test_api):
    """Verify component updates telemetry after action.

    Covers: REQ-<Component>-005
    """
    instance = fprime_test_api.get_mnemonic("Module.ComponentName")

    fprime_test_api.send_and_assert_command(
        f"{instance}.COMMAND",
        max_delay=5,
    )

    result = fprime_test_api.assert_telemetry(
        f"{instance}.ChannelName",
        value=expected_value,
        start="NOW",
        timeout=5,
    )
```

### Step 5 — Test Sequences

For workflows that involve multiple commands or time-ordered events:

```python
def test_my_component_sequence(fprime_test_api):
    """Verify component handles a full operational cycle.

    Covers: REQ-<Component>-010
    """
    instance = fprime_test_api.get_mnemonic("Module.ComponentName")

    # Step 1: Initialize
    fprime_test_api.send_and_assert_command(
        f"{instance}.INIT", max_delay=5
    )

    # Step 2: Trigger operation
    fprime_test_api.send_and_assert_command(
        f"{instance}.START", max_delay=5
    )

    # Step 3: Verify telemetry sequence
    ch_seq = [
        fprime_test_api.get_telemetry_pred(f"{instance}.Status", "RUNNING"),
        fprime_test_api.get_telemetry_pred(f"{instance}.Progress", 100),
    ]
    fprime_test_api.assert_telemetry_sequence(ch_seq, timeout=30)
```

### Step 6 — Draft Requirements Coverage

Draft which requirements should be covered by integration tests.
Aim for reasonable coverage of the component's requirements,
especially those that exercise inter-component behavior. Map each
test to a requirement in the docstring (`Covers: REQ-*`).

### Step 7 — Run Integration Tests

```bash
# Start the deployment + GDS (in separate terminal)
fprime-gds --dictionary <path-to-dictionary>

# Run the component's reusable tests against a deployment
pytest <Component>/test/int/ \
       --dictionary <path-to-dictionary> \
       --deployment-config <path>/int_config.json
```

The `int_config.json` maps qualified component names to topology
instance names:

```json
{
    "Module.ComponentName": "topologyInstance"
}
```

---

## Key API Methods

| Method | Purpose |
|---|---|
| `send_command(mnemonic, args)` | Send command (no assertion) |
| `send_and_assert_command(mnemonic, args, max_delay)` | Send + assert command events |
| `await_telemetry(channel, value, timeout)` | Wait for telemetry update |
| `assert_telemetry(channel, value, start, timeout)` | Assert telemetry exists |
| `assert_telemetry_count(n, channel)` | Assert count of updates |
| `assert_telemetry_sequence(predicates, timeout)` | Assert ordered sequence |
| `await_event(event, args, timeout)` | Wait for event |
| `assert_event(event, args, start, timeout)` | Assert event exists |
| `assert_event_count(n, event)` | Assert count of events |
| `get_telemetry_pred(channel, value)` | Create telemetry predicate |
| `get_event_pred(event, args)` | Create event predicate |
| `get_mnemonic(qualified_name)` | Resolve instance name from config |

---

## CI Integration

For automated CI, use the `fprime-actions/run-integration-tests` action:

```yaml
- name: "Integration Tests"
  uses: nasa/fprime-actions/run-integration-tests@devel
  with:
    test-working-directory: "<Component>/test/int"
    binary: "build-artifacts/*/<Deployment>/bin/<Deployment>"
    gds-args: "--dictionary build-artifacts/*/<Deployment>/dict/<Dict>.json"
    pytest-args: "--deployment-config <path>/int_config.json"
```

---

## Anti-Patterns

- Hardcoding instance mnemonics — use `get_mnemonic()` for portability
- Writing tests that depend on test execution order
- Ignoring `max_delay` / `timeout` (tests will hang or be flaky)
- Not mapping tests to requirements
- Testing unit-level behavior in integration tests (use unit tests
  for that)
