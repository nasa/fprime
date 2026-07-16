# components::SeqDispatcher

Dispatches command sequences to available command sequencers, allowing the spacecraft controllers to run multiple sequences at once without having to manually manage which `CmdSequencer`s those sequences run on.

### Usage
* Call the `RUN` command just like you would call it on a `CmdSequencer`
* If any connected `CmdSequencer` is available, it will route the sequence to the first one it finds
* `RUN` can be made blocking or non-blocking, just like `CmdSequencer`'s `RUN`

## State diagram
![State diagram of the SeqDispatcher](seq_dispatcher_model.png "SeqDispatcher model")

## Port Descriptions
|Type| Name | Description |
|async input|seqRunIn|Equivalent to the RUN cmd, dispatches a sequence to the first available sequencer|
|output|seqRunOut|This is used by the SeqDispatcher to send sequence run calls to sequencers|
|output|seqCancelOut|This is used by the SeqDispatcher to cancel a running sequence on a specific sequencer|
|async input|seqDoneIn|Called by a command sequencer whenever it has finished any sequence|
|async input|seqStartIn|Called by a command sequencer whenever it starts any sequence|

## Commands
| Name | Description |
|RUN|Dispatches a sequence to the first available sequencer|
|LOG_STATUS|Logs via Events the state of each connected command sequencer|
|ABORT|Aborts any running sequence matching the given file name. Cancels the sequencer(s) running that file; the resulting seqDoneIn clears the dispatcher's state|

## Events
| Name | Description |
|InvalidSequencer|The given sequencer index is invalid for an unspecified reason|
|NoAvailableSequencers|There are no available sequencers to dispatch a sequence to|
|UnknownSequenceFinished|We received a call to seqDoneIn that didn't have a corresponding seqStartIn call|
|UnexpectedSequenceStarted|We received a call to seqStartIn but we didn't receive a call to seqDoneIn before that|
|LogSequencerStatus|Shows the current state and sequence filename for a particular sequencer. Produced by the LOG_STATUS command|
|SequenceAborted|A running sequence matching the ABORT file name was canceled on the given sequencer|
|AbortSequenceNotFound|No running sequence matched the ABORT file name|



## Telemetry
| Name | Description |
|dispatchedCount|Number of sequences dispatched|
|errorCount|Number of sequences dispatched that returned an error. Note: if a sequence was run in non-blocking mode, even if the sequence errors out, this error count will never increase|
|sequencersAvailable|Number of sequencers ready to run a sequence|
|abortedCount|Number of sequences aborted by the ABORT command|

## Unit Tests
Add unit test descriptions in the chart below
| Name | Description |
|testDispatch|Tests the basic dispatch functionality of the `SeqDispatcher`|
|testLogStatus|Tests the LOG_STATUS command|
|testAbort|Tests that ABORT cancels the matching sequencer and clears state on seqDoneIn|
|testAbortNotFound|Tests that ABORT with an unmatched file name errors and cancels nothing|

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
|---|---|---|