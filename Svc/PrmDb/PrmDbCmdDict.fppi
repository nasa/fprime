enum Merge : U8{
    MERGE,
    RESET
}
@ Command to save parameter image to file. Uses file name passed to constructor
async command PRM_SAVE_FILE \
  opcode 0x00

@ Loads a file from storage into the staging database. The file could have selective IDs and not the whole set.
async command PRM_LOAD_FILE(
  fileName: string size FileNameStringSize @< The name of the on-board file to set parameters from
  merge: Merge @< Whether to merge or fully reset the parameter database from the file contents
  ) \
  opcode 0x01

@ Commits the backup database to become the prime (active) database
async command PRM_COMMIT_STAGED \
  opcode 0x02
