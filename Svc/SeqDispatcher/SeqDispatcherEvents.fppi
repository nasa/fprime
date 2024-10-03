event InvalidSequencer(
    idx: U16
) \
    severity warning high \
    format "Invalid sequence index {}"

event NoAvailableSequencers() \
    severity warning high \
    format "No available cmd sequencers to dispatch a sequence to"

event UnknownSequenceFinished(
    idx: U16
) \
    severity warning low \
    format "Sequencer {} completed a sequence with no matching start notification"

event ConflictingSequenceStarted(
    idx: U16,
    newSequence: string size 240,
    sequenceInInternalState: string size 240
) \
    severity warning high \
    format "Sequencer {} started a sequence {} while still running {}"

event UnexpectedSequenceStarted(
    idx: U16,
    newSequence: string size 240
) \
    severity warning low \
    format "Sequencer {} was externally commanded to start a sequence {}"

event LogSequencerStatus(
    idx: U16
    $state: CmdSequencerState
    filename: string size 240
) \
    severity activity low \
    format "Sequencer {} with state {} is running file {}"