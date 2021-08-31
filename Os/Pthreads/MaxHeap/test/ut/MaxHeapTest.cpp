#include "Os/Pthreads/MaxHeap/MaxHeap.hpp"
#include <Fw/Types/Assert.hpp>
#include <stdio.h>
#include <string.h>

using namespace Os;

#define DEPTH 5
#define DATA_SIZE 3
int main() {
  printf("Creating heap.\n");
  bool ret;
  MaxHeap heap;
  ret = heap.create(0);
  FW_ASSERT(ret, ret);
  ret = heap.create(1000000);
  FW_ASSERT(ret, ret);
  ret = heap.create(1);
  FW_ASSERT(ret, ret);
  ret = heap.create(DEPTH);
  FW_ASSERT(ret, ret);
  ret = heap.create(DEPTH);
  FW_ASSERT(ret, ret);
  NATIVE_INT_TYPE value;
  NATIVE_UINT_TYPE size;
  NATIVE_UINT_TYPE id=0;

  printf("Testing empty...\n");
  ret = heap.pop(value, id);
  FW_ASSERT(id == 0, id);
  FW_ASSERT(!ret, ret);
  ret = heap.pop(value, id);
  FW_ASSERT(id == 0, id);
  FW_ASSERT(!ret, ret);
  ret = heap.pop(value, id);
  FW_ASSERT(id == 0, id);
  FW_ASSERT(!ret, ret);
  printf("Passed.\n");

  printf("Testing push...\n");
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    //printf("Inserting value %d\n", ii);
    ret = heap.push(ii, ii);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii+1, size, ii+1);
  }
  ret = heap.push(50, id);
  FW_ASSERT(!ret, ret);
  ret = heap.push(50, id);
  FW_ASSERT(!ret, ret);
  ret = heap.push(50, id);
  FW_ASSERT(!ret, ret);
  printf("Passed.\n");

  printf("Testing pop...\n");
  //heap.print();
  for(NATIVE_INT_TYPE ii = DEPTH-1; ii >= 0; --ii) {
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    FW_ASSERT(id == (NATIVE_UINT_TYPE) ii, id, ii);
    size = heap.getSize();
    FW_ASSERT((NATIVE_INT_TYPE) size == ii, size, ii);
    //printf("Heap state after pop:\n");
    //heap.print();
    //printf("Got value %d\n", value);
    FW_ASSERT(value == ii, value, ii);
  }
  ret = heap.pop(value, id);
  FW_ASSERT(!ret, ret);
  printf("Passed.\n");

  printf("Testing random...\n");
  NATIVE_INT_TYPE values[DEPTH] = {56, 0, 500, 57, 5};
  NATIVE_INT_TYPE sorted[DEPTH] = {500, 57, 56, 5, 0};
  //heap.print();
  // Push values on in random order:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Push next value in the list:
    ret = heap.push(values[ii], id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii+1, size, ii+1);

    // Pop the top value:
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii, size, ii);
    FW_ASSERT(value >= values[ii], value, values[ii]);

    // Push the popped value:
    ret = heap.push(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii+1, size, ii+1);
  }
  ret = heap.push(values[0], id);
  FW_ASSERT(!ret, ret);
  
  // Get values out from largest to smallest:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Pop the top value:
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == (DEPTH-ii-1), size, (DEPTH-ii-1));
    FW_ASSERT(value == sorted[ii], value, sorted[ii]);

    // Push the top value:
    ret = heap.push(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == (DEPTH-ii), size, (DEPTH-ii));

    // Pop again:
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == (DEPTH-ii-1), size, (DEPTH-ii-1));
    FW_ASSERT(value == sorted[ii], value, sorted[ii]);
  }
  ret = heap.pop(value, id);
  FW_ASSERT(!ret, ret);
  printf("Passed.\n");

  printf("Testing no priority...\n");
  // For this test we expect things to come in fifo order
  // since all the priorities are the same:
  NATIVE_INT_TYPE priorities[DEPTH] = {7, 7, 7, 7, 7};
  NATIVE_UINT_TYPE data[DEPTH] = {43, 56, 47, 33, 11};
  // Push values on in random order:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Push next value in the list:
    ret = heap.push(priorities[ii], data[ii]);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii+1, size, ii+1);
  }
  
  // Get values out in FIFO order:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Pop the top value:
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == (DEPTH-ii-1), size, (DEPTH-ii-1));
    FW_ASSERT(value == priorities[ii], value, priorities[ii]);
    FW_ASSERT(id == data[ii], id, data[ii]);
  }
  printf("Passed.\n");

  printf("Testing mixed priority...\n");
  // For this test we expect things to come in fifo order
  // for things of the same priority and in priority
  // order for things of different priorities.
  NATIVE_INT_TYPE pries[DEPTH] = {1, 7, 100, 1, 7};
  NATIVE_INT_TYPE data2[DEPTH] = {4, 22, 99, 12344, 33};
  NATIVE_INT_TYPE orderedPries[DEPTH] = {100, 7, 7, 1, 1};
  NATIVE_UINT_TYPE ordered[DEPTH] = {99, 22, 33, 4, 12344};
  // Push values on in random order:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Push next value in the list:
    ret = heap.push(pries[ii], data2[ii]);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == ii+1, size, ii+1);
  }

  // Get values out in FIFO order:
  for(NATIVE_UINT_TYPE ii = 0; ii < DEPTH; ++ii) {
    // Pop the top value:
    ret = heap.pop(value, id);
    FW_ASSERT(ret, ret);
    size = heap.getSize();
    FW_ASSERT(size == (DEPTH-ii-1), size, (DEPTH-ii-1));
    FW_ASSERT(value == orderedPries[ii], value, orderedPries[ii]);
    FW_ASSERT(id == ordered[ii], id, ordered[ii]);
  }
  printf("Passed.\n");

  printf("Test done.\n");
}
