/*
 * PassiveRateGroupConfig.hpp:
 *
 * Configuration settings for the passive rate group component.
 */

#ifndef SVC_PASSIVE_RATE_GROUP_CFG_HPP_
#define SVC_PASSIVE_RATE_GROUP_CFG_HPP_

// Enable runtime measurement of per-output port execution time on the Svc.PassiveRateGroup component
// This will take an Os::RawTime measurement before and after each RateGroupMemberOut port is invoked
// and may incure some overall overhead on the execution time of the rate group.
#define SVC_PASSIVE_RATE_GROUP_PORT_CYCLE_TIME (0)

#endif
