module Svc {
  struct SeqArgs {
    $size: FwSizeType
    args: [SequenceArgumentsMaxSize] U8
  } default { $size = 0 }

  @ Port to request a sequence be run
  port CmdSeqIn(
                 filename: string size 240 @< The sequence file
                 args: SeqArgs @< Sequence arguments (placeholder - not currently processed)
               )

  @ Port to cancel a sequence
  port CmdSeqCancel

}

