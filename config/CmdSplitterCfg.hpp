/*
 * CmdSplitterCfg.hpp:
 *
 * Used to configure the Svc::CmdSplitter component's remote opcode base.
 *
 */

#ifndef CONFIG_CMD_SPLITTER_CFG_HPP_
#define CONFIG_CMD_SPLITTER_CFG_HPP_

#include <FpConfig.hpp>

namespace Svc {
//!< Base value for remote opcodes used by Svc::CmdSplitter
static const FwOpcodeType CMD_SPLITTER_REMOTE_OPCODE_BASE = 0x10000000;
};

#endif /* CONFIG_CMD_SPLITTER_CFG_HPP_ */
