// ======================================================================
// \title  RecordedComSubtopologyConfig.cpp
// \author autocoded
// \brief  cpp file for RecordedCom subtopology configuration
//
// ======================================================================

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/Subtopologies/RecordedCom/RecordedComConfig/RecordedComSubtopologyConfig.hpp>

namespace RecordedCom {
namespace Allocation {
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace RecordedCom
