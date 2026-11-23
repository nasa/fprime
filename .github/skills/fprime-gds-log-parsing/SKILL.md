---
name: fprime-gds-log-parsing
description: Locate and parse F Prime GDS logs (event.log, channel.log, command.log, flight software console log) - file map, line formats, and grep recipes. Use when diagnosing a GDS run or answering a question from a `logs/` directory.
---

# Skill: Parsing F Prime GDS logs

## 1. Where logs are

Each GDS tool invocation logs to a run directory
`<logs>/<tool>-<YYYY_MM_DD-HH_MM_SS>/`, where `<tool>` is `fprime-gds`,
`fprime-cli`, or `pytest` (integration tests) unless `--log-prefix`
changes it. `<logs>` is `-l/--logs`, default `./logs` under the
directory the tool was started from.
Invocations started in the same second share a directory and append to
the same files; with `--log-directly` files go into `<logs>` itself.
Newest GDS session: `ls -dt logs/fprime-gds-*/ | head -1`.

**Logs are per client, not per session.** Each directory holds only
what that process sent and what it received while connected. A command
sent with `fprime-cli` or by a test is in *that* tool's `command.log`,
not in `fprime-gds-*/command.log` (which records GUI-sent commands
only). For events and telemetry read the longest-connected client
(normally `fprime-gds-*`); for commands read every sibling
`command.log`. Nothing names the session a client belonged to: it is
the newest `fprime-gds-*` with an `event.log` started before it (one
holding only `fprime-gds.log` is a failed launch).

## 2. File map

| File | Content | Format |
|---|---|---|
| `event.log` | Events received and decoded | CSV, §3 |
| `channel.log` | Telemetry channel updates received | CSV, §3 |
| `command.log` | Commands sent by this client | CSV, §3 |
| `<App>.log` (e.g. `Ref.log`) | stdout/stderr of the flight software the GDS launched: console event echo, asserts, OS warnings. Only in `fprime-gds-*`, absent with `-n` | text, §4 |
| `recv.bin`, `sent.bin` | Packets received; `sent.bin` is usually empty, so it does not tell whether a command was sent | binary, do not grep |
| `fprime-gds.log`, `comm.py.log`, `__main__.py.log`, `fprime-cli.log`, `pytest.log`, `<plugin>/-c.log` | Python logging of each GDS process (launcher, comm adapter, GUI server, CLI, test, plugins). Usually start-up INFO lines only | `[date time,ms] [LEVEL] logger: msg` |
| `<destination file>.log` | Packet trace of one file downlink (`fprime-gds-*` only, copy under `<plugin>/`) | text |
| `bin/`, `dict/` | Copy of the deployment and dictionary under test (`pytest-*` only) | — |

## 3. CSV line formats

No header row. Fields are comma-separated and **not quoted**. The last
field (message, value, or argument list) can contain commas, so take it
with `cut -f6-` (events) or `-f5-` (channels, commands).

```text
event.log    time,raw_time,Component.instance.EventName,event_id,EventSeverity.<SEV>,message
channel.log  time,raw_time,Component.instance.ChannelName,channel_id,value
command.log  time,raw_time,Component.instance.COMMAND,opcode,[arg, ...]
```

```text
2026-11-23T21:44:43.610893,(2(0)-1795470283:610893),FileHandling.fileManager.FileRemoveError,83894275,EventSeverity.WARNING_HI,Could not remove file /nonexistent/file.txt, returned status 8
2026-11-23T21:44:20.089989,(2(0)-1795470260:89989),Ref.rateGroup2Comp.RgMaxTime,268443648,217 us
2026-11-23T21:44:43.599225,(2(0)-1795470283:599225),FileHandling.fileManager.RemoveFile,83894275,['/nonexistent/file.txt', False]
```

- `time`: ISO local time of the GDS host, no zone suffix; sorts
  lexically. For events and channels it is the **flight** timestamp, for
  commands the ground send time.
- `raw_time`: `(time_base(context)-seconds:microseconds)`.
- `<SEV>`: `COMMAND`, `ACTIVITY_LO`, `ACTIVITY_HI`, `WARNING_LO`,
  `WARNING_HI`, `FATAL`, `DIAGNOSTIC`.
- IDs and opcodes are decimal. `cmdDisp` messages print opcodes in hex
  (`Opcode 0x5002003`), sequencer messages in decimal
  (`opcode 83894275`) with a numeric result (`error 4` is
  `EXECUTION_ERROR`, `error 3` is `FORMAT_ERROR`).
- A channel is logged each time the flight software sends it (about
  1 Hz, only if written since the last send): a single sample means it
  was written once, and the last sample can lag a crash.

## 4. Flight software console log (`<App>.log`)

```text
EVENT: (83894275) (2:1795470283,610893) WARNING_HI: (FileHandling.fileManager) FileRemoveError : Could not remove file /nonexistent/file.txt, returned status 8
```

`EVENT: (id) (time_base:seconds,microseconds) SEVERITY: (instance) EventName : message`.
It differs from `event.log`: epoch time only, instance and event name
are split, and enum arguments show their number (`FORMAT_ERROR (3)`).
The console echo bypasses the downlink, so it has every event except a
FATAL. An event here but not in `event.log` was emitted
before the client connected (boot events), filtered on board
(`SET_EVENT_FILTER`), or dropped (see the counter recipe in §5).
It also shows how the process ended. A normal stop is
`[WARNING] ReceiveTask failed to recv ...` then `Exiting...`. A crash
ends with `FATAL <decimal event id> handled.` and
`Exiting with abort signal`. An assert also prints
`Assert: "<file>:<line>" <args>` first and usually reaches `event.log`
as `fatalAdapter.AF_ASSERT_<n>`. A FATAL event prints only its ID and
may never reach `event.log`: look the ID up under events in the
dictionary (the same number can be an unrelated opcode).
Non-event lines: `grep -v '^EVENT:' Ref.log`. Two events can share one
line, so count with `grep -o 'EVENT: (' Ref.log | wc -l`.

## 5. Recipes

Run from inside a run directory.

```bash
# Anything non-nominal, and a severity histogram
grep -E 'EventSeverity\.(WARNING_LO|WARNING_HI|FATAL)' event.log
cut -d, -f5 event.log | sort | uniq -c

# Events of one component instance, compact (time, name, message)
grep ',Ref\.SG1\.' event.log | cut -d, -f1,3,6-

# Commands the dispatcher reported as failed. A crash instead leaves
# an OpCodeDispatched with no completion.
grep -E 'cmdDisp\.(OpCodeError|MalformedCommand|InvalidCommand|TooManyCommands|CommandDroppedQueueOverflow)' event.log

# Fate of one command: its events, then who sent it (hex -> decimal;
# the directory names the tool). ../* spans all sessions and misses
# --log-directly directories: add their path.
grep -E 'Opcode 0x5002003 ' event.log
grep -H ",$((0x5002003))," ../*/command.log

# One channel: time series (add `| tail -1` for the latest value)
grep ',Ref\.SG1\.Output,' channel.log | cut -d, -f1,5-

# On-board counters, last value: command errors, dropped events/commands.
# CommandsDispatched includes sequence commands; for an exact count use
# grep -c OpCodeDispatched event.log
awk -F, '/\.(CommandErrors|CommandsDispatched|CommandsDropped|EventsDropped),/{last[$3]=$5} END{for(c in last) print c, last[c]}' channel.log

# Merged timeline of commands (all clients, hex opcode) and events
{ awk -F, '{printf "%s,CMD 0x%x,%s,%s\n", $1, $4, $3, substr($0, index($0, ",[") + 1)}' ../*/command.log; cut -d, -f1,3,5- event.log; } | sort
```

An event name or ID alone does not tell you its arguments or severity
definition; look them up in the dictionary JSON
(`pytest-*/dict/`, else
`build-artifacts/<platform>/<App>/dict/*TopologyDictionary.json`).

## 6. Gotchas

- Empty `command.log` in `fprime-gds-*` is normal when commands came
  from the CLI or a test. Commands run by an on-board sequence appear in
  no `command.log`: follow `OpCodeDispatched` and the sequencer's
  `CS_CommandComplete` / `CS_CommandError` events. File uplinks are in
  no `command.log` either, only in `fileUplink` events.
- A component can refuse a command with a `WARNING_*` event and still
  report `OpCodeCompleted`; read the events from just before a
  command's dispatch (a handler's events can precede
  `OpCodeDispatched` by microseconds) to its completion.
- A dispatch with no `command.log` line came from a sequence, or from a
  client that logged elsewhere (`--log-directly`) or not at all
  (`--disable-data-logging`, which leaves only the tool's Python log).
  A command sent after the flight software died is still in
  `command.log`; only the missing `OpCodeDispatched` shows it.
- `event.log` and `channel.log` start when that client connected and
  can miss boot events; use `<App>.log` for those.
- A `channel.log` that stops growing means the flight software died or
  disconnected; the GDS process logs say nothing about it, and with
  `-n` (no `<App>.log`) the cause is not in the logs.
- A zero-byte `event.log` in a short `fprime-cli-*` run means the tool
  exited before anything arrived, not that nothing happened.
