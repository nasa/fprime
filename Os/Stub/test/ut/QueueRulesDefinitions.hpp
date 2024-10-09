#ifndef OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#define OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
#include "Os/Stub/test/Queue.hpp"

using PriorityCompare = std::greater<FwQueuePriorityType>;
constexpr FwSizeType QUEUE_MESSAGE_SIZE_UPPER_BOUND = Os::Stub::Queue::Test::STUB_QUEUE_TEST_MESSAGE_MAX_SIZE;
constexpr FwSizeType QUEUE_DEPTH_UPPER_BOUND = 100;
constexpr bool TESTS_SUPPORT_BLOCKING = false;



#endif // OS_STUB_TEST_UT_QUEUE_RULES_DEFINITIONS
