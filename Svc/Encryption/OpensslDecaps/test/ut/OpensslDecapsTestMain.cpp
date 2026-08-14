// ======================================================================
// \title  OpensslDecapsTestMain.cpp
// \author vivi
// \brief  cpp file for OpensslDecaps component test main function
// ======================================================================

#include "OpensslDecapsTester.hpp"

TEST(Nominal, toDo) {
    Svc::Ccsds::OpensslDecapsTester tester;
    tester.toDo();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
