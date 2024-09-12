

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__

#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "Os/Queue.hpp"
#include <queue>
#include <vector>

#include "QueueRulesDefinitions.hpp"

namespace Os {
namespace Test {
namespace Queue {

constexpr FwSizeType DEPTH_BOUND = 100000;

struct Tester  {
  public:
    //! Constructor
    Tester() = default;
    virtual ~Tester() = default;

    struct QueueMessage {
        U8 data[QUEUE_MESSAGE_SIZE_UPPER_BOUND];
        FwQueuePriorityType priority;
        FwSizeType size;
    };

    struct QueueMessageComparer {
        bool operator()(const QueueMessage& a, const QueueMessage& b){ return HELPER(a.priority, b.priority);}

      private:
        static const PriorityCompare HELPER;
    };

    struct QueueState {
        FwSizeType depth = 0;
        FwSizeType messageSize = 0;
        FwSizeType hwm = 0;
        static FwSizeType queues;

        bool created = false;
        std::priority_queue<QueueMessage, std::vector<QueueMessage>, QueueMessageComparer> queue;
    };
    QueueState shadow;
    Os::Queue queue;

    //! Shadow is created
    bool is_shadow_created() const {
        this->shadow_check();
        return  this->shadow.created;
    }

    //! Shadow queue is full
    bool is_shadow_full() const {
        this->shadow_check();
        return this->shadow.queue.size() == this->shadow.depth;
    }

    //! Shadow is empty
    bool is_shadow_empty() const {
        this->shadow_check();
        return this->shadow.queue.empty();
    }

    void shadow_check() const {
        EXPECT_LE(this->shadow.queue.size(), this->shadow.depth) << "Shadow queue inconsistent.";
        EXPECT_EQ(this->shadow.depth, this->queue.getDepth());
        EXPECT_EQ(this->shadow.messageSize, this->queue.getMessageSize());
        EXPECT_EQ(this->shadow.queue.size(), this->queue.getMessagesAvailable());
        EXPECT_EQ(this->shadow.hwm, this->queue.getMessageHighWaterMark());
    }

    Os::QueueInterface::Status shadow_create(FwSizeType depth, FwSizeType messageSize);

    Os::QueueInterface::Status shadow_send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, Os::QueueInterface::BlockingType blockType);

    Os::QueueInterface::Status shadow_receive(U8* destination,
                                              FwSizeType capacity,
                                              QueueInterface::BlockingType blockType,
                                              FwSizeType& actualSize,
                                              FwQueuePriorityType& priority);


  public:
#include "QueueRules.hpp"
};

}
}
}


#endif
