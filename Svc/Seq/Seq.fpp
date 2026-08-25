module Svc {
  struct SeqArgs {
    $size: FwSizeType
    buffer: [SequenceArgumentsMaxSize] U8
  } default { $size = 0, buffer = 0 }

  @ Sequencer blocking state
  enum BlockState: U8 {
    BLOCK    = 0
    NO_BLOCK = 1
  }

  @ Port to request a sequence be run
  port CmdSeqIn(
    filename: string size FileNameStringSize @< The sequence file
    args: SeqArgs                            @< Sequence arguments
  )

  @ Port to cancel a sequence
  port CmdSeqCancel

  @ Port to request a sequence file be loaded (and staged) under a module name
  port SeqLoadIn(
    filename: string size FileNameStringSize      @< Sequence file to load
    name: string size SequenceModuleNameSize      @< Module name to load the sequence under (empty for a single unnamed module)
  )

  port SeqInvokeIn(
    name: string size FileNameStringSize @< Name of sequence to invoke
    args: SeqArgs                        @< Sequence Arguments
  )
}
