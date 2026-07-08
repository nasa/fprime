#ifndef CCSDSFRAMINGSUBTOPOLOGY_DEFS_HPP
#define CCSDSFRAMINGSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp>
#include "CcsdsFramingConfig/CcsdsFramingSubtopologyConfig.hpp"
#include "Svc/Subtopologies/CcsdsFraming/CcsdsFramingConfig/FppConstantsAc.hpp"

namespace CcsdsFraming {
struct SubtopologyState {
    // Empty - no external state needed for CcsdsFraming subtopology
};

struct TopologyState {
    SubtopologyState ccsdsFraming;
};
}  // namespace CcsdsFraming

#endif
