/*
 * \author: Tim Canham
 * \file:
 * \brief
 *
 * Configuration settings for the PassiveRateGroup component.
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 */

#ifndef PASSIVERATEGROUP_PASSIVERATEGROUPCFG_HPP_
#define PASSIVERATEGROUP_PASSIVERATEGROUPCFG_HPP_

namespace Svc {

// Enable per-component execution time measurement within a rate group cycle.
// When 0, timing code is eliminated at compile time (see #5293).
#define PASSIVE_RATE_GROUP_ENABLE_COMPONENT_TIMING 0

}  // namespace Svc

#endif /* PASSIVERATEGROUP_PASSIVERATEGROUPCFG_HPP_ */
