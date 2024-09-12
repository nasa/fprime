

#include "CommonTests.hpp"
#include "Fw/Types/String.hpp"

struct PickedMessage {
    FwSizeType size;
    FwQueuePriorityType priority;
    U8* sent;
    U8 received[QUEUE_MESSAGE_SIZE_UPPER_BOUND];
};

PickedMessage pick_message(FwSizeType max_size) {
    PickedMessage message;

    message.size = STest::Random::lowerUpper(1, max_size);
    message.priority = STest::Random::lowerUpper(0, std::numeric_limits<U32>::max());

    message.sent = new U8[message.size];
    for (FwSizeType i = 0; i < message.size; i++) {
        message.sent[i] = STest::Random::lowerUpper(0, std::numeric_limits<U8>::max());
    }
    return message;
}

void check_received(PickedMessage& message, PickedMessage& test) {
    EXPECT_EQ(message.size, test.size);
    EXPECT_EQ(message.priority, test.priority);
    for (FwSizeType i = 0; i < message.size; i++) {
        ASSERT_EQ(message.received[i], test.received[i]) << "at index " << i;
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Create
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Create::Create() : STest::Rule<Os::Test::Queue::Tester>("Create") {}

bool Os::Test::Queue::Tester::Create::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::Queue::Tester::Create::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    const bool created = state.is_shadow_created();
    Fw::String name("nAmE");
    FwSizeType depth = STest::Random::lowerUpper(1, QUEUE_DEPTH_UPPER_BOUND);
    FwSizeType messageSize = STest::Random::lowerUpper(1, QUEUE_MESSAGE_SIZE_UPPER_BOUND);
    QueueInterface::Status status = state.shadow_create(depth, messageSize);
    QueueInterface::Status test_status = state.queue.create(name, depth, messageSize);
    ASSERT_EQ(status, created ? QueueInterface::Status::ALREADY_CREATED : QueueInterface::Status::OP_OK);
    ASSERT_EQ(status, test_status);
    ASSERT_EQ(name, state.queue.getName());
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Send
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Send::Send() : STest::Rule<Os::Test::Queue::Tester>("Send") {}

bool Os::Test::Queue::Tester::Send::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created;
}

void Os::Test::Queue::Tester::Send::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    PickedMessage pick = pick_message(state.shadow.messageSize);
    const bool is_full = state.is_shadow_full();
    QueueInterface::Status status = state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
    QueueInterface::Status test_status = state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);

    ASSERT_EQ(status, is_full ? QueueInterface::Status::FULL : QueueInterface::Status::OP_OK);
    ASSERT_EQ(test_status, status);
    state.shadow_check();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Receive
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Queue::Tester::Receive::Receive() : STest::Rule<Os::Test::Queue::Tester>("Receive") {}

bool Os::Test::Queue::Tester::Receive::precondition(const Os::Test::Queue::Tester& state  //!< The test state
) {
    return state.shadow.created;
}

void Os::Test::Queue::Tester::Receive::action(Os::Test::Queue::Tester& state  //!< The test state
) {
    PickedMessage message;
    PickedMessage test;
    const bool is_empty = state.is_shadow_empty();

        QueueInterface::Status status = state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                         QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

    QueueInterface::Status test_status = state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND, QueueInterface::BlockingType::NONBLOCKING, test.size, test.priority);
    ASSERT_EQ(status, is_empty ? QueueInterface::Status::EMPTY : QueueInterface::Status::OP_OK);
    ASSERT_EQ(status, test_status);
    if (status == QueueInterface::Status::OP_OK) {
        check_received(message, test);
    }
}
    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  Overflow
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Test::Queue::Tester::Overflow::Overflow() :
        STest::Rule<Os::Test::Queue::Tester>("Overflow")
  {
  }


  bool Os::Test::Queue::Tester::Overflow::precondition(
            const Os::Test::Queue::Tester& state //!< The test state
        ) 
  {
      return state.is_shadow_created();
  }

  
  void Os::Test::Queue::Tester::Overflow::action(
            Os::Test::Queue::Tester& state //!< The test state
        ) 
  {
      while (not state.is_shadow_full()) {
          PickedMessage pick = pick_message(state.shadow.messageSize);
          QueueInterface::Status status =
              state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
          QueueInterface::Status test_status =
              state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
          ASSERT_EQ(status, QueueInterface::Status::OP_OK);
          ASSERT_EQ(status, test_status);
      }
      PickedMessage pick = pick_message(state.shadow.messageSize);
      QueueInterface::Status status =
          state.shadow_send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
      QueueInterface::Status test_status =
          state.queue.send(pick.sent, pick.size, pick.priority, QueueInterface::BlockingType::NONBLOCKING);
      ASSERT_EQ(status, QueueInterface::Status::FULL);
      ASSERT_EQ(status, test_status);
      state.shadow_check();
  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  Underflow
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Test::Queue::Tester::Underflow::Underflow() :
        STest::Rule<Os::Test::Queue::Tester>("Underflow")
  {
  }


  bool Os::Test::Queue::Tester::Underflow::precondition(
            const Os::Test::Queue::Tester& state //!< The test state
        ) 
  {
      return state.is_shadow_created();
  }

  
  void Os::Test::Queue::Tester::Underflow::action(
            Os::Test::Queue::Tester& state //!< The test state
        ) 
  {
      PickedMessage message;
      PickedMessage test;
      while (not state.is_shadow_empty()) {
          QueueInterface::Status status =
              state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                                   QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

          QueueInterface::Status test_status =
              state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                                  QueueInterface::BlockingType::NONBLOCKING, test.size, test.priority);
          ASSERT_EQ(status, QueueInterface::Status::OP_OK);
          ASSERT_EQ(status, test_status);
          check_received(message, test);
      }
      QueueInterface::Status status =
          state.shadow_receive(message.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                               QueueInterface::BlockingType::NONBLOCKING, message.size, message.priority);

      QueueInterface::Status test_status =
          state.queue.receive(test.received, QUEUE_MESSAGE_SIZE_UPPER_BOUND,
                              QueueInterface::BlockingType::NONBLOCKING, test.size, test.priority);
      ASSERT_EQ(status, QueueInterface::Status::EMPTY);
      ASSERT_EQ(status, test_status);
      state.shadow_check();
  }

