struct SequenceExecutionArgs {
    filePath: string size FileNameStringSize
    $block: BlockState
}

struct BreakpointArgs {
    @ whether or not to break at the breakpoint index
    breakOnBreakpoint: bool
    @ whether or not to remove the breakpoint after breaking on it
    breakOnlyOnceOnBreakpoint: bool
    @ the statement index at which to break, before dispatching
    breakpointIndex: U32
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
    @ called on VALIDATE cmd with the path of the sequence file to validate. only raised in IDLE state
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run. only raised in IDLE state
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd. only raised in AWAITING_CMD_RUN_VALIDATED state
    signal cmd_RUN_VALIDATED: SequenceExecutionArgs
    @ called on CANCEL cmd. raised in all states except IDLE
    signal cmd_CANCEL
    @ called in SET_BREAKPOINT cmd. raised in any state
    signal cmd_SET_BREAKPOINT: BreakpointArgs
    @ called in CLEAR_BREAKPOINT cmd. raised in any state
    signal cmd_CLEAR_BREAKPOINT

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
    @ called when a sequence failed to execute successfully
    action report_seqFailed
    @ called when a sequence starts
    action report_seqStarted

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
    action sendCmdResponse_OK
    @ responds to the calling command with EXECUTION_ERROR
    action sendCmdResponse_EXECUTION_ERROR

    @ clears all variables related to the loading/validating of the sequence file
    action clearSequenceFile
    @ clears the breakpoint setting
    action clearBreakpoint

    initial enter IDLE

    @ sequencer is ready to load, validate and run a sequence
    state IDLE {
        # start with an unset goal state, and no debugging settings
        entry do { clearBreakpoint, setGoalState_IDLE, clearSequenceFile }
        # -->
        # wait for a cmd

        ####################
        # commands         #
        ####################
        # validate does not take as input a block state, only a path
        on cmd_VALIDATE do { setGoalState_VALID, setSequenceFilePath } enter VALIDATING
        # run takes both path and block state
        on cmd_RUN do { setGoalState_RUNNING, setSequenceFilePath, setSequenceBlockState } enter VALIDATING

        on cmd_SET_BREAKPOINT do { setBreakpoint }
        on cmd_CLEAR_BREAKPOINT do { clearBreakpoint }
    }

    state VALIDATING {

        # do it this way so we are only running validate while we
        # are in the VALIDATING state
        entry do { report_seqStarted, signalEntered }

        on entered do { validate }
        # -->
        on result_failure do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
        on result_success enter VALID

        ####################
        # commands         #
        ####################
        @ cancelled the sequence while it was validating
        on cmd_CANCEL do { report_seqCancelled, sendCmdResponse_EXECUTION_ERROR } enter IDLE

        on cmd_SET_BREAKPOINT do { setBreakpoint }
        on cmd_CLEAR_BREAKPOINT do { clearBreakpoint }
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
        entry do { sendCmdResponse_OK }
        # wait here until we get the RUN_VALIDATED cmd

        ####################
        # commands         #
        ####################
        @ cancelled the sequence after we validated it
        on cmd_CANCEL do { report_seqCancelled } enter IDLE
        @ the sequence path has already been decided on, so only set the sequenceShouldBlock var
        on cmd_RUN_VALIDATED do { setSequenceBlockState } enter RUNNING

        on cmd_SET_BREAKPOINT do { setBreakpoint }
        on cmd_CLEAR_BREAKPOINT do { clearBreakpoint }
    }


    ###############
    # runtime     #    
    ###############

    @ checks if sequencer should wake from sleep
    action checkShouldWake
    @ iterates to the next statement and dispatches it
    action dispatchStatement
    @ resets the sequence runtime
    action resetRuntime
    @ checks if the current statement has timed out
    action checkStatementTimeout
    @ increments the m_sequencesStarted counter
    action incrementSequenceCounter
    @ reports that a breakpoint was hit
    action report_seqBroken
    @ sets the breakpoint to the provided args
    action setBreakpoint: BreakpointArgs
    @ sets the "break on next line" flag to true
    action setBreakBeforeNextLine
    @ sets the "break on next line" flag to false
    action clearBreakBeforeNextLine


    @ called in dispatchStatement method when a statement was successfully dispatched
    signal result_dispatchStatement_success
    @ called in dispatchStatement method when a statement was unable to be sent out
    signal result_dispatchStatement_failure
    @ called in dispatchStatement method when there were no more statements in the sequence
    signal result_dispatchStatement_noMoreStatements
    @ raised whenever the checkTimers port is called
    signal checkTimersIn
    @ raised when we are done sleeping
    signal result_checkShouldWake_wakeup
    @ raised when we should keep sleeping
    signal result_checkShouldWake_keepSleeping
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time base or context
    signal result_timeOpFailed
    @ a statement is telling the sequencer to go to sleep
    signal stmtResponse_beginSleep
    @ called when statement successfully executed. only raised in the RUNNING.AWAITING_CMD_RESPONSE state
    signal stmtResponse_success
    @ called when the statement unsuccessfully executed. only raised in the RUNNING.AWAITING_CMD_RESPONSE state
    signal stmtResponse_failure
    @ called when an unexpected or incorrect statement response comes in. only raised in the RUNNING state
    signal stmtResponse_unexpected
    @ called when the statement is telling the sequencer to await a later stmt response
    signal stmtResponse_keepWaiting
    @ raised when the statement times out, according to the timeout parameter
    signal result_checkStatementTimeout_statementTimeout
    @ raised when the statement has not timed out yet
    signal result_checkStatementTimeout_noTimeout

    @ called in CONTINUE cmd. only raised in RUNNING.PAUSED state
    signal cmd_CONTINUE
    @ called in BREAK cmd. only raised in RUNNING state
    signal cmd_BREAK
    @ called in STEP cmd. only raised in RUNNING.PAUSED state
    signal cmd_STEP

    @ return true if should break at this point in execution, before dispatching
    @ next stmt
    guard shouldBreak
    @ return true if this breakpoint should only happen once
    guard breakOnce

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        @ start with a fresh baked runtime, and tick up the
        @ sequence counter
        entry do { resetRuntime, incrementSequenceCounter }

        initial enter BREAK_CHECK

        @ check whether to pause execution before dispatching the next statement
        choice BREAK_CHECK {
            if shouldBreak do { report_seqBroken } enter PAUSED else enter DISPATCH_STATEMENT
        }

        @ sequencer is stepping into a single statement and dispatching it
        state DISPATCH_STATEMENT {
            entry do { dispatchStatement }
            # -->
            on result_dispatchStatement_noMoreStatements do { report_seqSucceeded, sendCmdResponse_OK } enter IDLE
            on result_dispatchStatement_failure do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            on result_dispatchStatement_success enter AWAITING_STATEMENT_RESPONSE
        }

        state AWAITING_STATEMENT_RESPONSE {
            on checkTimersIn do { checkStatementTimeout }
            # -->
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # result_checkStatementTimeout_noTimeout not handled, no need

            on stmtResponse_success enter BREAK_CHECK
            on stmtResponse_failure do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            on stmtResponse_beginSleep enter SLEEPING
            # stmtResponse_keepWaiting not handled, we're already in the awaiting state
        }

        @ sequencer is not taking any action, waiting for a time in the future to continue
        state SLEEPING {
            on checkTimersIn do { checkShouldWake, checkStatementTimeout }
            # -->
            on result_checkShouldWake_wakeup enter BREAK_CHECK
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # if result_checkShouldWake_keepSleeping is raised, stay asleep
            # if result_checkStatementTimeout_noTimeout is raised, stay asleep
        }

        @ a breakpoint was hit or the sequence was broken via command. 
        @ sequencer is not taking any action, or allowing timeouts.
        @ must be exited via command, either CANCEL or CONTINUE
        state PAUSED {
            entry do { signalEntered, clearBreakBeforeNextLine }
            # -->
            on entered if breakOnce do { clearBreakpoint }

            on cmd_CONTINUE enter DISPATCH_STATEMENT
            # a step is really just a continue followed immediately by a break
            on cmd_STEP do { setBreakBeforeNextLine } enter DISPATCH_STATEMENT
        }

        @ fail the sequence if we got an unexpected statement response while running it.
        @ the definition of unexpected is complicated, see FpySequencer::cmdResponseIn_handler.
        @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
        @ sometimes, we can produce responses in strange situations nominally by cancelling and
        @ quickly running sequences. this signal does not cover such cases.
        on stmtResponse_unexpected do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE

        ####################
        # commands         #
        ####################
        on cmd_CANCEL do { report_seqCancelled, sendCmdResponse_EXECUTION_ERROR } enter IDLE
        on cmd_BREAK do { setBreakBeforeNextLine }
        on cmd_SET_BREAKPOINT do { setBreakpoint }
        on cmd_CLEAR_BREAKPOINT do { clearBreakpoint }
    }
}
