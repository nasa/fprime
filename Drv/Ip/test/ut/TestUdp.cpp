//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <Drv/Ip/UdpSocket.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Os/Console.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Console logger;

enum UdpMode {
    DUPLEX,
    SEND,
    RECEIVE
};

void test_with_loop(U32 iterations, UdpMode udp_mode) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    Drv::SocketDescriptor udp1_fd;
    Drv::SocketDescriptor udp2_fd;

    U16 port1 = Drv::Test::get_free_port(true);
    ASSERT_NE(0, port1);
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
        ASSERT_TRUE(udp_mode == SEND || udp_mode == RECEIVE || udp_mode == DUPLEX) << "Invalid udp mode supplied";
        // Configure send for SEND and DUPLEX
        if ((udp_mode == SEND) || (udp_mode == DUPLEX)) {
            udp2.configureSend("127.0.0.1", port2, 0, 100);
        }
        // Configure receive for RECEIVE and DUPLEX
        if ((udp_mode == RECEIVE) || (udp_mode == DUPLEX)) {
            udp2.configureRecv("127.0.0.1", port1);
        }
        status2 = udp2.open(udp2_fd);
        ASSERT_EQ(status2, Drv::SOCK_SUCCESS);

        udp1.configureSend("127.0.0.1", port1, 0, 100);
        udp1.configureRecv("127.0.0.1", port2);
        status1 = udp1.open(udp1_fd);
        ASSERT_EQ(status1, Drv::SOCK_SUCCESS);

        // If all the opens worked, then run this
        if (Drv::SOCK_SUCCESS == status1 && Drv::SOCK_SUCCESS == status2) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(udp1_fd.fd, udp1);
            Drv::Test::force_recv_timeout(udp2_fd.fd, udp2);
            // Test UDP receiving for RECEIVE and DUPLEX
            if ((udp_mode == RECEIVE) || (udp_mode == DUPLEX)) {
                Drv::Test::send_recv(udp1, udp2, udp1_fd, udp2_fd);
            }
            // Test UDP sending for RECEIVE and DUPLEX
            if ((udp_mode == SEND) || (udp_mode == DUPLEX)) {
                Drv::Test::send_recv(udp2, udp1, udp2_fd, udp1_fd);
            }
        }
        udp1.close(udp1_fd);
        udp2.close(udp2_fd);
    }
}

TEST(Nominal, TestNominalUdp) {
    test_with_loop(1, DUPLEX);
}

TEST(Nominal, TestMultipleUdp) {
    test_with_loop(100, DUPLEX);
}

TEST(SingleSide, TestSingleSideReceiveUdp) {
    test_with_loop(1, RECEIVE);
}

TEST(SingleSide, TestSingleSideMultipleReceiveUdp) {
    test_with_loop(100, RECEIVE);
}

TEST(SingleSide, TestSingleSideSendUdp) {
    test_with_loop(1, SEND);
}

TEST(SingleSide, TestSingleSideMultipleSendUdp) {
    test_with_loop(100, SEND);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
