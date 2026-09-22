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

//! When true, execute sequence command even if the sequence tracker table is full.
//! When false, do not execute the sequence command if the sequence tracker table is full.
//! The 'CMD_DISPATCHER_EXECUTE_WHEN_TRACKER_FULL' macro exists only so the unit tests can build both conditions. It is not a supported deployment knob.
#ifndef CMD_DISPATCHER_EXECUTE_WHEN_TRACKER_FULL
#define CMD_DISPATCHER_EXECUTE_WHEN_TRACKER_FULL false
#endif
constexpr bool ExecuteCommandWhenSequenceTrackerTableIsFull = CMD_DISPATCHER_EXECUTE_WHEN_TRACKER_FULL;

}  // namespace CmdDispatcherCfg
}  // namespace Svc

#endif /* CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_ */
