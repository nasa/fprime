enum TrapReason {
  @ No recorded trap
  NONE
  @ Triggered by unreachable instruction
  UNREACHABLE
  @ A host function has noted an unrecoverable failure
  HOST
  @ Integer or floating point division by zero
  DIVIDE_BY_ZERO
  @ An indirect call tried to map to a table function out of range
  INVALID_TABLE_INDEX
  @ The function type in an indirect call does not match the function pointer's type
  INVALID_TABLE_FUNCTION_TYPE
  @ An indirect call referenced an uninitialized table element
  UNINITIALIZED_TABLE_ELEMENT
  @ An imported global could not be read
  GLOBAL_GET_FAILED
  @ An imported global could not be set
  GLOBAL_SET_FAILED
  @ The interpreter ran out of memory
  OUT_OF_MEMORY
  @ memory.grow failed because a host function has taken ownership of a memory
  MEMORY_REF_NOT_UNIQUE
  @ A memory operation is out of bounds
  MEMORY_OUT_OF_BOUNDS
  @ Ran out of stack space
  STACK_OVERFLOW
  @ Attempting to convert Inf to integer
  UNREPRESENTABLE_RESULT
  @ Signed division causes integer overflow
  INTEGER_OVERFLOW
  @ Attempting to convert NaN to integer
  BAD_CONVERSION_TO_INTEGER
}

enum AllocError: U8 {
  @ Not enough pages could be allocated to accommodate this allocation
  OUT_OF_MEMORY

  @ Page was too small to fit this allocation
  PAGE_TOO_SMALL

  @ A generic allocation failure
  ALLOCATION_FAILED
}

@ diagram-layout engine=elk direction=RL cycleBreaking=GREEDY_MODEL_ORDER considerModelOrder=NODES_AND_EDGES nodePlacement=BRANDES_KOEPF
state machine EngineStateMachine {
  initial enter IDLE

  ###############
  # Commands    #
  ###############

  @ called on CONTINUE cmd. only raised in RUNNING.PAUSED state
  signal cmd_CONTINUE

  @ Called on CANCEL cmd. Raised in LOADING, READY and RUNNING modes
  signal cmd_CANCEL

  ###############
  # Guards      #
  ###############

  @ return true if execution should pause before spinning the next statement
  guard pendingPause

  @ a host function is waiting to be processed
  guard pendingHostFunction

  @ the pending host function is a sleep (therefore we need to check the sleep timers)
  guard pendingHostFunctionIsSleep

  ###############
  # Actions     #
  ###############

  @ generic signal raised
  action signalEntered

  @ spins the interpreter loop, executing up to a bounded number of instructions
  action spin

  @ resets the engine's state (clears operand stack, pc, fp, sp)
  action reset

  ###############
  # Responses   #
  ###############

  action clearExitStatus
  action setExitReason_INTERPRETER_FINISHED
  action setExitReason_INTERPRETER_TRAP
  action setExitReason_REPLY_TIMEOUT
  action setExitReason_HOST_FAILURE
  action setExitReason_TIMER_INCOMPARABLE
  action setExitReason_UNEXPECTED_REPLY
  action setExitReason_CANCEL

  action setExitCode: I32
  action setTrapReason: TrapReason
  action setLastHostFunction

  @ Send a signal back to the controller state machine that we have finished executing
  @ The response codes are stored in m_exitReason, m_exitCode, m_tlmLastTrapReason
  action finish

  ###############
  # Events      #
  ###############

  @ reports that execution was paused at a breakpoint
  action reportPaused

  #####################
  # Host interactions #
  #####################

  @ sets the pause flag to false
  action clearPause
  @ dispatch a host function port call
  action dispatchPendingHostFunction
  @ clears the pending host function port call
  action clearPendingHostFunction

  @ Generic entry of a state
  signal entered

  @ A function has been invoked, run the interpreter to completion
  signal run: RequestContext

  @ Set the current executing context
  action setContext: RequestContext

  @ Clear the current executing context
  action clearContext

  @ raised whenever the checkTimers port is called
  signal checkTimers

  @ Signal to AWAITING_RESPONSE to resume the interpreter with no host function return value
  signal hostResume

  @ spacewasm_engine_resume
  action resume

  @ Signal to AWAITING_RESPONSE to resume the interpreter with an i32 host function return value
  signal hostResumeI32: I32

  @ spacewasm_engine_resume_some(I32(value))
  action resumeI32: I32

  @ A periodic check on the pending timer to see if we can wake up
  action checkSleepTimers

  @ A periodic check on any host function to guard against timeouts
  action checkTimeout

  @ called when a paused host statement (F´ command) unsuccessfully executed. only raised in RUNNING.AWAITING_RESPONSE
  signal hostResponseFailure
  @ called when an unexpected or incorrect statement response comes in. only raised in the RUNNING state
  signal hostResponseUnexpected: HostFunction
  @ Fw::Times are not comparable during a sleep
  signal hostResponseTimeIncomparable
  @ A host function timed-out
  signal hostResponseTimeout

  #######################
  # Interpreter signals #
  #######################

  @ The program has completed
  signal interpreterFinished: I32
  @ The program has been aborted
  signal interpreterTrap: TrapReason
  @ No more fuel (ran to instruction bound)
  signal interpreterOutOfFuel
  @ An asynchronous host function has been called and has requested the interpreter pause
  signal interpreterPause

  @ The idle state means there is no function running in the engine
  state IDLE {
    entry do { clearContext, reset }
    on run do { setContext } enter RUNNING
  }

  @ sequencer is executing the loaded/invoked Wasm program
  state RUNNING {
    # Leaving the run state will always reply to the controller
    # We explicitely set the response state to unknown and update it before exiting the state
    entry do { clearExitStatus }
    exit do { clearPendingHostFunction, clearPause, finish }

    initial enter PAUSE_CHECK

    @ check whether to pause execution before spinning the next statement
    choice PAUSE_CHECK {
      if pendingPause enter PAUSED else enter SPINNING
    }

    @ sequencer is spinning the interpreter loop
    state SPINNING {
      entry do { signalEntered }
      on entered do { spin }

      on interpreterFinished do {
        setExitCode
        setExitReason_INTERPRETER_FINISHED
      } enter IDLE

      on interpreterOutOfFuel enter PAUSE_CHECK
      on interpreterPause enter HOST_FUNCTION
      on interpreterTrap do {
        setTrapReason
        setExitReason_INTERPRETER_TRAP
      } enter IDLE

      # Check if there is a pending pause before dispatching the host function
      choice HOST_FUNCTION {
        if pendingPause enter PAUSED else enter AWAITING_RESPONSE
      }
    }

    @ a host function paused the interpreter and is awaiting a response
    state AWAITING_RESPONSE {
      initial do {
        setLastHostFunction
        dispatchPendingHostFunction
      } enter PICK_AWAIT
      exit do { clearPendingHostFunction }

      # Check if we should simple wait or "sleep"
      choice PICK_AWAIT {
        if pendingHostFunctionIsSleep enter SLEEPING else enter WAITING
      }

      state WAITING

      @ Models the special case sleeping host function which has a time of it's own
      state SLEEPING {
        on checkTimers do { checkSleepTimers }
      }

      on hostResume do { resume } enter PAUSE_CHECK
      on hostResumeI32 do { resumeI32 } enter PAUSE_CHECK
      on hostResponseFailure do { setExitReason_HOST_FAILURE } enter IDLE

      on hostResponseTimeIncomparable do {
        setExitReason_TIMER_INCOMPARABLE
      } enter IDLE

      on hostResponseTimeout do { setExitReason_REPLY_TIMEOUT } enter IDLE

      on checkTimers do { checkTimeout }
    }

    state PAUSED {
      entry do { clearPause, reportPaused }

      on cmd_CONTINUE enter RESUME
      choice RESUME {
        if pendingHostFunction enter AWAITING_RESPONSE else enter SPINNING
      }
    }

    @ fail the sequence if we got an unexpected statement response while running it.
    @ the definition of unexpected is complicated, see WasmSequencer::cmdResponseIn_handler.
    @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
    @ sometimes, we can produce responses in strange situations nominally by cancelling and
    @ quickly running sequences. this signal does not cover such cases.
    on hostResponseUnexpected do { setExitReason_UNEXPECTED_REPLY } enter IDLE

    ####################
    # commands         #
    ####################

    on cmd_CANCEL do { setExitReason_CANCEL } enter IDLE
  }
}
