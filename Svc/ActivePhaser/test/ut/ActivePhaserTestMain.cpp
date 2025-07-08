// ======================================================================
// \title  ActivePhaserTestMain.cpp
// \author shaokail
// \brief  cpp file for ActivePhaser component test main function
// ======================================================================

#include "ActivePhaserTester.hpp"

TEST(Nominal, toDo) {
    Svc::ActivePhaserTester tester;
    tester.toDo();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
