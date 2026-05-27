#ifndef FILEHANDLINGCFDPSUBTOPOLOGY_DEFS_HPP
#define FILEHANDLINGCFDPSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include "Svc/Subtopologies/FileHandlingCfdp/FileHandlingCfdpConfig/FppConstantsAc.hpp"
#include "Svc/Subtopologies/FileHandlingCfdp/FileHandlingCfdpConfig/FileHandlingCfdpSubtopologyConfig.hpp"

namespace FileHandlingCfdp {
struct SubtopologyState {
    // Empty - no external state needed for FileHandlingCfdp subtopology
};

struct TopologyState {
    SubtopologyState fileHandlingCfdp;
};
}  // namespace FileHandlingCfdp

#endif
