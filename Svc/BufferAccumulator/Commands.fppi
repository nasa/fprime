enum OpState {
  ACCUMULATE = 0
  DRAIN = 1
}

@ Set the mode
async command BA_SetMode(
                          mode: OpState
                        ) \
  opcode 0x00

enum BlockMode {
  NOBLOCK = 0
  BLOCK = 1
}

@ Drain the commanded number of buffers
async command BA_DrainBuffers(
                               numToDrain: U32
                               blockMode: BlockMode
                             ) \
  opcode 0x01
