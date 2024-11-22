@ Sequence file was successfully loaded.
event CS_SequenceLoaded(
                         fileName: string size 60 @< The name of the sequence file
                       ) \
  severity activity low \
  id 0 \
  format "Loaded sequence {}"

@ A command sequence was successfully canceled.
event CS_SequenceCanceled(
                           fileName: string size 60 @< The name of the sequence file
                         ) \
  severity activity high \
  id 1 \
  format "Sequence file {} canceled"

@ The Sequence File Loader could not read the sequence file.
event CS_FileReadError(
                        fileName: string size 60 @< The name of the sequence file
                      ) \
  severity warning high \
  id 2 \
  format "Error reading sequence file {}"

@ The sequence file format was invalid.
event CS_FileInvalid(
                      fileName: string size 60 @< The name of the sequence file
                      stage: FileReadStage @< The read stage
                      error: I32 @< The error code
                    ) \
  severity warning high \
  id 3 \
  format "Sequence file {} invalid. Stage: {} Error: {}"

@ The format of a command record was invalid.
event CS_RecordInvalid(
                        fileName: string size 60 @< The name of the sequence file
                        recordNumber: U32 @< The record number
                        error: I32 @< The error code
                      ) \
  severity warning high \
  id 4 \
  format "Sequence file {}: Record {} invalid. Err: {}"

@ The sequence file was too large.
event CS_FileSizeError(
                        fileName: string size 60 @< The name of the sequence file
                        $size: U32 @< Invalid size
                      ) \
  severity warning high \
  id 5 \
  format "Sequence file {} too large. Size: {}"

@ The sequence file was not found
event CS_FileNotFound(
                       fileName: string size 60 @< The sequence file
                     ) \
  severity warning high \
  id 6 \
  format "Sequence file {} not found."


@ The sequence file validation failed
event CS_FileCrcFailure(
                         fileName: string size 60 @< The sequence file
                         storedCRC: U32 @< The CRC stored in the file
                         computedCRC: U32 @< The CRC computed over the file
                       ) \
  severity warning high \
  id 7 \
  format "Sequence file {} had invalid CRC. Stored 0x{x}, Computed 0x{x}."

@ The Command Sequencer issued a command and received a success status in return.
event CS_CommandComplete(
                          fileName: string size 60 @< The name of the sequence file
                          recordNumber: U32 @< The record number of the command
                          opCode: U32 @< The command opcode
                        ) \
  severity activity low \
  id 8 \
  format "Sequence file {}: Command {} (opcode {}) complete"

@ A command sequence successfully completed.
event CS_SequenceComplete(
                           fileName: string size 60 @< The name of the sequence file
                         ) \
  severity activity high \
  id 9 \
  format "Sequence file {} complete"

@ The Command Sequencer issued a command and received an error status in return.
event CS_CommandError(
                       fileName: string size 60 @< The name of the sequence file
                       recordNumber: U32 @< The record number
                       opCode: U32 @< The opcode
                       errorStatus: U32 @< The error status
                     ) \
  severity warning high \
  id 10 \
  format "Sequence file {}: Command {} (opcode {}) completed with error {}"

@ The Command Sequencer received a command that was invalid for its current mode.
event CS_InvalidMode \
  severity warning high \
  id 11 \
  format "Invalid mode"

@ Number of records in header doesn't match number in file
event CS_RecordMismatch(
                         fileName: string size 60 @< The name of the sequence file
                         header_records: U32 @< The number of records in the header
                         extra_bytes: U32 @< The number of bytes beyond last record
                       ) \
  severity warning high \
  id 12 \
  format "Sequence file {} header records mismatch: {} in header, found {} extra bytes."

@ The running time base doesn't match the time base in the sequence files
event CS_TimeBaseMismatch(
                           fileName: string size 60 @< The name of the sequence file
                           time_base: U16 @< The current time
                           seq_time_base: U16 @< The sequence time base
                         ) \
  severity warning high \
  id 13 \
  format "Sequence file {}: Current time base doesn't match sequence time: base: {} seq: {}"

@ The running time base doesn't match the time base in the sequence files
event CS_TimeContextMismatch(
                              fileName: string size 60 @< The name of the sequence file
                              currTimeBase: U8 @< The current time base
                              seqTimeBase: U8 @< The sequence time base
                            ) \
  severity warning high \
  id 14 \
  format "Sequence file {}: Current time context doesn't match sequence context: base: {} seq: {}"

@ A local port request to run a sequence was started
event CS_PortSequenceStarted(
                              filename: string size 60 @< The sequence file
                            ) \
  severity activity high \
  id 15 \
  format "Local request for sequence {} started."

@ A command status came back when no sequence was running
event CS_UnexpectedCompletion(
                               $opcode: U32 @< The reported opcode
                             ) \
  severity warning high \
  id 16 \
  format "Command complete status received while no sequences active. Opcode: {}"

@ Switched step mode
event CS_ModeSwitched(
                       mode: SeqMode @< The new mode
                     ) \
  severity activity high \
  id 17 \
  format "Sequencer switched to {} step mode"

@ A sequence related command came with no active sequence
event CS_NoSequenceActive \
  severity warning low \
  id 18 \
  format "No sequence active."

@ A sequence passed validation
event CS_SequenceValid(
                        filename: string size 60 @< The sequence file
                      ) \
  severity activity high \
  id 19 \
  format "Sequence {} is valid."

@ A sequence passed validation
event CS_SequenceTimeout(
                          filename: string size 60 @< The sequence file
                          $command: U32 @< The command that timed out
                        ) \
  severity warning high \
  id 20 \
  format "Sequence {} timed out on command {}"

@ A command in a sequence was stepped through
event CS_CmdStepped(
                     filename: string size 60 @< The sequence file
                     $command: U32 @< The command that was stepped
                   ) \
  severity activity high \
  id 21 \
  format "Sequence {} command {} stepped"

@ A manual sequence was started
event CS_CmdStarted(
                     filename: string size 60 @< The sequence file
                   ) \
  severity activity high \
  id 22 \
  format "Sequence {} started"

@ Wait for the current running sequence file complete
event CS_JoinWaiting(
                      filename: string size 60 @< The sequence file
                      recordNumber: U32 @< The record number
                      opCode: U32 @< The opcode
                    ) \
  severity activity high \
  id 23 \
  format "Start waiting for sequence file {}: Command {} (opcode {}) to complete"

@ Cannot run new sequence when current sequence file is still running.
event CS_JoinWaitingNotComplete() \
  severity warning high \
  id 24 \
  format "Still waiting for sequence file to complete"

event CS_NoRecords(
                      fileName: string size 60 @< The name of the sequence file
                    ) \
  severity warning low \
  id 25 \
  format "Sequence file {} has no records. Ignoring."