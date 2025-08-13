#ifndef FILEHANDLINGSUBTOPOLOGY_DEFS_HPP
#define FILEHANDLINGSUBTOPOLOGY_DEFS_HPP

#include "Svc/Subtopologies/FileHandling/FileHandlingConfig/FppConstantsAc.hpp"

namespace FileHandling {
// State for topology construction
struct SubtopologyState {
    // Empty - no external state needed for FileHandling subtopology
};

struct TopologyState {
    SubtopologyState fileHandling;
};
}  // namespace FileHandling

#endif
