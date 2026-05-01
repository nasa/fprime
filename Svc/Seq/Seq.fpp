module Svc {
  struct SeqArgs {
    $size: FwSizeType
    buffer: [SequenceArgumentsMaxSize] U8
  } default { $size = 0, buffer = 0 }

  @ Port to request a sequence be run
  port CmdSeqIn(
                 filename: string size FileNameStringSize @< The sequence file
                 args: SeqArgs                            @< Sequence arguments
               )

  @ Port to cancel a sequence
  port CmdSeqCancel

}

