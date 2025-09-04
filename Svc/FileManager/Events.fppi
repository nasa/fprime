@ An error occurred while attempting to create a directory
event DirectoryCreateError(
                            dirName: string size FileNameStringSize @< The name of the directory
                            status: U32 @< The error status
                          ) \
  severity warning high \
  id 0x00 \
  format "Could not create directory {}, returned status {}"

@ An error occurred while attempting to remove a directory
event DirectoryRemoveError(
                            dirName: string size FileNameStringSize @< The name of the directory
                            status: U32 @< The error status
                          ) \
  severity warning high \
  id 0x01 \
  format "Could not remove directory {}, returned status {}"

@ An error occurred while attempting to move a file
event FileMoveError(
                     sourceFileName: string size FileNameStringSize @< The name of the source file
                     destFileName: string size FileNameStringSize @< The name of the destination file
                     status: U32 @< The error status
                   ) \
  severity warning high \
  id 0x02 \
  format "Could not move file {} to file {}, returned status {}"

@ An error occurred while attempting to remove a file
event FileRemoveError(
                       fileName: string size FileNameStringSize @< The name of the file
                       status: U32 @< The error status
                     ) \
  severity warning high \
  id 0x03 \
  format "Could not remove file {}, returned status {}"

@ The File System component executed a shell command that returned status non-zero
event ShellCommandFailed(
                          $command: string size 256 @< The command string
                          status: U32 @< The status code
                        ) \
  severity warning high \
  id 0x04 \
  format "Shell command {} failed with status {}"

@ The File System component returned status non-zero when trying to append 2 files together
event AppendFileFailed(
                        source: string size FileNameStringSize @< The name of the file being read from
                        target: string size FileNameStringSize @< The name of the file to append to
                        status: U32 @< The error status
                      ) \
  severity warning high \
  id 0x05 \
  format "Appending {} onto {} failed with status {}"

@ The File System component appended 2 files without error
event AppendFileSucceeded(
                           source: string size FileNameStringSize @< The name of the file being read from
                           target: string size FileNameStringSize @< The name of the file to append to
                         ) \
  severity activity high \
  id 0x06 \
  format "Appended {} to the end of {}"

@ The File System component executed a shell command that returned status zero
event ShellCommandSucceeded(
                             $command: string size FileNameStringSize @< The command string
                           ) \
  severity activity high \
  id 0x07 \
  format "Shell command {} succeeded"

@ The File System component created a new directory without error
event CreateDirectorySucceeded(
                                dirName: string size FileNameStringSize @< The name of the directory
                              ) \
  severity activity high \
  id 0x08 \
  format "Created directory {} successfully"

@ The File System component deleted and existing directory without error
event RemoveDirectorySucceeded(
                                dirName: string size FileNameStringSize @< The name of the directory
                              ) \
  severity activity high \
  id 0x09 \
  format "Removed directory {} successfully"

@ The File System component moved a file to a new location without error
event MoveFileSucceeded(
                         sourceFileName: string size FileNameStringSize @< The name of the source file
                         destFileName: string size FileNameStringSize @< The name of the destination file
                       ) \
  severity activity high \
  id 0x0A \
  format "Moved file {} to file {} successfully"

@ The File System component deleted an existing file without error
event RemoveFileSucceeded(
                           fileName: string size FileNameStringSize @< The name of the file
                         ) \
  severity activity high \
  id 0x0B \
  format "Removed file {} successfully"

@ The File System component appended 2 files without error
event AppendFileStarted(
                         source: string size FileNameStringSize @< The name of the file being read from
                         target: string size FileNameStringSize @< The name of the file to append to
                       ) \
  severity activity high \
  id 0x0C \
  format "Appending file {} to the end of {}..."

@ The File System component began executing a shell command
event ShellCommandStarted(
                           $command: string size 256 @< The command string
                         ) \
  severity activity high \
  id 0x0D \
  format "Running shell command {}..."

@ The File System component began creating a new directory
event CreateDirectoryStarted(
                              dirName: string size FileNameStringSize @< The name of the directory
                            ) \
  severity activity high \
  id 0x0E \
  format "Creating directory {}..."

@ The File System component began deleting a directory
event RemoveDirectoryStarted(
                              dirName: string size FileNameStringSize @< The name of the directory
                            ) \
  severity activity high \
  id 0x0F \
  format "Removing directory {}..."

@ The File System component began moving a file to a new location
event MoveFileStarted(
                       sourceFileName: string size FileNameStringSize @< The name of the source file
                       destFileName: string size FileNameStringSize @< The name of the destination file
                     ) \
  severity activity high \
  id 0x10 \
  format "Moving file {} to file {}..."

@ The File System component began deleting an existing file
event RemoveFileStarted(
                         fileName: string size FileNameStringSize @< The name of the file
                       ) \
  severity activity high \
  id 0x11 \
  format "Removing file {}..."

@ File size response
event FileSizeSucceeded(
                         fileName: string size FileNameStringSize @< The name of the file
                         $size: FwSizeType @< The size of the file in bytes
                       ) \
  severity activity high \
  id 0x12 \
  format "The size of file {} is {} B"

@ Failed to get file size
event FileSizeError(
                      fileName: string size FileNameStringSize @< The name of the file
                      status: U32 @< The error status
                    ) \
  severity warning high \
  id 0x13 \
  format "Failed to get the size of file {}, returned status {}"

@ Checking file size
event FileSizeStarted(
                       fileName: string size FileNameStringSize @< The name of the file
                     ) \
  severity activity high \
  id 0x14 \
  format "Checking size of file {}..."

@ Starting directory listing
event ListDirectoryStarted(
                            dirName: string size FileNameStringSize @< The name of the directory
                          ) \
  severity activity high \
  id 0x15 \
  format "Listing contents of directory {}..."

@ Directory listing completed successfully  
event ListDirectorySucceeded(
                              dirName: string size FileNameStringSize @< The name of the directory
                              fileCount: U32 @< The number of files found
                            ) \
  severity activity high \
  id 0x16 \
  format "Directory {} contains {} files"

@ Failed to list directory
event ListDirectoryError(
                          dirName: string size FileNameStringSize @< The name of the directory
                          status: U32 @< The error status
                        ) \
  severity warning high \
  id 0x17 \
  format "Failed to list directory {}, returned status {}"

@ Directory listing file entry
event DirectoryListing(
                        dirName: string size FileNameStringSize @< The name of the directory
                        fileName: string size FileNameStringSize @< The name of the file
                        fileSize: FwSizeType @< The size of the file in bytes
                      ) \
  severity activity high \
  id 0x18 \
  format "Directory {}: {} ({} bytes)"

@ Directory listing subdirectory entry
event DirectoryListingSubdir(
                        dirName: string size FileNameStringSize @< The name of the directory
                        subdirName: string size FileNameStringSize @< The name of the subdirectory
                      ) \
  severity activity high \
  id 0x19 \
  format "Directory {}: {}"
