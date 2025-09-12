@ Create a directory
async command CreateDirectory(
                               dirName: string size FileNameStringSize @< The directory to create
                             ) \
  opcode 0x00

@ Move a file
async command MoveFile(
                        sourceFileName: string size FileNameStringSize @< The source file name
                        destFileName: string size FileNameStringSize @< The destination file name
                      ) \
  opcode 0x01

@ Remove a directory, which must be empty
async command RemoveDirectory(
                               dirName: string size FileNameStringSize @< The directory to remove
                             ) \
  opcode 0x02

@ Remove a file
async command RemoveFile(
                          fileName: string size FileNameStringSize @< The file to remove
                          ignoreErrors: bool @< Ignore nonexistent files
                        ) \
  opcode 0x03

@ Perform a Linux shell command and write the output to a log file.
async command ShellCommand(
                            $command: string size 256 @< The shell command string
                            logFileName: string size FileNameStringSize @< The name of the log file
                          ) \
  opcode 0x04

@ Append 1 file's contents to the end of another.
async command AppendFile(
                          source: string size FileNameStringSize @< The name of the file to take content from
                          target: string size FileNameStringSize @< The name of the file to append to
                        ) \
  opcode 0x05

async command FileSize(
                        fileName: string size FileNameStringSize @< The file to get the size of
                      ) \
  opcode 0x06

@ List the contents of a directory
async command ListDirectory(
                              dirName: string size FileNameStringSize @< The directory to list
                            ) \
  opcode 0x07
