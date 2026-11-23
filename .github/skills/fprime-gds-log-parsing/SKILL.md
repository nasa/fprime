---
name: fprime-gds-log-parsing
description: Find and parse the logs written by the F Prime GDS (fprime-gds, fprime-cli, GDS integration tests). Maps each log file to its content, gives the line format of event.log, channel.log, command.log and the flight software console log, and grep/awk recipes to answer "what went wrong", "did this command complete", and "what was this channel's value". Use when diagnosing a GDS run, a failed integration test, or any question answerable from a `logs/` directory. Keywords: F Prime, GDS, logs, event.log, channel.log, command.log, recv.bin, TestLog, grep, telemetry, EVR.
---

# Skill: Parsing F Prime GDS logs

## 1. Where logs are

Each GDS tool invocation logs to a run directory
`<logs>/<tool>-<YYYY_MM_DD-HH_MM_SS>/`, where `<tool>` is `fprime-gds`,
`fprime-cli`, or `pytest` (integration tests). `<logs>` is `-l/--logs`,
default `./logs` under the directory the tool was started from.
Invocations started in the same second share a directory and append to
the same files. Newest run: `ls -dt logs/*/ | head -1`.

**Logs are per client, not per session.** Each directory holds only
what that process sent and what it received while connected. A command
sent with `fprime-cli` or by a test is in *that* tool's `command.log`,
not in `fprime-gds-*/command.log` (which records GUI-sent commands
only). For events and telemetry read the longest-connected client
(normally `fprime-gds-*`); for commands read every sibling
`command.log`.

## 2. File map

| File | Content | Format |
|---|---|---|
| `event.log` | Events received and decoded | CSV, §3 |
| `channel.log` | Telemetry channel updates received | CSV, §3 |
| `command.log` | Commands sent by this client | CSV, §3 |
| `<App>.log` (e.g. `Ref.log`) | stdout/stderr of the flight software the GDS launched: console event echo, asserts, OS warnings. Only in `fprime-gds-*`, absent with `-n` | text, §4 |
| `recv.bin`, `sent.bin` | Raw downlinked bytes; `sent.bin` is usually empty, so it does not tell whether a command was sent | binary, do not grep |
| `fprime-gds.log`, `comm.py.log`, `__main__.py.log`, `fprime-cli.log`, `pytest.log`, `custom-data-handlers-app/` | Python logging of each GDS process (launcher, comm adapter, GUI server, CLI, test). Look here for connection and framing problems | `[date time,ms] [LEVEL] logger: msg` |
| `<filename>.log` | One per downlinked file | Python logging |
| `TestLog_<date>.xlsx` | Integration test API log (`pytest-*` only) | spreadsheet, not greppable |
| `bin/`, `dict/` | Copy of the deployment and dictionary under test (`pytest-*` only) | — |

## 3. CSV line formats

No header row. Fields are comma-separated and **not quoted**; only the
last field may itself contain commas, so cut it with `-f6-` / `-f5-`.

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
- IDs and opcodes are decimal; event messages print opcodes in hex
  (`Opcode 0x5002003`).
- Channel `value` carries its unit when the dictionary has a format
  string (`217 us`); arrays and structs print as Python literals
  (`['12', '1']`, `{'type': 'SQUARE', ...}`).

## 4. Flight software console log (`<App>.log`)

```text
EVENT: (83894275) (2:1795470283,610893) WARNING_HI: (FileHandling.fileManager) FileRemoveError : Could not remove file /nonexistent/file.txt, returned status 8
```

`EVENT: (id) (time_base:seconds,microseconds) SEVERITY: (instance) EventName : message`.
It differs from `event.log`: epoch time only, instance and event name
are split, and enum arguments show their number (`FORMAT_ERROR (3)`).
The console echo bypasses the downlink, so it is the only place that
has every event. An event here but not in `event.log` was emitted
before the client connected (boot events), filtered on board
(`SET_EVENT_FILTER`), or dropped (see the counter recipe in §5).
It also holds assert text and how the process ended: a normal stop is
`[WARNING] ReceiveTask failed to recv ...` then `Exiting...`.
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

# Commands the dispatcher reported as failed
grep -E 'cmdDisp\.(OpCodeError|MalformedCommand|InvalidCommand|TooManyCommands|CommandDroppedQueueOverflow)' event.log

# Fate of one command: its events, then who sent it (hex -> decimal;
# the directory names the tool)
grep -E 'Opcode 0x5002003 ' event.log
grep -H ",$((0x5002003))," ../*/command.log

# One channel: time series, latest value
grep ',Ref\.SG1\.Output,' channel.log | cut -d, -f1,5-
grep ',Ref\.SG1\.Output,' channel.log | tail -1

# On-board counters, last value: command errors, dropped events/commands
awk -F, '/\.(CommandErrors|CommandsDispatched|CommandsDropped|EventsDropped),/{last[$3]=$5} END{for(c in last) print c, last[c]}' channel.log

# Time window (ISO strings compare lexically)
awk -F, '$1>="2026-11-23T21:44:40" && $1<"2026-11-23T21:44:45"' event.log

# Merged timeline of commands (all clients, hex opcode) and events
{ awk -F, '{printf "%s,CMD 0x%x,%s,%s\n", $1, $4, $3, substr($0, index($0, ",[") + 1)}' ../*/command.log; cut -d, -f1,3,5- event.log; } | sort

# GDS process errors (connection, framing, decoding)
grep -rhE '\[(WARNING|ERROR|CRITICAL)\]' --include='*.log' .
```

An event name or ID alone does not tell you its arguments or severity
definition; look them up in the dictionary JSON
(`pytest-*/dict/`, else
`build-artifacts/<platform>/<App>/dict/*TopologyDictionary.json`).

## 6. Gotchas

- Empty `command.log` in `fprime-gds-*` is normal when commands came
  from the CLI or a test. Commands run by an on-board sequence appear in
  no `command.log`; only their `OpCodeDispatched` events show them.
- A component can refuse a command with a `WARNING_*` event and still
  report `OpCodeCompleted`; read the warnings between a command's
  dispatch and completion.
- `event.log` and `channel.log` start when that client connected and
  can miss boot events; use `<App>.log` for those.
- A zero-byte `event.log` in a short `fprime-cli-*` run means the tool
  exited before anything arrived, not that nothing happened.
- Each severity is one token, so `WARNING` alone also matches message
  text; anchor on `EventSeverity\.`.
- `--disable-data-logging` suppresses `event.log`, `channel.log`,
  `command.log`, `recv.bin`, and `sent.bin` entirely.
