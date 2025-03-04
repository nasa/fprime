state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    @ return true if the goal state is RUNNING
    # see explanation in FpySequencer::m_goalState
    guard goalStateIs_RUNNING

    @ given a statement response, return true if the
    @ statement successfully executed
    guard statementSuccessful: StatementResponse

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
    signal statementResponseIn: StatementResponse

    @ called in stepStatement method when a statement was successfully dispatched
    signal result_stepStatement_success
    @ called in stepStatement method when a statement was unable to be sent out
    signal result_stepStatement_failure
    @ called in stepStatement method when there were no more statements in the sequence
    signal result_stepStatement_noMoreStatements
    @ called in stepStatement method when the sequence execution was cancelled
    signal result_stepStatement_cancelled

    @ generic failure of an action
    signal result_failure
    @ generic success of an action
    signal result_success

    ####################
    # actions          #
    ####################
    @ sets the current sequence file path member var
    action setSequenceFilePath: SequenceExecutionArgs
    @ sets the block state of the sequence to be run
    action setSequenceBlockState: SequenceExecutionArgs
    @ resets the sequence runtime
    action resetRuntime
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

    @ iterates to the next statement and dispatches it
    action stepStatement
    
    @ indicates to the component that the next call to stepStatement should cancel
    action cancelNextStepStatement

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

    ####################
    # topology         #
    ####################
    initial enter IDLE

    @ sequencer is ready to load, validate and run a sequence
    state IDLE {
        # start with an unset sequence file, and unset goal state
        entry do { resetRuntime, setGoalState_IDLE }
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
        on statementResponseIn do { report_unexpectedStatementResponse }
    }

    state VALIDATING {

        entry do { validate }

        on result_failure do { report_invalidSeq, cmdResponseOut_EXECUTION_ERROR } enter IDLE
        # close the sequence file now that it's in memory
        on result_success enter VALID

        ####################
        # commands         #
        ####################
        @ cancelled the sequence while it was validating
        on cmd_CANCEL do { report_seqCancelled, cmdResponseOut_EXECUTION_ERROR } enter IDLE

        ####################
        # misc             #
        ####################
        on statementResponseIn do { report_unexpectedStatementResponse }
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
        on statementResponseIn do { report_unexpectedStatementResponse }
    }

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        initial enter STEP_STATEMENT

        @ sequencer is stepping into a single statement and sending it out to the dispatcher
        state STEP_STATEMENT {
            entry do { stepStatement }
            # -->
            on result_stepStatement_cancelled do { report_seqCancelled, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_stepStatement_noMoreStatements do { report_seqSucceeded, cmdResponseOut_OK } enter IDLE
            on result_stepStatement_failure do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_stepStatement_success enter AWAITING_STATEMENT_RESPONSE


            on statementResponseIn do { report_unexpectedStatementResponse }
        }
        
        @ sequencer is awaiting the execution result of a directive
        state AWAITING_STATEMENT_RESPONSE {
            on statementResponseIn enter STATEMENT_DONE
        }

        choice STATEMENT_DONE {
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
