enum OpState {
  ACCUMULATE = 0
  DRAIN = 1
}

@ Set the mode
async command BA_SetMode(
                          mode: OpState
                        ) \
  opcode 0x00
