@ the number of seconds to wait before giving up
@ on a directive or command. if <= 0 or greater than U32 max, never time out.
@ accuracy of this timeout is determined by the rate group driving this
@ component. it will be rounded up
param STATEMENT_TIMEOUT_SECS: F32 default 0

@ the base directory relative to which sequence file paths are resolved.
@ if empty (the default), paths are used as-is and resolved relative to the
@ process's current working directory.
param SEQ_BASE_DIR: string size FileNameStringSize default ""