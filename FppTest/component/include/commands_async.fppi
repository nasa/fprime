@ An async command with no params 
async command CMD_ASYNC

@ A command with priority
async command CMD_PRIORITY opcode 0x20 priority 10

@ A command with params and priority
async command CMD_PARAMS_PRIORITY(
  u32: U32
) priority 20

@ A command with queue full behavior
async command CMD_DROP drop

@ A command with params, priority, and queue full behavior
async command CMD_PARAMS_PRIORITY_DROP(
  u32: U32
) priority 30 drop
