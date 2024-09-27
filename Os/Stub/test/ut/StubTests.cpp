#include <gtest/gtest.h>
#include <Os/File.hpp>
#include <Os/Task.hpp>
#include <Os/Queue.hpp>
#include <Os/Mutex.hpp>
#include <Os/Condition.hpp>
#include <Os/Console.hpp>


TEST(Stub, File) {
    Os::File _;
}

TEST(Stub, Task) {
    Os::Task _;
}

TEST(Stub, Queue) {
    Os::Queue _;
}

TEST(Stub, ConditionVariable) {
    Os::ConditionVariable _;
}

TEST(Stub, Mutex) {
    Os::Mutex _;
}

TEST(Stub, Console) {
    Os::Console _;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}