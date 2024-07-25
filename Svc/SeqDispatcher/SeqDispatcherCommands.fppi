async command RUN(
                      fileName: string size 240 @< The name of the sequence file
                      $block: Fw.Wait @< Return command status when complete or not
                    ) \
    opcode 0

async command LOG_STATUS() opcode 1