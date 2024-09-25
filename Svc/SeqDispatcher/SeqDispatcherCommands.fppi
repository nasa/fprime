@ Dispatches a sequence to the first available sequencer
async command RUN(
                      fileName: string size 240 @< The name of the sequence file
                      $block: Fw.Wait @< Return command status when complete or not
                    ) \
    opcode 0

@ Logs via Events the state of each connected command sequencer
async command LOG_STATUS() opcode 1