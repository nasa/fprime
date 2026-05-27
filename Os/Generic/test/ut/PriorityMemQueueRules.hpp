// ======================================================================
// \title  Os/Generic/test/ut/PriorityMemQueueRules.hpp
// \author B. Duckett
// \brief  hpp file for rules for PriorityMemQueue tests
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

// ------------------------------------------------------------------------------------------------------
// Rule:  Create
//
// ------------------------------------------------------------------------------------------------------
struct Create : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Create() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("Create") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return !state.isCreated(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        printf("[PriorityMemQueue] Create: Creating queue\n");
        Os::QueueInterface::Status status = state.create();
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        ASSERT_TRUE(state.isCreated());
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Teardown
//
// ------------------------------------------------------------------------------------------------------
struct Teardown : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Teardown() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("Teardown") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        printf("[PriorityMemQueue] Teardown: Tearing down queue\n");
        state.teardown();
        ASSERT_FALSE(state.isCreated());
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Send
//
// ------------------------------------------------------------------------------------------------------
struct Send : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Send() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("Send") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated() && !state.isFull(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        QueueMessage msg = state.generateRandomMessage();

        // Choose a random blocking type
        Os::QueueInterface::BlockingType blockType = (STest::Random::lowerUpper(0, 1) == 0)
                                                         ? Os::QueueInterface::BlockingType::NONBLOCKING
                                                         : Os::QueueInterface::BlockingType::BLOCKING;

        printf("[PriorityMemQueue] Send: priority=%u, size=%zu, blockType=%s\n", msg.priority, msg.size,
               (blockType == Os::QueueInterface::BlockingType::NONBLOCKING) ? "NONBLOCKING" : "BLOCKING");
        Os::QueueInterface::Status status = state.send(msg, blockType);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Receive
//
// ------------------------------------------------------------------------------------------------------
struct Receive : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Receive() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("Receive") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) {
        return state.isCreated() && !state.isEmpty();
    }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        QueueMessage msg;

        // Choose a random blocking type
        Os::QueueInterface::BlockingType blockType = (STest::Random::lowerUpper(0, 1) == 0)
                                                         ? Os::QueueInterface::BlockingType::NONBLOCKING
                                                         : Os::QueueInterface::BlockingType::BLOCKING;

        printf("[PriorityMemQueue] Receive: blockType=%s\n",
               (blockType == Os::QueueInterface::BlockingType::NONBLOCKING) ? "NONBLOCKING" : "BLOCKING");
        Os::QueueInterface::Status status = state.receive(msg, blockType);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        printf("[PriorityMemQueue] Receive: priority=%u, size=%zu\n", msg.priority, msg.size);
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  EnablePriority
//
// ------------------------------------------------------------------------------------------------------
struct EnablePriority : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    EnablePriority() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("EnablePriority") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        // Choose a priority from 1-2 (0 is already enabled by default)
        FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(STest::Random::lowerUpper(1, 2));

        printf("[PriorityMemQueue] EnablePriority: priority=%u\n", priority);
        state.enablePriority(priority);

        // Verify the priority is enabled
        ASSERT_TRUE(state.m_shadow.isPriorityEnabled(priority));
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  DisablePriority
//
// ------------------------------------------------------------------------------------------------------
struct DisablePriority : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    DisablePriority() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("DisablePriority") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        // Choose a priority from 1-2 (0 should always be enabled)
        FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(STest::Random::lowerUpper(1, 2));

        printf("[PriorityMemQueue] DisablePriority: priority=%u\n", priority);
        state.disablePriority(priority);

        // Verify the priority is disabled
        ASSERT_FALSE(state.m_shadow.isPriorityEnabled(priority));
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FillQueue
//
// ------------------------------------------------------------------------------------------------------
struct FillQueue : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    FillQueue() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("FillQueue") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated() && !state.isFull(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        printf("[PriorityMemQueue] FillQueue: Filling queue to capacity\n");
        // Fill the queue until it's full
        U32 count = 0;
        while (!state.isFull()) {
            QueueMessage msg = state.generateRandomMessage();
            Os::QueueInterface::Status status = state.send(msg, Os::QueueInterface::BlockingType::NONBLOCKING);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
            count++;
        }

        printf("[PriorityMemQueue] FillQueue: Filled with %u messages\n", count);
        ASSERT_TRUE(state.isFull());
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SendFull
//
// ------------------------------------------------------------------------------------------------------
struct SendFull : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SendFull() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("SendFull") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated() && state.isFull(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        QueueMessage msg = state.generateRandomMessage();

        printf("[PriorityMemQueue] SendFull: Attempting send to full queue, priority=%u, size=%zu\n", msg.priority,
               msg.size);
        // Try to send to a full queue with non-blocking
        Os::QueueInterface::Status status = state.send(msg, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::FULL, status);
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveEmpty
//
// ------------------------------------------------------------------------------------------------------
struct ReceiveEmpty : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReceiveEmpty() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("ReceiveEmpty") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated() && state.isEmpty(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        QueueMessage msg;

        printf("[PriorityMemQueue] ReceiveEmpty: Attempting receive from empty queue\n");
        // Try to receive from an empty queue with non-blocking
        Os::QueueInterface::Status status = state.receive(msg, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);
        state.verify();
    }
};

// ------------------------------------------------------------------------------------------------------
// Rule:  PriorityOrder
//
// ------------------------------------------------------------------------------------------------------
struct PriorityOrder : public STest::Rule<Ref::Test::PriorityMemQueue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    PriorityOrder() : STest::Rule<Ref::Test::PriorityMemQueue::Tester>("PriorityOrder") {}

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Ref::Test::PriorityMemQueue::Tester& state) { return state.isCreated(); }

    //! Action
    void action(Ref::Test::PriorityMemQueue::Tester& state) {
        printf("[PriorityMemQueue] PriorityOrder: Testing priority ordering\n");
        // We'll test with just a few priorities to keep it simple
        const FwQueuePriorityType testPriorities[] = {0, 1, 2};
        const U32 numTestPriorities = FW_NUM_ARRAY_ELEMENTS(testPriorities);

        // Enable the test priorities
        for (U32 i = 0; i < numTestPriorities; ++i) {
            state.enablePriority(testPriorities[i]);
        }

        // Send messages with different priorities
        QueueMessage msgs[FW_NUM_ARRAY_ELEMENTS(testPriorities)];

        for (U32 i = 0; i < numTestPriorities; ++i) {
            msgs[i].randomize();
            msgs[i].priority = testPriorities[i];
            msgs[i].id = i;

            printf("[PriorityMemQueue] PriorityOrder: Sending priority=%u, size=%zu\n", msgs[i].priority, msgs[i].size);
            Os::QueueInterface::Status status = state.send(msgs[i], Os::QueueInterface::BlockingType::NONBLOCKING);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        }

        // Receive messages and verify they come in priority order (highest priority first)
        // Higher priority number = higher priority, so expect 2, 1, 0
        for (U32 i = 0; i < numTestPriorities; ++i) {
            QueueMessage received;
            Os::QueueInterface::Status status = state.receive(received, Os::QueueInterface::BlockingType::NONBLOCKING);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

            // Expected priority is in reverse order (highest first: 2, 1, 0)
            FwQueuePriorityType expectedPriority = testPriorities[numTestPriorities - 1 - i];
            printf("[PriorityMemQueue] PriorityOrder: Received priority=%u, expected=%u\n", received.priority,
                   expectedPriority);
            // The priority should match the expected priority
            ASSERT_EQ(expectedPriority, received.priority);
        }

        state.verify();
    }
};
