@ The Buffer Logger closed a log file
event BL_LogFileClosed(
                        file: string size 256 @< The file
                      ) \
  severity diagnostic \
  id 0x00 \
  format "File {} closed"

@ The Buffer Logger encountered an error opening a log file
event BL_LogFileOpenError(
                           errornum: U32 @< The error number returned from the open operation
                           file: string size 256 @< The file
                         ) \
  severity warning high \
  id 0x01 \
  format "Error {} opening file {}"

@ The Buffer Logger encountered an error writing a validation file
event BL_LogFileValidationError(
                                 validationFile: string size 256 @< The validation file
                                 status: U32 @< The Os::Validate::Status return
                               ) \
  severity warning high \
  id 0x02 \
  format "Failed creating validation file {} with status {}"

@ The Buffer Logger encountered an error writing to a log file
event BL_LogFileWriteError(
                            errornum: U32 @< The error number returned from the write operation
                            bytesWritten: U32 @< The number of bytes successfully written
                            bytesToWrite: U32 @< The number of bytes attempted
                            file: string size 256 @< The file
                          ) \
  severity warning high \
  id 0x03 \
  format "Error {} while writing {} of {} bytes to {}"

@ Buffer logger was activated
event BL_Activated \
  severity activity low \
  id 0x04 \
  format "Buffer logger was activated"

@ Buffer logger was deactivated
event BL_Deactivated \
  severity activity low \
  id 0x05 \
  format "Buffer logger was deactivated"

@ No log file open command was received by BufferLogger
event BL_NoLogFileOpenInitError \
  severity warning high \
  id 0x06 \
  format "No log file open command"
