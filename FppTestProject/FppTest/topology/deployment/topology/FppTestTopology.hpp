// ======================================================================
// \title  FppTestTopology.hpp
// \author bocchino
// \brief Header file containing the topology instantiation definitions
// ======================================================================

#ifndef MICROPORTS_MICROPORTSTOPOLOGY_HPP
#define MICROPORTS_MICROPORTSTOPOLOGY_HPP

#include "Fw/Time/TimeInterval.hpp"
#include "FppTest/topology/deployment/topology/FppTestTopologyDefs.hpp"

namespace FppTest {

void setupTopology(const TopologyState& state);
void teardownTopology(const TopologyState& state);

}  // namespace FppTest

#endif
