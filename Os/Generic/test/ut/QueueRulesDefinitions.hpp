// ======================================================================
// \title Os/Generic/test/ut/QueueRulesDefinitions.hpp
// \brief definitions for queue testing
// ======================================================================
#ifndef OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#define OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#include <deque>
#include <queue>
#include "FpConfig.h"
using PriorityCompare = std::less<FwQueuePriorityType>;
constexpr FwSizeType QUEUE_MESSAGE_SIZE_UPPER_BOUND = 1024;
constexpr FwSizeType QUEUE_DEPTH_UPPER_BOUND = 100;
constexpr bool TESTS_SUPPORT_BLOCKING = true;
#endif  // OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
