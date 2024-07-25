event INVALID_SEQUENCER(
    idx: U16
) \
    severity warning high \
    format "Invalid sequence index {}"

event NO_AVAILABLE_SEQUENCERS() \
    severity warning low \
    format "No available cmd sequencers to dispatch a sequence to"

event UNKNOWN_SEQUENCE_FINISHED(
    idx: U16
) \
    severity warning low \
    format "Sequencer {} said it finished a sequence but we didn't even know it had started one"

event LOG_SEQUENCER_STATUS(
    idx: U16
    state: CmdSequencerState
    filename: string size 240
) \
    severity activity low \
    format "Sequencer {} with state {} is running file {}"