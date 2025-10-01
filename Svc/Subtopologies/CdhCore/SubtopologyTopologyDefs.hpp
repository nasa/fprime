#ifndef CDHCORESUBTOPOLOGY_DEFS_HPP
#define CDHCORESUBTOPOLOGY_DEFS_HPP

#include "Svc/Subtopologies/CdhCore/CdhCoreConfig/FppConstantsAc.hpp"

namespace CdhCore {
// State for topology construction
struct SubtopologyState {
    // Empty - no external state needed for CdhCore subtopology
};

struct TopologyState {
    SubtopologyState cdhCore;
};
}  // namespace CdhCore

#endif
