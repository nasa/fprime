/*
 * CmdDispatcherImplCfg.hpp
 *
 *  Created on: May 6, 2015
 *      Author: tcanham
 */

#ifndef CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_
#define CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_

// Define configuration values for dispatcher

enum {
    CMD_DISPATCHER_DISPATCH_TABLE_SIZE = 100, // !< The size of the table holding opcodes to dispatch
    CMD_DISPATCHER_SEQUENCER_TABLE_SIZE = 25, // !< The size of the table holding commands in progress
};

// Toggles whether events are triggered during command opcode registration. Disabling this can be
// useful for memory-constrained systems with limited downlink buffer space as the events can crowd
// out more useful events during initialization.
#ifndef CMD_DISPATCHER_ENABLE_OPCODE_REGISTER_EVENTS
#define CMD_DISPATCHER_ENABLE_OPCODE_REGISTER_EVENTS \
    1  //!< Indicates whether or not command opcode registration events are triggered
#endif

#endif /* CMDDISPATCHER_COMMANDDISPATCHERIMPLCFG_HPP_ */
