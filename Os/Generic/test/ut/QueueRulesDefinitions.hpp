#ifndef OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#define OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#include "FpConfig.h"
#include <queue>
#include <deque>
using PriorityCompare = std::less<FwQueuePriorityType>;
constexpr FwSizeType QUEUE_MESSAGE_SIZE_UPPER_BOUND = 1024;
constexpr FwSizeType QUEUE_DEPTH_UPPER_BOUND = 100;



#endif // OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
