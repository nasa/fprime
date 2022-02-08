// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, toDo) {
    Drv::Tester tester;
    tester.initialization();
}

TEST(Nominal, send) {
    Drv::Tester tester;
    tester.packetSend();
}

TEST(Nominal, receive) {
    Drv::Tester tester;
    tester.packetReceive();
}

TEST(Error, receive) {
    Drv::Tester tester;
    tester.packetReceiveError();
}

TEST(Error, allocationError) {
    Drv::Tester tester;
    tester.packetAllocationError();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
