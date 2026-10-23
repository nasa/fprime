/*
 * CmdDispatcherImplCfg.hpp
 *
 *  Created on: May 6, 2015
 *      Author: tcanham
 */

#ifndef CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_
#define CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_

#include <Fw/FPrimeBasicTypes.hpp>

// Define configuration values for dispatcher

enum {
    CMD_DISPATCHER_DISPATCH_TABLE_SIZE = 150,  // !< The size of the table holding opcodes to dispatch
    CMD_DISPATCHER_SEQUENCER_TABLE_SIZE = 25,  // !< The size of the table holding commands in progress
};

namespace Svc {
namespace CmdDispatcherCfg {

//! Include command opcodes in events when true.
//! When false, opcode fields are set to the maximum FwOpcodeType value.
constexpr bool IncludeCommandOpcodesInEvents = true;

constexpr FwOpcodeType getEventOpcode(const FwOpcodeType opcode) {
    return IncludeCommandOpcodesInEvents ? opcode : std::numeric_limits<FwOpcodeType>::max();
}

//! Default for whether a sequence command is dispatched when the sequence tracker table is full.
//! Per-instance override: CommandDispatcherImpl::configure(bool).
constexpr bool EXECUTE_WHEN_SEQUENCE_TABLE_FULL_DEFAULT = false;

}  // namespace CmdDispatcherCfg
}  // namespace Svc

#endif /* CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_ */
