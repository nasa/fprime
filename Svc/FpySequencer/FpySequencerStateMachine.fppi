
struct CmdResponse {
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
    @ called on VALIDATE cmd with the path of the sequence file to validate. only raised in IDLE state
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run. only raised in IDLE state
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd. only raised in AWAITING_CMD_RUN_VALIDATED state
    signal cmd_RUN_VALIDATED: SequenceExecutionArgs
    @ called on CANCEL cmd. raised in all states except IDLE
    signal cmd_CANCEL

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
    @ warns that a cmd response came in unexpectedly
    action report_unexpectedCmdResponse: CmdResponse

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
    }


    ###############
    # runtime     #    
    ###############

    @ given a cmd response, return true if the
    @ cmd successfully executed
    guard cmdSuccessful: CmdResponse
    @ return true if the sleep is over
    guard shouldWake
    @ given a cmd response, return true if we
    @ were expecting this cmd to respond
    guard expectedCmd: CmdResponse

    @ iterates to the next statement and dispatches it
    action dispatchStatement
    @ indicates to the component that the next call to dispatchStatement should cancel
    action cancelNextStepStatement
    @ resets the sequence runtime
    action resetRuntime
    @ reports that the wrong cmd opcode came in thru the cmdResponseIn port
    action report_wrongCmdResponse

    @ called in dispatchStatement method when a statement was successfully dispatched
    signal result_dispatchStatement_dispatchedCmd
    @ called in dispatchStatement method when a statement has put us to sleep
    signal result_dispatchStatement_beginSleep
    @ called in dispatchStatement method when a statement was unable to be sent out
    signal result_dispatchStatement_failure
    @ called in dispatchStatement method when there were no more statements in the sequence
    signal result_dispatchStatement_noMoreStatements
    @ called in dispatchStatement method when the sequence execution was cancelled
    signal result_dispatchStatement_cancelled
    @ raised whenever the checkShouldWake port is called
    signal checkShouldWake
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time base or context
    signal timeOperationError
    signal directiveResponse_beginSleep
    @ called when the expected cmd response comes in, and it is OK. only raised in the RUNNING.AWAITING_CMD_RESPONSE state
    signal cmdResponse_success: CmdResponse
    @ called when the expected cmd response comes in, and it is not OK. only raised in the RUNNING.AWAITING_CMD_RESPONSE state
    signal cmdResponse_failure: CmdResponse

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        @ start with a fresh baked runtime
        entry do { resetRuntime }

        initial enter DISPATCH_STATEMENT

        @ sequencer is stepping into a single statement and dispatching it
        state DISPATCH_STATEMENT {
            entry do { dispatchStatement }
            # -->
            on result_dispatchStatement_cancelled do { report_seqCancelled, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_dispatchStatement_noMoreStatements do { report_seqSucceeded, cmdResponseOut_OK } enter IDLE
            on result_dispatchStatement_failure do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
            on result_dispatchStatement_success enter AWAITING_RESPONSE

            @ we got a cmd response in but we weren't waiting for one
            on cmdResponseIn do { report_unexpectedCmdResponse }
        }

        state AWAITING_RESPONSE {
            on cmdResponse do 
            on beginSleep
        }

        @ sequencer is not taking any action, waiting for a time in the future to continue
        state SLEEPING {
            on checkShouldWake if shouldWake enter DISPATCH_STATEMENT
            # TODO what to do if CANCEL while sleeping?

            # this can occur if the time base/ctx changes, or was invalid from the start
            on timeOperationError do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
        }
        
        @ sequencer is awaiting the execution result of a command
        state AWAITING_CMD_RESPONSE {
            on cmdResponseIn if expectedCmd enter SOME_CMD_RESPONSE_RECEIVED
        }
        # -->
        choice SOME_CMD_RESPONSE_RECEIVED {
            if expectedCmd enter RIGHT_CMD_RESPONSE_RECEIVED else do { report_wrongCmdResponse } enter IDLE
        }
        # -->
        choice RIGHT_CMD_RESPONSE_RECEIVED {
            if cmdSuccessful enter DISPATCH_STATEMENT else do { report_seqFailed, cmdResponseOut_EXECUTION_ERROR } enter IDLE
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
