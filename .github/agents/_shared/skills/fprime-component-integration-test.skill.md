---
name: fprime-component-integration-test
description: Integration testing phase of F Prime component development. Guides the agent through writing pytest-based integration tests using the GDS Integration Test API against a running deployment. Trigger when unit tests pass and the component is integrated into a topology. Keywords: F Prime, integration test, pytest, GDS, fprime_test_api, system test, deployment, topology.
---

# Skill: F Prime Component Integration Testing

Integration tests verify that components work correctly **together** in
a running deployment. They exercise the system through the Ground Data
System (GDS) — sending commands, checking events, and reading telemetry
over the actual communication stack.

---

## STOP — Prerequisites and Questions

Before writing integration tests, **ask the user**:

1. Has the component been added to the topology? (If not, it must be
   integrated first — see `docs/user-manual/overview/development-practice.md`
   § "Assemble Topology".)
2. Which **system-level requirements** should integration tests verify?
   (These are higher-level than unit test requirements.)
3. What is the deployment name and dictionary location?
4. Does the deployment need to be running for these tests? (Usually
   yes — via `fprime-gds`.)
5. Are there any timing constraints or delays to account for?
6. Should we use the **reusable integration test** framework (for
   standard Svc components) or write **custom tests**?
7. What instance names are used in the topology for this component?

**Do not guess at instance mnemonics, opcode names, or channel
names.** Look them up in the topology FPP or ask the user.

---

## Step-by-Step Process

### Step 1 — Set Up Test Directory

Create the integration test directory in your deployment:

```
<Deployment>/
└── test/
    └── int/
        ├── test_<deployment>.py
        └── int_config.json     (if using reusable tests)
```

### Step 2 — Write a Basic Integration Test

Integration tests use `pytest` with the `fprime_test_api` fixture:

```python
def test_my_component_nominal(fprime_test_api):
    """Verify <ComponentInstance> responds to <Command>.

    Covers: REQ-<Component>-001
    """
    fprime_test_api.send_and_assert_command(
        "<instanceName>.<COMMAND_MNEMONIC>",
        args=["arg1_value", "arg2_value"],
        max_delay=5,
    )
```

### Step 3 — Assert on Events

```python
def test_my_component_reports_event(fprime_test_api):
    """Verify <ComponentInstance> emits <EventName> on <trigger>.

    Covers: REQ-<Component>-003
    """
    # Send the triggering command
    fprime_test_api.send_command("<instanceName>.<COMMAND>", ["arg"])

    # Wait for and assert the expected event
    fprime_test_api.assert_event(
        "<instanceName>.EventName",
        args=[expected_arg],
        start="NOW",
        timeout=5,
    )
```

### Step 4 — Assert on Telemetry

```python
def test_my_component_telemetry(fprime_test_api):
    """Verify <ComponentInstance> updates <Channel> after <action>.

    Covers: REQ-<Component>-005
    """
    fprime_test_api.send_and_assert_command(
        "<instanceName>.<COMMAND>",
        max_delay=5,
    )

    result = fprime_test_api.assert_telemetry(
        "<instanceName>.ChannelName",
        value=expected_value,
        start="NOW",
        timeout=5,
    )
```

### Step 5 — Test Sequences

For workflows that involve multiple commands or time-ordered events:

```python
def test_my_component_sequence(fprime_test_api):
    """Verify <ComponentInstance> handles a full operational cycle.

    Covers: REQ-<Component>-010
    """
    # Step 1: Initialize
    fprime_test_api.send_and_assert_command(
        "<instanceName>.INIT", max_delay=5
    )

    # Step 2: Trigger operation
    fprime_test_api.send_and_assert_command(
        "<instanceName>.START", max_delay=5
    )

    # Step 3: Verify telemetry sequence
    ch_seq = [
        fprime_test_api.get_telemetry_pred("<instanceName>.Status", "RUNNING"),
        fprime_test_api.get_telemetry_pred("<instanceName>.Progress", 100),
    ]
    fprime_test_api.assert_telemetry_sequence(ch_seq, timeout=30)
```

### Step 6 — Reusable Tests (for Standard Components)

If the component is a standard F Prime service (`Svc/*`), use the
reusable test framework:

1. Create `int_config.json` mapping qualified names to instance
   mnemonics:

```json
{
    "Svc.CommandDispatcher": "CdhCore.cmdDisp",
    "Svc.FileDownlink": "FileHandling.fileDownlink"
}
```

2. Run existing tests against your deployment:

```bash
pytest ./lib/fprime/Svc/CmdDispatcher/test/int \
       --dictionary <path-to-dictionary> \
       --deployment-config ./test/int/int_config.json
```

**Ask the user** for the correct instance names for their topology.

### Step 7 — Run Integration Tests

```bash
# Start the deployment + GDS (in separate terminal)
fprime-gds --dictionary <path-to-dictionary>

# Run tests
pytest <Deployment>/test/int/ \
       --dictionary <path-to-dictionary>
```

### Step 8 — Verify Against System Requirements

Map each test to a system-level requirement. Document coverage in test
docstrings (`Covers: REQ-*`).

**Ask the user** if coverage is sufficient or if additional scenarios
are needed.

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
    test-working-directory: "<Deployment>/test/int"
    binary: "build-artifacts/*/<Deployment>/bin/<Deployment>"
    gds-args: "--dictionary build-artifacts/*/<Deployment>/dict/<Dict>.json"
    pytest-args: "--deployment-config <path>/int_config.json"
```

---

## Anti-Patterns

- ❌ Guessing instance mnemonics — look up in topology or ask user
- ❌ Hardcoding timeout values without asking about system timing
- ❌ Testing unit-level behavior in integration tests (use unit tests
  for that)
- ❌ Writing tests that depend on test execution order
- ❌ Ignoring `max_delay` / `timeout` (tests will hang or be flaky)
- ❌ Not mapping tests to system-level requirements
- ❌ Running integration tests without a running deployment
