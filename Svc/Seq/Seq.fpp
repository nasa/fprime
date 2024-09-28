module Svc {

  @ Port to request a sequence be run
  port CmdSeqIn(
                 filename: string size 240 @< The sequence file
               )

  @ Port to cancel a sequence
  port CmdSeqCancel

}

