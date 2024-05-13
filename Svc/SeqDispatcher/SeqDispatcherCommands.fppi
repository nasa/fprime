async command RUN(
                      fileName: string size 240 @< The name of the sequence file
                      $block: Svc.CmdSequencer.BlockState @< Return command status when complete or not
                    ) \
    opcode 0

async command LOG_STATUS() opcode 1

async command PING() opcode 2