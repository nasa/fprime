event InvalidSequencer(
    idx: U16
) \
    severity warning high \
    format "Invalid sequence index {}"

event NoAvailableSequencers() \
    severity warning low \
    format "No available cmd sequencers to dispatch a sequence to"

event UnknownSequenceFinished(
    idx: U16
) \
    severity warning low \
    format "Sequencer {} said it finished a sequence but we didn't even know it had started one"

event UnexpectedSequenceStarted(
    idx: U16,
    newSequence: string size 240,
    sequenceInInternalState: string size 240
) \
    severity warning high \
    format "Sequencer {} said it started a sequence {} but we thought it was still running {}"

event LogSequencerStatus(
    idx: U16
    state: CmdSequencerState
    filename: string size 240
) \
    severity activity low \
    format "Sequencer {} with state {} is running file {}"
