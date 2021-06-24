@ Read a named file off the disk. Divide it into packets and send the packets for transmission to the ground.
async command SendFile(
                        sourceFileName: string size 100 @< The name of the on-board file to send
                        destFileName: string size 100 @< The name of the destination file on the ground
                      ) \
  opcode 0x00

@ Cancel the downlink in progress, if any
async command Cancel \
  opcode 0x01

@ Read a named file off the disk from a starting position. Divide it into packets and send the packets for transmission to the ground.
async command SendPartial(
                           sourceFileName: string size 100 @< The name of the on-board file to send
                           destFileName: string size 100 @< The name of the destination file on the ground
                           startOffset: U32 @< Starting offset of the source file
                           length: U32 @< Number of bytes to send from starting offset. Length of 0 implies until the end of the file
                         ) \
  opcode 0x02
