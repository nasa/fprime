//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <Drv/Ip/UdpSocket.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Os/Log.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Log logger;

void test_with_loop(U32 iterations, bool duplex) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    // When not duplex, we can allow the OS to choose a port
    U16 port1 = (duplex) ? Drv::Test::get_free_port(true) : 0;
    U16 port2 = port1;
    for (U8 i = 0; (i < std::numeric_limits<U8>::max()) && (port2 == port1); i++) {
        port2 = Drv::Test::get_free_port(true);
    }
    if (port2 == port1) {
        GTEST_SKIP() << "Could not find two unique and available UDP ports. SKipping test.";
    }
    ASSERT_NE(0, port2);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        Drv::UdpSocket udp1;
        Drv::UdpSocket udp2;
        udp2.configureRecv("127.0.0.1", port1);
        // If simplex, test only half the channel
        if (duplex) {
            udp2.configureSend("127.0.0.1", port2, 0, 100);
        }
        status2 = udp2.open();
        ASSERT_EQ(status2, Drv::SOCK_SUCCESS);

        udp1.configureSend("127.0.0.1", udp2.getRecvPort(), 0, 100);
        udp1.configureRecv("127.0.0.1", port2);
        status1 = udp1.open();
        ASSERT_EQ(status1, Drv::SOCK_SUCCESS);

        // If all the opens worked, then run this
        if (Drv::SOCK_SUCCESS == status1 && Drv::SOCK_SUCCESS == status2) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(udp1);
            Drv::Test::force_recv_timeout(udp2);
            Drv::Test::send_recv(udp1, udp2);
            // Allow duplex connections
            if (duplex) {
                Drv::Test::send_recv(udp2, udp1);
            }
        }
        udp1.close();
        udp2.close();
    }
}

TEST(Nominal, TestNominalUdp) {
    test_with_loop(1, false);
}

TEST(Nominal, TestMultipleUdp) {
    test_with_loop(100, false);
}

TEST(SingleSide, TestSingleSideUdp) {
    test_with_loop(1, true);
}

TEST(SingleSide, TestSingleSideMultipleUdp) {
    test_with_loop(100, true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
