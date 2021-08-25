#include "gtest/gtest.h"
#include <Os/Queue.hpp>
#include <stdio.h>
#include <string.h>
#include <Fw/Types/Assert.hpp>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#if defined TGT_OS_TYPE_LINUX         
#include <time.h>
#endif
#if defined TGT_OS_TYPE_DARWIN    
#include <sys/time.h>
#endif

// Set this to 1 if testing a priority queue
// Set this to 0 if testing a fifo queue
#define PRIORITY_QUEUE 1

enum {
        SER_BUFFER_SIZE = 100,
        QUEUE_SIZE = 10
};

class MyTestSerializedBuffer : public Fw::SerializeBufferBase {
    public:
        MyTestSerializedBuffer() { memset(m_someBuffer, 0, sizeof(m_someBuffer)); }
        ~MyTestSerializedBuffer() {}
        NATIVE_UINT_TYPE getBuffCapacity(void) const { return sizeof(this->m_someBuffer); }
        U8* getBuffAddr(void) { return m_someBuffer;}
        const U8* getBuffAddr(void) const {return m_someBuffer;}
    private:
        U8 m_someBuffer[SER_BUFFER_SIZE];
};

Os::Queue* createTestQueue(char *name, U32 size, I32 depth) {
    Os::Queue* testQueue = new Os::Queue();
    Os::Queue::QueueStatus stat = testQueue->create(Os::QueueString(name), depth, size);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);

    // Make sure the queue is of the correct size:
    NATIVE_INT_TYPE num;
    num = testQueue->getNumMsgs(); //!< get the number of messages in the queue
    EXPECT_EQ(num, 0);
    num = testQueue->getMaxMsgs(); //!< get the maximum number of messages (high watermark)
    EXPECT_EQ(num, 0);
    num = testQueue->getQueueSize(); //!< get the queue depth (maximum number of messages queue can hold)
    EXPECT_EQ(num, depth);
    num = testQueue->getMsgSize(); //!< get the message size (maximum message size queue can hold)
    EXPECT_EQ(num, (I32) size);

    return testQueue;
}

MyTestSerializedBuffer getSendBuffer(I32 startByte) {
    Fw::SerializeStatus serStat;
    MyTestSerializedBuffer sendBuff;
    NATIVE_UINT_TYPE size = sendBuff.getBuffCapacity() - sizeof(NATIVE_UINT_TYPE);

    // Fill serialized buffer with data:
    U8 sendDataBuff[SER_BUFFER_SIZE];
    I32 count = startByte;
    for (I32 byte = 0; byte < (I32)size; byte++) {
        sendDataBuff[byte] = count;
        count++;
    }
    serStat = sendBuff.serialize(sendDataBuff, size);
    EXPECT_EQ(serStat,Fw::FW_SERIALIZE_OK);
    return sendBuff;
}

void compareBuffers(MyTestSerializedBuffer& a, MyTestSerializedBuffer& b) {
    NATIVE_UINT_TYPE size = a.getBuffCapacity() - sizeof(NATIVE_UINT_TYPE);
    Fw::SerializeStatus serStat;
    U8 aBuff[SER_BUFFER_SIZE];
    serStat = a.deserialize(aBuff, size);
    EXPECT_EQ(serStat,Fw::FW_SERIALIZE_OK);
    U8 bBuff[SER_BUFFER_SIZE];
    serStat = b.deserialize(bBuff, size);
    EXPECT_EQ(serStat,Fw::FW_SERIALIZE_OK);

    for (I32 ii = 0; ii < (I32)size; ii++) {
        if (aBuff[ii] != bBuff[ii]) {
            printf("Byte %d mismatch. A: %d B: %d\n", ii, aBuff[ii], bBuff[ii]);
            EXPECT_TRUE(0);
        }
    }
}

void fillQueue(Os::Queue* queue) {
    // Fill the queue.
    // Note this loop should only need to go to QUEUE_SIZE,
    // but the SysV queues don't actually have a size, so
    // instead we loop until we get a queue full response:
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    Os::Queue::QueueStatus stat;
    for( NATIVE_INT_TYPE ii = 0; ii < QUEUE_SIZE*100; ii++ ) {
        stat = queue->send(sendBuff, 0, Os::Queue::QUEUE_NONBLOCKING);
        if(stat == Os::Queue::QUEUE_FULL)
          break;
        EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
    }
}

void drainQueue(Os::Queue* queue) {
    // Fill the queue.
    // Note this loop should only need to go to QUEUE_SIZE,
    // but the SysV queues don't actually have a size, so
    // instead we loop until we get a queue full response:
    MyTestSerializedBuffer recvBuff;
    I32 prio;
    Os::Queue::QueueStatus stat;
    while (queue->getNumMsgs() > 0) {
        stat = queue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
        EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
    }
}

extern "C" {
    void qtest_block_receive(void);
    void qtest_nonblock_receive(void);
    void qtest_performance(void);
    void qtest_nonblock_send(void);
    void qtest_block_send(void);
    void qtest_concurrent(void);
}

// Alarm signal handler for waking up a blocked queue:
Os::Queue* globalQueue = NULL;
void alarm_send_block(int sig)
{
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    Os::Queue::QueueStatus stat;
    stat = globalQueue->send(sendBuff, 0, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
}

void alarm_send_nonblock(int sig)
{
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    Os::Queue::QueueStatus stat;
    stat = globalQueue->send(sendBuff, 0, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
}

void alarm_receive_block(int sig)
{
    MyTestSerializedBuffer recvBuff;
    Os::Queue::QueueStatus stat;
    I32 prio;
    stat = globalQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
}

void alarm_receive_nonblock(int sig)
{
    MyTestSerializedBuffer recvBuff;
    Os::Queue::QueueStatus stat;
    I32 prio;
    stat = globalQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
}

void alarm_error(int sig)
{
    // A failure here means that something blocked that was
    // not supposed to block!
    EXPECT_TRUE(0);
}

// This test verifies queue behavior for active components
// ie. non-blocking send on queue full, blocking receive on queue empty
void qtest_nonblock_send(void) {
    printf("-----------------------------\n");
    printf("-- nonblocking send test ----\n");
    printf("-----------------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ",SER_BUFFER_SIZE,QUEUE_SIZE);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);

    // TEST 1
    printf("Testing non-blocking send on queue full...\n");
    // Register an alarm handler to wake us up in case we block:
    globalQueue = testQueue;
    signal(SIGALRM, alarm_error);
    alarm(2);
    fillQueue(testQueue);
    // Make sure we get a queue full response:
    stat = testQueue->send(sendBuff, 0, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_FULL);
    // Reset the alarm:
    alarm(0);
    drainQueue(testQueue);
    globalQueue = NULL;
    printf("Passed.\n");

    delete testQueue;
    printf("Test complete.\n");
    printf("-----------------------------\n");
    printf("-----------------------------\n");
}

void qtest_block_send(void) {
    printf("-----------------------------\n");
    printf("---- blocking send test -----\n");
    printf("-----------------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ",SER_BUFFER_SIZE,QUEUE_SIZE);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);

    // TEST 1
    printf("Testing blocking send on queue full with blocking receive...\n");
    // Register an alarm handler to wake us up in case we block:
    globalQueue = testQueue;
    signal(SIGALRM, alarm_receive_block);
    alarm(2);
    fillQueue(testQueue);
    // Make sure we get a queue full response:
    stat = testQueue->send(sendBuff, 0, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
    // Reset the alarm:
    alarm(0);
    drainQueue(testQueue);
    globalQueue = NULL;
    printf("Passed.\n");

    // TEST 2
    printf("Testing blocking send on queue full nonblocking receive...\n");
    // Register an alarm handler to wake us up in case we block:
    globalQueue = testQueue;
    signal(SIGALRM, alarm_receive_nonblock);
    alarm(2);
    fillQueue(testQueue);
    // Make sure we get a queue full response:
    stat = testQueue->send(sendBuff, 0, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat, Os::Queue::QUEUE_OK);
    // Reset the alarm:
    alarm(0);
    drainQueue(testQueue);
    globalQueue = NULL;
    printf("Passed.\n");

    delete testQueue;
    printf("Test complete.\n");
    printf("-----------------------------\n");
    printf("-----------------------------\n");
}

// This test verifies queue behavior for active components
// ie. non-blocking send on queue full, blocking receive on queue empty
void qtest_block_receive(void) {
    printf("-----------------------------\n");
    printf("-- blocking receive test ----\n");
    printf("-----------------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ",SER_BUFFER_SIZE,QUEUE_SIZE);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer recvBuff;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    I32 prio; // not used

    // TEST 1
    printf("Testing successful receive after send...\n");
    stat = testQueue->send(sendBuff, 0, Os::Queue::QUEUE_NONBLOCKING);
    ASSERT_EQ(stat, Os::Queue::QUEUE_OK);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    ASSERT_EQ(stat, Os::Queue::QUEUE_OK);
    compareBuffers(sendBuff, recvBuff);
    printf("Passed.\n");

    // TEST 2
    printf("Testing blocking receive on queue empty with blocking send...\n");
    // Register an alarm handler to wake us after waiting on the queue:
    globalQueue = testQueue;
    signal(SIGALRM, alarm_send_block);
    alarm(2);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    globalQueue = NULL;
    printf("Passed.\n");

    // TEST 3
    printf("Testing blocking receive on queue empty with nonblocking send...\n");
    // Register an alarm handler to wake us after waiting on the queue:
    globalQueue = testQueue;
    signal(SIGALRM, alarm_send_nonblock);
    alarm(2);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    globalQueue = NULL;
    printf("Passed.\n");

    // TEST 5
    printf("Test send and receive with priorities...\n");
    // Send messages with mixed priorities,
    // and make sure we get back the buffers in
    // the correct order.
    I32 sendBuffStart[6] = {11, 45, 70, 123, 200, 400};
    NATIVE_INT_TYPE priorities[6] = {0, 50, 50, 99, 0, 16};
    for( I32 ii = 0; ii < 6; ii++ ) {
      // Generate a new send buffer for each enqueue:
      MyTestSerializedBuffer sendBuff2 = getSendBuffer(sendBuffStart[ii]);
      stat = testQueue->send(sendBuff2, priorities[ii], Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    }

#if PRIORITY_QUEUE
    I32 expectedSendBuffStart[6] = {123, 45, 70, 400, 11, 200};
    NATIVE_INT_TYPE expectedPriorities[6] = {99, 50, 50, 16, 0, 0};
#else
    I32 expectedSendBuffStart[6] = {11, 45, 70, 123, 200, 400};
    NATIVE_INT_TYPE expectedPriorities[6] = {0, 0, 0, 0, 0, 0};
#endif
    for( I32 ii = 0; ii < 6; ii++ ) {
      // Generate a new send buffer for each enqueue:
      MyTestSerializedBuffer expectedSendBuff2 = getSendBuffer(expectedSendBuffStart[ii]);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      EXPECT_EQ(prio,expectedPriorities[ii]);
      EXPECT_TRUE(memcmp(recvBuff.getBuffAddr(), expectedSendBuff2.getBuffAddr(), recvBuff.getBuffLength()) == 0);
    }
    printf("Passed.\n");

    delete testQueue;
    printf("Test complete.\n");
    printf("-----------------------------\n");
    printf("-----------------------------\n");
}

// This test verifies queue behavior for queued components
// ie. non-blocking send on queue full, non-blocking receive on queue empty
void qtest_nonblock_receive(void) {
    printf("-----------------------------\n");
    printf("- nonblocking receive test --\n");
    printf("-----------------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ", SER_BUFFER_SIZE, QUEUE_SIZE);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer recvBuff;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    I32 prio; // not used

    // TEST 1
    printf("Testing successful receive after send...\n");
    stat = testQueue->send(sendBuff, 0, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    compareBuffers(sendBuff, recvBuff);
    printf("Passed.\n");

    // TEST 2
    printf("Testing non-blocking receive on queue empty...\n");
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_NO_MORE_MSGS);
    printf("Passed.\n");

    // TEST 3
    printf("Test send and receive with priorities...\n");
    // Send messages with mixed priorities,
    // and make sure we get back the buffers in
    // the correct order.
    I32 sendBuffStart[6] = {11, 45, 70, 123, 200, 400};
    NATIVE_INT_TYPE priorities[6] = {0, 50, 50, 99, 0, 16};
    for( I32 ii = 0; ii < 6; ii++ ) {
      // Generate a new send buffer for each enqueue:
      MyTestSerializedBuffer sendBuff2 = getSendBuffer(sendBuffStart[ii]);
      stat = testQueue->send(sendBuff2, priorities[ii], Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    }

#if PRIORITY_QUEUE
    I32 expectedSendBuffStart[6] = {123, 45, 70, 400, 11, 200};
    NATIVE_INT_TYPE expectedPriorities[6] = {99, 50, 50, 16, 0, 0};
#else
    I32 expectedSendBuffStart[6] = {11, 45, 70, 123, 200, 400};
    NATIVE_INT_TYPE expectedPriorities[6] = {0, 0, 0, 0, 0, 0};
#endif
    for( I32 ii = 0; ii < 6; ii++ ) {
      // Generate a new send buffer for each enqueue:
      MyTestSerializedBuffer expectedSendBuff2 = getSendBuffer(expectedSendBuffStart[ii]);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      EXPECT_EQ(prio,expectedPriorities[ii]);
      EXPECT_TRUE(memcmp(recvBuff.getBuffAddr(), expectedSendBuff2.getBuffAddr(), recvBuff.getBuffLength()) == 0);
    }
    printf("Passed.\n");

    delete testQueue;
    printf("Test complete.\n");
    printf("-----------------------------\n");
    printf("-----------------------------\n");
}

// This test shows the performance of the queue:
void qtest_performance(void) {
    printf("-----------------------------\n");
    printf("---- performance test -------\n");
    printf("-----------------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ", SER_BUFFER_SIZE, 10);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer recvBuff;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    I32 prio; // not used
    F64 elapsedTime;
    I32 numIterations;

#if defined TGT_OS_TYPE_LINUX         
    timespec stime;
    timespec etime;
#endif
#if defined TGT_OS_TYPE_DARWIN    
    timeval stime;
    timeval etime;
#endif

    // TEST 1
    printf("Testing shallow queue...\n");
#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&stime);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&stime,0);
#endif
    numIterations = 1000000;
    for( NATIVE_INT_TYPE ii = 0; ii < numIterations; ii++ ) {
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    }
#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&etime);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_nsec - stime.tv_nsec))/1000000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&etime,0);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_usec - stime.tv_usec))/1000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif

    // TEST 2
    printf("Testing deep queue...\n");
    // Fill the queue up first:
    U32 count = 0;
    while(1) {
      stat = testQueue->send(sendBuff, count%4, Os::Queue::QUEUE_NONBLOCKING);
      count++;
      if(stat == Os::Queue::QUEUE_FULL)
        break;
    }
#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&stime);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&stime,0);
#endif
    numIterations = 1000000;
    for( NATIVE_INT_TYPE ii = 0; ii < numIterations; ii++ ) {
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
      stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
      EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    }
#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&etime);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_nsec - stime.tv_nsec))/1000000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&etime,0);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_usec - stime.tv_usec))/1000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif
    while(1) {
      stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_NONBLOCKING);
      if(stat == Os::Queue::QUEUE_NO_MORE_MSGS)
        break;
    }

    delete testQueue;
    printf("Test complete.\n");
    printf("-----------------------------\n");
    printf("-----------------------------\n");
}

#define NUM_THREADS 4
I32 numIterations = 50000;
void *run_task(void *ptr)
{
  Os::Queue* testQueue = (Os::Queue*) ptr;
  Os::Queue::QueueStatus stat;
  I32 prio; // not used
  MyTestSerializedBuffer recvBuff;
  MyTestSerializedBuffer sendBuff = getSendBuffer(0);
  for( NATIVE_INT_TYPE ii = 0; ii < numIterations; ii++ ) {
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->receive(recvBuff, prio, Os::Queue::QUEUE_BLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
    stat = testQueue->send(sendBuff, ii%4, Os::Queue::QUEUE_NONBLOCKING);
    EXPECT_EQ(stat,Os::Queue::QUEUE_OK);
  }
  return NULL;
}

// This test shows the concurrent performance of the queue:
void qtest_concurrent(void) {

    printf("---------------------\n");
    printf("-- concurrent test --\n");
    printf("---------------------\n");
    Os::Queue* testQueue = createTestQueue((char*)"TestQ", SER_BUFFER_SIZE, 10);
    Os::Queue::QueueStatus stat;
    MyTestSerializedBuffer recvBuff;
    MyTestSerializedBuffer sendBuff = getSendBuffer(0);
    F64 elapsedTime;

#if defined TGT_OS_TYPE_LINUX         
    timespec stime;
    timespec etime;
#endif
#if defined TGT_OS_TYPE_DARWIN    
    timeval stime;
    timeval etime;
#endif

    printf("Testing deep queue...\n");
    // Fill the queue up first:
    U32 count = 0;
    while(1) {
      stat = testQueue->send(sendBuff,count%4, Os::Queue::QUEUE_NONBLOCKING);
      count++;
      if(stat == Os::Queue::QUEUE_FULL)
        break;
    }
#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&stime);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&stime,0);
#endif

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    pthread_t thread[NUM_THREADS];

    for(U32 ii = 0; ii < NUM_THREADS; ++ii) {
      if(pthread_create(&thread[ii], NULL, run_task, testQueue)) {
        EXPECT_TRUE(0);
      }
    }
    
    for(U32 ii = 0; ii < NUM_THREADS; ++ii) {
      if(pthread_join(thread[ii], NULL)) {
        EXPECT_TRUE(0);      
      }
    }
#endif

#if defined TGT_OS_TYPE_LINUX         
    (void)clock_gettime(CLOCK_REALTIME,&etime);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_nsec - stime.tv_nsec))/1000000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif
#if defined TGT_OS_TYPE_DARWIN    
    (void)gettimeofday(&etime,0);
    elapsedTime = ((F64)(etime.tv_sec - stime.tv_sec)) + ((F64)(etime.tv_usec - stime.tv_usec))/1000000;
    printf("Time: %0.3fs (%0.3fus per)\n", elapsedTime, 1000000*elapsedTime/(F64) numIterations);
#endif

    delete testQueue;
    printf("Test complete.\n");
    printf("---------------------\n");
    printf("---------------------\n");
}
