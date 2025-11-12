@ the number of seconds to wait before giving up
@ on a directive or command. if <= 0 or greater than U32 max, never time out.
@ accuracy of this timeout is determined by the rate group driving this
@ component. it will be rounded up
param STATEMENT_TIMEOUT_SECS: F32 default 0

@ the default value of the EXIT_ON_CMD_FAIL sequence flag
param FLAG_DEFAULT_EXIT_ON_CMD_FAIL: bool default false