#include <Svc/UdpReceiver/test/ut/Tester.hpp>
#include <Fw/Test/UnitTest.hpp>

TEST(Nominal,OpenConnection) {

    COMMENT("Open the connections");

    Svc::Tester tester;
    tester.openTest("50000");

}

TEST(Nominal,RecvPacket) {

    COMMENT("Receive a packet");

    Svc::Tester tester;
    tester.recvTest("50000");

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
