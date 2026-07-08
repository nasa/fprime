#ifndef COMCCSDSSUBTOPOLOGY_DEFS_HPP
#define COMCCSDSSUBTOPOLOGY_DEFS_HPP

#include "Svc/Subtopologies/CcsdsFraming/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsds/ComCcsdsConfig/FppConstantsAc.hpp"
#include "Svc/Subtopologies/SpacePacket/SubtopologyTopologyDefs.hpp"

namespace ComCcsds {
struct SubtopologyState {
    // Empty - no external state needed for ComCcsds subtopology
};

struct TopologyState {
    SubtopologyState comCcsds;
};
}  // namespace ComCcsds

#endif
