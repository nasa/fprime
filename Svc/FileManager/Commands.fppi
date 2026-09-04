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

@ Append 1 file's contents to the end of another.
async command AppendFile(
                          source: string size FileNameStringSize @< The name of the file to take content from
                          target: string size FileNameStringSize @< The name of the file to append to
                        ) \
  opcode 0x05

@ Get the size of a file
async command FileSize(
                        fileName: string size FileNameStringSize @< The file to get the size of
                      ) \
  opcode 0x06

@ List the contents of a directory
async command ListDirectory(
                              dirName: string size FileNameStringSize @< The directory to list
                            ) \
  opcode 0x07


@ Calculate the CRC of a file
async command CalculateCrc(
                              filename: string size FileNameStringSize @< The file to CRC
                            ) \
  opcode 0x08

@ Generate a data product from a file, split into chunks.
@ Each chunk is emitted as a FileChunkHeaderRecord (file name, offset, data size)
@ followed by a FileChunkDataRecord containing the chunk bytes, so ground tools
@ can reassemble the original file regardless of DP buffer sizing.
async command GenerateDp(
                          fileName: string size FileNameStringSize @< The file to package as a data product
                          chunkSize: U32 @< The maximum number of file bytes per chunk
                          beginOffset: U64 @< The offset in the file at which to start
                          endOffset: U64 @< The offset in the file at which to stop, exclusive; 0 means the end of the file
                          $priority: U32 @< The container priority; 0 means the configured default
                          mode: Svc.FileManager.GenerateDpMode @< Whether to pace the chunks with the rate group or emit them immediately
                        ) \
  opcode 0x09
