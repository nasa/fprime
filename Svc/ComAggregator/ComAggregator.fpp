module Svc {
    @ Aggregation state machine
    state machine AggregationMachine {
        @ Initial state: WAIT_STATUS
        @ Wait for initial 'status' to start the process
        initial enter WAIT_STATUS

        @ Rate-group driven timeout signal
        signal timeout

        @ Fill buffer signal
        signal fill: Svc.ComDataContextPair

        @ Status return
        signal status: Fw.Success

        @ Check if the aggregation buffer is full, i.e. cannot accept the incoming buffer
        guard isFull: Svc.ComDataContextPair

        @ Check if the aggregation will fill, i.e. the incoming buffer will exactly fit
        guard willFill: Svc.ComDataContextPair

        @ Check if not empty
        guard isNotEmpty

        @ Check if last status is good 
        guard isGood: Fw.Success

        @ Check if the aggregation buffer was completely filled from held continuation data
        guard isSpanFull

        @ Clear the buffer fill state, last status
        action doClear

        @ Fill the buffer with data
        action doFill: Svc.ComDataContextPair

        @ Send the buffer data
        action doSend

        @ Hold a buffer
        action doHold: Svc.ComDataContextPair

        @ Hold a buffer, first splitting its leading bytes into the remaining frame space when spanning
        action doSplitHold: Svc.ComDataContextPair


        @ Assert no status when in fill state
        action assertNoStatus

        @ The IS_FULL_THEN_SEND choice: this will check if the aggregation buffer is too-full to allow another buffer.
        @ Otherwise, it will continue to fill.
        choice IS_FULL_THEN_SEND {
            if isFull do { doSplitHold, doSend } enter WAIT_STATUS \
                else enter WILL_FILL_THEN_SEND
        }

        @ The WILL_FILL_THEN_SEND choice: this will check if the buffer will be exactly filled by the incoming buffer.
        @ Otherwise, it will continue to fill.
        choice WILL_FILL_THEN_SEND {
            if willFill do { doFill, doSend } enter WAIT_STATUS \
                else do { doFill } enter FILL
        }

        @ The IS_GOOD_STATUS choice
        choice IS_GOOD_STATUS {
            if isGood do { doClear } enter SPAN_CHECK \
                else enter WAIT_STATUS
        }

        @ The SPAN_CHECK choice: after a clear, held continuation data may have completely refilled the
        @ aggregation buffer (a packet spanning one or more full frames). If so, send again immediately.
        choice SPAN_CHECK {
            if isSpanFull do { doSend } enter WAIT_STATUS \
                else enter FILL
        }
        
        @ Wait for com status from downstream
        state WAIT_STATUS {
            # ASSERT: fill cannot happen before initial 'status'
            on fill do { doHold }
            # IGNORE: 'timeout', this signal is irrelevant
            # On status, move to IS_GOOD_STATUS choice
            on status enter IS_GOOD_STATUS
        }

        @ Buffer aggregation in-progress
        state FILL {
            # Fill buffer, check if full then send or fill
            on fill enter IS_FULL_THEN_SEND
            # Timeout, send buffer
            on timeout if isNotEmpty do { doSend } enter WAIT_STATUS
            # ASSERT: status cannot happen while filling
            on status do { assertNoStatus }
        }
    }

    @ Aggregates com buffers
    active component ComAggregator {
        import Svc.Framer
        sync input port timeout: Svc.Sched

        @ State machine instance for aggregation state machine
        state machine instance aggregationMachine: AggregationMachine
    }
}