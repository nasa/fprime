module Svc {

  @ Port to request a sequence be run
  port CmdSeqIn(
                 ref filename: Fw.String @< The sequence file
               )

}
