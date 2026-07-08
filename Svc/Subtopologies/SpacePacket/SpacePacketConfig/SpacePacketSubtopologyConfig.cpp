#include "SpacePacketSubtopologyConfig.hpp"

namespace SpacePacket {
namespace Allocation {
// This instance can be changed to use a different allocator in the SpacePacket Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace SpacePacket
