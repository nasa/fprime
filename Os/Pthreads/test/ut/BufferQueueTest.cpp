#include "Os/Pthreads/BufferQueue.hpp"
#include <Fw/Types/Assert.hpp>
#include <stdio.h>
#include <string.h>

using namespace Os;

// Set this to 1 if testing a priority queue
// Set this to 0 if testing a fifo queue
#define PRIORITY_QUEUE 1

#define DEPTH 5
#define MSG_SIZE 3
int main() {
  printf("Creating queue.\n");
  bool ret;
  NATIVE_UINT_TYPE size;
  NATIVE_UINT_TYPE count;
  NATIVE_UINT_TYPE maxCount;
  NATIVE_INT_TYPE priority = 0;
  BufferQueue queue;
  ret = queue.create(15, 34);
  FW_ASSERT(ret, ret);
  ret = queue.create(DEPTH, MSG_SIZE);
  FW_ASSERT(ret, ret);
  U8 recv[MSG_SIZE];

  U8 send[MSG_SIZE];
  for(U32 ii = 0; ii < sizeof(send); ++ii) {
    send[ii] = ii;
  }

  printf("Test empty queue...\n");
  size = sizeof(recv);
  ret = queue.pop(&recv[0], size, priority);
  FW_ASSERT(size == 0);
  FW_ASSERT(!ret, ret);
  FW_ASSERT(priority == 0, priority);
  printf("Passed.\n");

  printf("Test full queue...\n");
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    printf("Pushing %d.\n", ii);
    ret = queue.push(&send[0], sizeof(send), priority);
    FW_ASSERT(ret, ret);
    count = queue.getCount();
    maxCount = queue.getMaxCount();
    FW_ASSERT(count == ii+1, count);
    FW_ASSERT(maxCount == ii+1, maxCount);
  }
  ret = queue.push(&send[0], sizeof(send), priority);
  FW_ASSERT(!ret, ret);
  printf("Passed.\n");

  printf("Test weird size...\n");
  size = 1;
  ret = queue.pop(&recv[0], size, priority);
  FW_ASSERT(!ret, ret);
  FW_ASSERT(size == sizeof(send), size);
  FW_ASSERT(priority == 0, priority);
  printf("Passed.\n");

  printf("Test pop...\n");
  for(NATIVE_UINT_TYPE ii = DEPTH; ii > 0; --ii) {
    printf("Popping %d.\n", ii);
    size = sizeof(recv);
    ret = queue.pop(&recv[0], size, priority);
    FW_ASSERT(size == sizeof(recv), size);
    FW_ASSERT(ret, ret);
    FW_ASSERT(memcmp(recv, send, size) == 0);
    FW_ASSERT(priority == 0, priority);
    count = queue.getCount();
    maxCount = queue.getMaxCount();
    FW_ASSERT(count == ii-1, count);
    FW_ASSERT(maxCount == DEPTH, maxCount);
  }
  size = sizeof(recv);
  ret = queue.pop(&recv[0], size, priority);
  FW_ASSERT(size == 0);
  FW_ASSERT(!ret, ret);
  FW_ASSERT(priority == 0, priority);
  printf("Passed.\n");

  printf("Test priorities...\n");
  // Send messages with mixed priorities,
  // and make sure we get back the buffers in
  // the correct order.
  BufferQueue queue2;
  ret = queue2.create(1, 1);
  FW_ASSERT(ret, ret);
  ret = queue2.create(1, 1);
  FW_ASSERT(ret, ret);
  ret = queue2.create(DEPTH, 100);
  FW_ASSERT(ret, ret);
  ret = queue2.create(DEPTH, 100);
  FW_ASSERT(ret, ret);
  NATIVE_INT_TYPE priorities[DEPTH] = {9, 4, 100, 4, 9};
  const char* messages[DEPTH] = {"hello", "how are you", "pretty good", "cosmic bro", "kthxbye"};
  // Fill queue:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    printf("Pushing '%s' at priority %d with size %d\n", messages[ii], priorities[ii], (int)strlen(messages[ii]));
    ret = queue2.push((U8*) messages[ii], (int)strlen(messages[ii]), priorities[ii]);
    FW_ASSERT(ret, ret);
    count = queue2.getCount();
    maxCount = queue2.getMaxCount();
    FW_ASSERT(count == ii+1, count);
    FW_ASSERT(maxCount == ii+1, maxCount);
  }

#if PRIORITY_QUEUE
  // Pop things off the queue in the expected order:
  NATIVE_INT_TYPE orderedPriorities[DEPTH] = {100, 9, 9, 4, 4};
  const char* orderedMessages[DEPTH] = {"pretty good", "hello", "kthxbye", "how are you", "cosmic bro"};
#else
  NATIVE_INT_TYPE orderedPriorities[DEPTH] = {0, 0, 0, 0, 0};
  const char* orderedMessages[DEPTH] = {"hello", "how are you", "pretty good", "cosmic bro", "kthxbye"};
#endif
  char temp[101];
  size = sizeof(temp) - 1; // Leave room for extra \0 because of the message print
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    ret = queue2.pop((U8*) temp, size, priority);
    temp[size] = '\0';
    FW_ASSERT(ret, ret);
    FW_ASSERT(priority == orderedPriorities[ii], priority);
    FW_ASSERT(size == strlen(orderedMessages[ii]), strlen(orderedMessages[ii]));
    printf("Popped '%s' at priority %d. Expected '%s' at priority %d.\n",
           temp, priority, orderedMessages[ii], orderedPriorities[ii]);
    FW_ASSERT(memcmp(temp, orderedMessages[ii], size) == 0, ii);
    size = sizeof(temp) - 1; //Reset size
  }


  printf("Passed.\n");

  printf("Test done.\n");
}
