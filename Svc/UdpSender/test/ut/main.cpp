#include <Svc/UdpSender/test/ut/Tester.hpp>
#include <Fw/Test/UnitTest.hpp>

TEST(Nominal,OpenConnection) {

    COMMENT("Open the connections");

    Svc::Tester tester;
    tester.openTest("127.0.0.1","50000");

}

TEST(Nominal,SendPacket) {

    COMMENT("Send a packet");

    Svc::Tester tester;
    tester.sendTest("127.0.0.1","50000");

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
