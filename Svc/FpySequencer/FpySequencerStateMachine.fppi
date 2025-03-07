
struct StatementResponse {
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
}

struct SequenceExecutionArgs {
    filePath: string
    $block: BlockState
}

state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    @ return true if the goal state is RUNNING
    # see explanation in FpySequencer::m_goalState
    guard goalStateIs_RUNNING

    ####################
    # signals          #
    ####################
    @ called on VALIDATE cmd with the path of the sequence file to validate
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd
    signal cmd_RUN_VALIDATED: SequenceExecutionArgs
    @ called on CANCEL cmd
    signal cmd_CANCEL

    @ called when a statement response comes in
    signal cmdResponseIn: StatementResponse

    @ generic failure of an action
    signal result_failure
    @ generic success of an action
    signal result_success

    @ generic entry of a state
    signal entered

    ####################
    # actions          #
    ####################
    @ simply raises the "entered" signal
    action signalEntered
    @ sets the current sequence file path member var
    action setSequenceFilePath: SequenceExecutionArgs
    @ sets the block state of the sequence to be run
    action setSequenceBlockState: SequenceExecutionArgs
    @ performs all steps necessary for sequence validation, and raises a signal result_success or result_failure
    action validate

    @ reports that a sequence succeeded
    action report_seqSucceeded
    @ reports that a sequence was cancelled
    action report_seqCancelled
    @ reports that a sequence failed
    action report_seqFailed

    @ warns that the user cmd was invalid
    action report_invalidCmd
    @ warns that the sequence failed validation
    action report_invalidSeq
    @ warns that a statement response came in unexpectedly
    action report_unexpectedStatementResponse: StatementResponse

    @ sets the goal state to RUNNING
    # see explanation in FpySequencer::m_goalState
    action setGoalState_RUNNING
    @ sets the goal state to VALID
    # see explanation in FpySequencer::m_goalState
    action setGoalState_VALID
    @ sets the goal state to IDLE
    # see explanation in FpySequencer::m_goalState
    action setGoalState_IDLE

    @ responds to the calling command with OK
    action cmdResponseOut_OK
    @ responds to the calling command with EXECUTION_ERROR
    action cmdResponseOut_EXECUTION_ERROR


    initial enter IDLE

    @ sequencer is ready to load, validate and run a sequence
    state IDLE {
        # start with an unset goal state
        entry do { setGoalState_IDLE }
        # -->
        # wait for a cmd

        ####################
        # commands         #
        ####################
        # validate does not take as input a block state, only a path
        on cmd_VALIDATE do { setGoalState_VALID, setSequenceFilePath } enter VALIDATING
        # run takes both path and block state
        on cmd_RUN do { setGoalState_RUNNING, setSequenceFilePath, setSequenceBlockState } enter VALIDATING

        ####################
        # misc             #
        ####################
        on cmdResponseIn do { report_unexpectedStatementResponse }
    }

    state VALIDATING {

        # do it this way so we are only running validate while we
        # are in the VALIDATING state... although TODO does this matter?
        entry do { signalEntered }

        on entered do { validate }
        # -->
        on result_failure do { report_invalidSeq, cmdResponseOut_EXECUTION_ERROR } enter IDLE
        on result_success enter VALID

        ####################
        # commands         #
        ####################
        @ cancelled the sequence while it was validating
        # TODO remove support for this? validate is an atomic action, so we still wait for
        # validate to complete... but if we don't have this, then it means if we send RUN
        # and then CANCEL, and the CANCEL arrives in VALIDATING, the seq doesn't get cancelled..
        # i think we have to have it here...
        on cmd_CANCEL do { report_seqCancelled, cmdResponseOut_EXECUTION_ERROR } enter IDLE

        ####################
        # misc             #
        ####################
        on cmdResponseIn do { report_unexpectedStatementResponse }
    }

    @ decide whether we should stop after validating, or proceed to running
    choice VALID {
        # if we already received the RUN cmd, move on to RUNNING. otherwise wait
        if goalStateIs_RUNNING enter RUNNING else enter AWAITING_CMD_RUN_VALIDATED
    }

    @ sequencer has validated the sequence and is waiting for a command to run it
    state AWAITING_CMD_RUN_VALIDATED {

        # we can only get to this state if we explicitly ran the VALIDATE cmd
        # that means now's the time to return a response, as VALIDATE always waits
        # until completion to return response
        entry do { cmdResponseOut_OK }
        # wait here until we get the RUN_VALIDATED cmd

        ####################
        # commands         #
        ####################
        @ cancelled the sequence after we validated it
        on cmd_CANCEL do { report_seqCancelled } enter IDLE
        @ the sequence path has already been decided on, so only set the sequenceShouldBlock var
        on cmd_RUN_VALIDATED do { setSequenceBlockState } enter RUNNING

        ####################
        # misc             #
        ####################
        on cmdResponseIn do { report_unexpectedStatementResponse }
    }


    ###############
    # runtime     #    
    ###############

    @ given a statement response, return true if the
    @ statement successfully executed
    guard statementSuccessful: StatementResponse
    @ return true if the sleep is over
    guard shouldWake

    @ iterates to the next statement and dispatches it
    action stepStatement
    @ indicates to the component that the next call to stepStatement should cancel
    action cancelNextStepStatement
    @ resets the sequence runtime
    action resetRuntime

    @ called in stepStatement method when a statement was successfully dispatched
    signal result_stepStatement_dispatchedCmd
    @ called in stepStatement method when a statement has put us to sleep
    signal result_stepStatement_beginSleep
    @ called in stepStatement method when a statement was unable to be sent out
    signal result_stepStatement_failure
    @ called in stepStatement method when there were no more statements in the sequence
    signal result_stepStatement_noMoreStatements
    @ called in stepStatement method when the sequence execution was cancelled
    signal result_stepStatement_cancelled
    @ raised whenever the checkShouldWake port is called
    signal checkShouldWake
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time base
    signal mismatchedTimeBase
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time context
    signal mismatchedTimeContext

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        @ start with a fresh baked runtime
        entry do { resetRuntime }

        initial enter STEP_STATEMENT

        @ sequencer is stepping into a single statement and dispatching it
        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on result_stepStatement_cancelled do { report_seqCancelled, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_stepStatement_noMoreStatements do { report_seqSucceeded, cmdResponseOut_OK } enter IDLE
            on result_stepStatement_failure do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_stepStatement_beginSleep enter SLEEPING
            on result_stepStatement_dispatchedCmd enter AWAITING_CMD_RESPONSE
            # these can occur if the dispatch needs to compare a serialized time with the internal time (WAIT_REL)
            on mismatchedTimeBase do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on mismatchedTimeContext do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE

            @ we got a cmd response in but we weren't waiting for one
            on cmdResponseIn do { report_unexpectedStatementResponse }
        }

        @ sequencer is not taking any action, waiting for a time in the future to continue
        state SLEEPING {
            on checkShouldWake if shouldWake enter STEP_STATEMENT

            on mismatchedTimeBase do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on mismatchedTimeContext do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
        }
        
        @ sequencer is awaiting the execution result of a command
        state AWAITING_CMD_RESPONSE {
            on cmdResponseIn enter CMD_RESPONSE_RECEIVED
        }

        choice CMD_RESPONSE_RECEIVED {
            if statementSuccessful enter STEP_STATEMENT else do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
        }

        ####################
        # commands         #
        ####################
        # don't immediately enter cancellation. the only valid cancellation
        # point during the run loop is just before we're about to send out
        # a statement
        on cmd_CANCEL do { cancelNextStepStatement }
    }
}
