@ The ComLogger encountered an error opening a file
event FileOpenError(
                     errornum: U32 @< The error number returned from open file
                     file: string size 240 @< The file
                   ) \
  severity warning high \
  id 0x00 \
  format "Error {} opening file {}"

@ The ComLogger encountered an error writing to a file
event FileWriteError(
                      errornum: U32 @< The error number returned from write file
                      bytesWritten: U32 @< The number of bytes successfully written to file
                      bytesToWrite: U32 @< The number of bytes attempted to write to file
                      file: string size 240 @< The file
                    ) \
  severity warning high \
  id 0x01 \
  format "Error {} while writing {} of {} bytes to {}"

@ The ComLogger encountered an error writing the validation file
event FileValidationError(
                           validationFile: string size 240 @< The validation file
                           file: string size 240 @< The file
                           status: U32 @< The Os::Validate::Status return
                         ) \
  severity warning low \
  id 0x02 \
  format "The ComLogger failed to create a validation file {} for {} with error {}."

@ The ComLogger successfully closed a file on command.
event FileClosed(
                  file: string size 240 @< The file
                ) \
  severity diagnostic \
  id 0x03 \
  format "File {} closed successfully."

event FileNotInitialized \
  severity warning low \
  id 0x04 \
  format "Could not open ComLogger file. File not initialized" \
  throttle 5
