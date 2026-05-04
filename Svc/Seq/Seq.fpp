module Svc {

  @ Sequencer blocking state
  enum BlockState : U8 {
    BLOCK = 0
    NO_BLOCK = 1
  }

  @ Port to request a sequence be run
  port CmdSeqIn(
                 filename: string size 240 @< The sequence file
               )

  @ Port to cancel a sequence
  port CmdSeqCancel

}
