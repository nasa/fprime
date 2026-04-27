@ Dispatches a sequence to the first available sequencer
async command RUN(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                      $block: Fw.Wait @< Return command status when complete or not
                    ) \
    opcode 0

@ Dispatches a sequence with arguments to the first available sequencer
async command RUN_ARGS(
                      fileName: string size FileNameStringSize @< The name of the sequence file
                      $block: Fw.Wait @< Return command status when complete or not
                      buffer: Svc.SeqArgs @< Arguments to pass to a sequencer
                    ) \
    opcode 1

@ Logs via Events the state of each connected command sequencer
async command LOG_STATUS() opcode 2