#ifndef DPCOMPRESISONSUBTOPOLOGY_DEFS_HPP
#define DPCOMPRESISONSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Os/FileSystem.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include "DpCompressionConfig/DpCompressionSubtopologyConfig.hpp"
#include "Svc/Subtopologies/DpCompression/DpCompressionConfig/FppConstantsAc.hpp"

namespace DpCompression {
// State for topology construction
struct SubtopologyState {
    // Empty - no external state needed for DataProducts subtopology
};

struct TopologyState {
    SubtopologyState dpCompression;
};
}  // namespace DpCompression

#endif
