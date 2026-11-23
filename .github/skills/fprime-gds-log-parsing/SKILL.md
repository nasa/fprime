---
name: fprime-gds-log-parsing
description: Locate and parse F Prime GDS logs (event.log, channel.log, command.log, flight software console log) - file map, line formats, grep recipes, and gotchas. Use when answering a question from a GDS log directory (`logs/`, a CI `gds-logs` artifact), such as an integration-test failure, a failed command, or a crash/FATAL.
---

# Skill: Parsing F Prime GDS logs

## 1. Where logs are

Each GDS tool invocation logs to a run directory
`<logs>/<tool>-<YYYY_MM_DD-HH_MM_SS>/` (local time of that process),
where `<tool>` is `fprime-gds`, `fprime-cli`, or `pytest` (integration
tests) unless `--log-prefix` changes it (`''` leaves a bare timestamp).
`<logs>` is `-l/--logs`, default `./logs` under the directory the tool
was started from; with `--log-directly` files go into `<logs>` itself.
Newest GDS session: `ls -d logs/fprime-gds-*/ | tail -1`.

**Logs are per client, not per session.** Each directory holds only
what that process sent and what it received while connected. A command
sent with `fprime-cli` or by a test is in *that* tool's `command.log`,
not in `fprime-gds-*/command.log` (GUI-sent commands only). For events
and telemetry read the longest-connected client, normally
`fprime-gds-*`, whose CSV logs are written by the GUI server: with
`--gui none` that directory has only `<App>.log` and Python logs. For
commands read every sibling `command.log`. Nothing names the session a
client belonged to: match by time, using the epoch seconds in
`raw_time` (§3).

Invocations started in the same second share a directory, and each
appends its own copy of every received line: `sort -u` such an
`event.log` or `channel.log` before counting.

## 2. File map

| File | Content | Format |
|---|---|---|
| `event.log` | Events received and decoded | CSV, §3 |
| `channel.log` | Telemetry channel updates received | CSV, §3 |
| `command.log` | Commands sent by this client | CSV, §3 |
| `<App>.log` (e.g. `Ref.log`) | stdout/stderr of the flight software the GDS launched: console event echo, asserts, OS warnings. Only in `fprime-gds-*`, absent with `-n` | text, §4 |
| `recv.bin`, `sent.bin` | Raw packets received; `sent.bin` is usually empty, so it does not tell whether a command was sent | binary, do not grep |
| `fprime-gds.log`, `comm.py.log`, `__main__.py.log`, `fprime-cli.log`, `pytest.log`, `<plugin>/-c.log` | Python logging of each GDS process (launcher, comm adapter, GUI server, CLI, test, plugins). Usually start-up INFO lines only | `[date time,ms] [LEVEL] logger: msg` |
| `<destination file>.log` | Packet trace of one file downlink (`fprime-gds-*` only, copy under `<plugin>/`) | text |

## 3. CSV line formats

No header row. Fields are comma-separated and **not quoted**. The last
field (message, value, or argument list) can contain commas, so take it
with `cut -d, -f6-` (events) or `-f5-` (channels, commands). An event
message can also contain a newline: such a record spans two lines.

```text
event.log    time,raw_time,instance.path.EventName,event_id,EventSeverity.<SEV>,message
channel.log  time,raw_time,instance.path.ChannelName,channel_id,value
command.log  time,raw_time,instance.path.COMMAND,opcode,[args as a Python list repr]
```

```text
2026-11-23T21:44:43.610893,(2(0)-1795470283:610893),FileHandling.fileManager.FileRemoveError,83894275,EventSeverity.WARNING_HI,Could not remove file /nonexistent/file.txt, returned status 8
2026-11-23T21:44:20.089989,(2(0)-1795470260:89989),Ref.rateGroup2Comp.RgMaxTime,268443648,217 us
2026-11-23T21:44:43.599225,(2(0)-1795470283:599225),FileHandling.fileManager.RemoveFile,83894275,['/nonexistent/file.txt', False]
```

- `time`: ISO local time of the process that wrote the file, no zone
  suffix. Lines are in arrival order; `sort` works within one time
  zone. For events and channels it is the **flight** timestamp, for
  commands the ground send time.
- `raw_time`: `(time_base(context)-seconds:microseconds)`; the seconds
  are zone-independent, so use them to join logs across time zones.
- IDs and opcodes are decimal. `cmdDisp` messages print opcodes in hex
  (`Opcode 0x5002003`), sequencer messages in decimal
  (`opcode 83894275`) with a numeric result (`error 4` is
  `EXECUTION_ERROR`, `error 3` is `FORMAT_ERROR`).
- A channel is logged each time the flight software sends it, not on a
  fixed period, so the last sample can lag a crash.

## 4. Flight software console log (`<App>.log`)

```text
EVENT: (83894275) (2:1795470283,610893) WARNING_HI: (FileHandling.fileManager) FileRemoveError : Could not remove file /nonexistent/file.txt, returned status 8
```

`EVENT: (id) (time_base:seconds,microseconds) SEVERITY: (instance) EventName : message`.
It differs from `event.log`: epoch time only, instance and event name
are split, enum arguments show their number (`FORMAT_ERROR (3)`), and
long messages are truncated (the next event can then share the line, so
count with `grep -o 'EVENT: (' Ref.log | wc -l`).
The console echo bypasses the downlink, so it has every event except
FATALs. An event here but not in `event.log` was emitted before the
client connected (boot events), filtered on board (`SET_EVENT_FILTER`,
`SET_ID_FILTER`), or dropped (`EventsDropped` channel, §5).

It also shows how the process ended. A normal stop is
`[WARNING] ReceiveTask failed to recv ...` then `Exiting...`. A FATAL
event or an assert ends with `FATAL <decimal event id> handled.` and
`Exiting with abort signal`. An assert also prints
`Assert: "<file>:<line>" <args>` first and usually reaches `event.log`
as `fatalAdapter.AF_ASSERT_<n>`. A FATAL event prints only its ID and
may never reach `event.log`: look the ID up under events in the
dictionary (the same number can be an unrelated opcode). A segfault or
`kill -9` leaves no final line at all.

## 5. Recipes

Run from inside a run directory. `../*/command.log` spans every client
under the same `<logs>`; add the path of `--log-directly` directories.

```bash
# Anything non-nominal, and a severity histogram
awk -F, '$5 ~ /WARNING|FATAL/' event.log
cut -d, -f5 event.log | sort | uniq -c

# Events of one component instance, compact (time, name, message)
grep ',Ref\.SG1\.' event.log | cut -d, -f1,3,6-

# Commands the dispatcher reported as failed
grep -E 'cmdDisp\.(OpCodeError|MalformedCommand|InvalidCommand|TooManyCommands|CommandDroppedQueueOverflow)' event.log

# Fate of one command: its events, then who sent it (hex -> decimal;
# the directory names the tool)
grep -E 'Opcode 0x5002003 ' event.log
grep -H ",$((0x5002003))," ../*/command.log

# One channel: time series (add `| tail -1` for the latest value)
grep ',Ref\.SG1\.Output,' channel.log | cut -d, -f1,5-

# On-board counters, last value: command errors, dropped events/commands
awk -F, '$3 ~ /\.cmdDisp\.Command|\.EventsDropped$/{last[$3]=$5} END{for(c in last) print c, last[c]}' channel.log

# Merged timeline of commands (all clients, hex opcode) and events
{ awk -F, '{printf "%s,CMD 0x%x,%s,%s\n", $1, $4, $3, substr($0, index($0, ",[") + 1)}' ../*/command.log; cut -d, -f1,3,5- event.log; } | sort
```

For an event's or command's arguments, look it up in the dictionary
JSON (`pytest-*/dict/`, else
`build-artifacts/<platform>/<App>/dict/*TopologyDictionary.json`).

## 6. Gotchas

- An `OpCodeDispatched` with no completion means a crash **or** lost
  events (dropped or filtered, including the `COMMAND` severity). Check
  `EventsDropped` first. The `CommandsDispatched` counter is exact
  (sequence commands included); `grep -c OpCodeDispatched event.log`
  under-counts when events were lost.
- Commands run by an on-board sequence appear in no `command.log`:
  follow `OpCodeDispatched` and the sequencer's `CS_CommandComplete` /
  `CS_CommandError` events. File uplinks are in no `command.log`
  either, only in `fileUplink` events.
- A dispatch with no `command.log` line otherwise came from a client
  that logged elsewhere (`--log-directly`, another `-l`) or not at all
  (`--disable-data-logging`: no CSV or `.bin` files).
- A component can refuse a command with a `WARNING_*` event and still
  report `OpCodeCompleted`; read the events from just before a
  command's dispatch (a handler's events can precede
  `OpCodeDispatched` by microseconds) to its completion.
- A command sent after the flight software died is still in
  `command.log`; only the missing `OpCodeDispatched` shows it.
- `event.log` and `channel.log` start when that client connected and
  can miss boot events; use `<App>.log` for those. A zero-byte
  `event.log` in a short `fprime-cli-*` run means the tool exited
  before anything arrived, not that nothing happened.
- A `channel.log` that stops growing means the flight software died or
  disconnected; the GDS process logs say nothing about it, and with
  `-n` (no `<App>.log`) the cause is not in the logs.
- A launch or CLI call rejected at argument checking prints its error
  on the terminal only; no log has it.
