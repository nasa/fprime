#include "Os/Generic/Types/MaxHeap.hpp"
#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>


#define DEPTH 5
#define DATA_SIZE 3

TEST(Nominal, Creation) {
    bool ret;
    {
        Types::MaxHeap heap;
        ret = heap.create(0);
        ASSERT_TRUE(ret);
    }
    {
        Types::MaxHeap heap;
        ret = heap.create(1000000);
        ASSERT_TRUE(ret);
    }
    {
        Types::MaxHeap heap;
        ret = heap.create(1);
        ASSERT_TRUE(ret);
    }
    {
        Types::MaxHeap heap;
        ret = heap.create(DEPTH);
        ASSERT_TRUE(ret);
    }
    {
        Types::MaxHeap heap;
        ret = heap.create(DEPTH);
        ASSERT_TRUE(ret);
    }
}

TEST(Nominal, Empty) {
    bool ret;
    Types::MaxHeap heap;
    ret = heap.create(DEPTH);
    ASSERT_TRUE(ret);


    FwQueuePriorityType value;
    FwSizeType id=0;

    printf("Testing empty...\n");
    ret = heap.pop(value, id);
    ASSERT_EQ(id, 0);
    ASSERT_FALSE(ret);
    ret = heap.pop(value, id);
    ASSERT_EQ(id, 0);
    ASSERT_FALSE(ret);
    ret = heap.pop(value, id);
    ASSERT_EQ(id, 0);
    ASSERT_FALSE(ret);
}

TEST(Nominal, PushPop) {
    printf("Creating heap.\n");
    bool ret;
    Types::MaxHeap heap;
    ret = heap.create(DEPTH);
    ASSERT_TRUE(ret);


    FwQueuePriorityType value;
    FwSizeType size;
    FwSizeType id=0;

    printf("Testing push...\n");
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        //printf("Inserting value %d\n", ii);
        ret = heap.push(ii, ii);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size,ii+1);
    }
    ret = heap.push(50, id);
    ASSERT_FALSE(ret);
    ret = heap.push(50, id);
    ASSERT_FALSE(ret);
    ret = heap.push(50, id);
    ASSERT_FALSE(ret);
    printf("Passed.\n");

    printf("Testing pop...\n");
    //heap.print();
    for(FwQueuePriorityType ii = DEPTH-1; ii >= 0; --ii) {
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        ASSERT_EQ(id, static_cast<FwSizeType>(ii));
        size = heap.getSize();
        ASSERT_EQ(size, static_cast<FwSizeType>(ii));
        //printf("Heap state after pop:\n");
        //heap.print();
        //printf("Got value %d\n", value);
        ASSERT_EQ(value, ii);
    }
    ret = heap.pop(value, id);
    ASSERT_FALSE(ret);
    printf("Passed.\n");

    printf("Testing random...\n");
    FwQueuePriorityType values[DEPTH] = {56, 0, 500, 57, 5};
    FwSizeType sorted[DEPTH] = {500, 57, 56, 5, 0};
    //heap.print();
    // Push values on in random order:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Push next value in the list:
        ret = heap.push(values[ii], id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, ii+1);

        // Pop the top value:
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, ii);
        ASSERT_GE(value, values[ii]);

        // Push the popped value:
        ret = heap.push(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, ii+1);
    }
    ret = heap.push(values[0], id);
    ASSERT_FALSE(ret);

    // Get values out from largest to smallest:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Pop the top value:
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, (DEPTH-ii-1));
        ASSERT_EQ(value, sorted[ii]);

        // Push the top value:
        ret = heap.push(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, (DEPTH-ii));

        // Pop again:
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, (DEPTH-ii-1));
        ASSERT_EQ(value, sorted[ii]);
    }
    ret = heap.pop(value, id);
    ASSERT_FALSE(ret);
    printf("Passed.\n");

    printf("Testing no priority...\n");
    // For this test we expect things to come in fifo order
    // since all the priorities are the same:
    FwQueuePriorityType priorities[DEPTH] = {7, 7, 7, 7, 7};
    FwSizeType data[DEPTH] = {43, 56, 47, 33, 11};
    // Push values on in random order:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Push next value in the list:
        ret = heap.push(priorities[ii], data[ii]);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, ii+1);
    }

    // Get values out in FIFO order:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Pop the top value:
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, (DEPTH-ii-1));
        ASSERT_EQ(value, priorities[ii]);
        ASSERT_EQ(id, data[ii]);
    }
    printf("Passed.\n");

    printf("Testing mixed priority...\n");
    // For this test we expect things to come in fifo order
    // for things of the same priority and in priority
    // order for things of different priorities.
    FwQueuePriorityType pries[DEPTH] = {1, 7, 100, 1, 7};
    FwQueuePriorityType data2[DEPTH] = {4, 22, 99, 12344, 33};
    FwQueuePriorityType orderedPries[DEPTH] = {100, 7, 7, 1, 1};
    FwSizeType ordered[DEPTH] = {99, 22, 33, 4, 12344};
    // Push values on in random order:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Push next value in the list:
        ret = heap.push(pries[ii], data2[ii]);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, ii+1);
    }

    // Get values out in FIFO order:
    for(FwSizeType ii = 0; ii < DEPTH; ++ii) {
        // Pop the top value:
        ret = heap.pop(value, id);
        ASSERT_TRUE(ret);
        size = heap.getSize();
        ASSERT_EQ(size, (DEPTH-ii-1));
        ASSERT_EQ(value, orderedPries[ii]);
        ASSERT_EQ(id, ordered[ii]);
    }
    printf("Passed.\n");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
