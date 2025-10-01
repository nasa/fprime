//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <cerrno>
#include <cstring>
#include <string>

#include <Drv/Ip/IpSocket.hpp>
#include <Drv/Ip/UdpSocket.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Console.hpp>

Os::Console logger;

enum UdpMode { DUPLEX, SEND, RECEIVE };

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

TEST(UdpZeroLength, TestZeroLengthUdpDatagram) {
    Drv::UdpSocket sender;
    Drv::UdpSocket receiver;
    Drv::SocketDescriptor send_fd;
    Drv::SocketDescriptor recv_fd;
    U16 port = Drv::Test::get_free_port(true);
    ASSERT_NE(0, port);

    // Configure receiver and sender
    ASSERT_EQ(receiver.configureRecv("127.0.0.1", port), Drv::SOCK_SUCCESS);
    ASSERT_EQ(receiver.open(recv_fd), Drv::SOCK_SUCCESS);

    ASSERT_EQ(sender.configureSend("127.0.0.1", port, 1, 0), Drv::SOCK_SUCCESS);
    ASSERT_EQ(sender.open(send_fd), Drv::SOCK_SUCCESS);

    // Send a zero-length datagram using the F' socket wrapper
    U8 empty_data[1] = {0};  // Buffer is required, but size is 0
    ASSERT_EQ(sender.send(send_fd, empty_data, 0), Drv::SOCK_SUCCESS)
        << "Failed to send zero-length datagram using F' socket wrapper";

    // Add a small delay to ensure the packet has time to be processed by the OS
    usleep(10000);  // 10ms delay

    // Receive the zero-length datagram using the F' socket wrapper
    U8 recv_buf[1] = {0xFF};
    FwSizeType recv_buf_len = 1;
    FwSignedSizeType recv_status = receiver.recv(recv_fd, recv_buf, recv_buf_len);

    // Expect 0 (success) for a zero-length datagram.
    ASSERT_EQ(recv_status, 0) << "Expected recv_status 0 for zero-length datagram, but got " << recv_status
                              << " with errno=" << errno;

    // Check that the received length is reported as 0
    ASSERT_EQ(recv_buf_len, 0) << "Expected received length 0, but got " << recv_buf_len;

    // Check that the received buffer is unchanged meaning no data was received
    ASSERT_EQ(recv_buf[0], 0xFF) << "Expected unchanged buffer (0xFF), but got " << recv_buf[0];

    sender.close(send_fd);
    receiver.close(recv_fd);
}

TEST(SingleSide, TestSingleSideMultipleSendUdp) {
    test_with_loop(100, SEND);
}

TEST(Ephemeral, TestEphemeralPorts) {
    Drv::UdpSocket receiver;
    Drv::SocketDescriptor recv_fd;
    const U16 recv_port = 50001;
    // Configure receiver as receiver-only with no send port.
    receiver.configureRecv("127.0.0.1", recv_port);
    receiver.configureSend("127.0.0.1", 0, 0, 100);
    ASSERT_EQ(receiver.open(recv_fd), Drv::SOCK_SUCCESS);

    Drv::UdpSocket sender;
    Drv::SocketDescriptor send_fd;
    // Configure sender for both send and receive (duplex) with ephemeral receive port
    sender.configureSend("127.0.0.1", recv_port, 0, 100);
    sender.configureRecv("127.0.0.1", 0);
    ASSERT_EQ(sender.open(send_fd), Drv::SOCK_SUCCESS);

    // Send a test message
    const char* msg = "hello from ephemeral sender";
    FwSizeType msg_len = static_cast<FwSizeType>(strlen(msg) + 1);
    ASSERT_EQ(sender.send(send_fd, reinterpret_cast<const U8*>(msg), msg_len), Drv::SOCK_SUCCESS);

    // Receive the message and capture sender's port
    char recv_buf[64] = {0};
    FwSizeType recv_buf_len = sizeof(recv_buf);
    ASSERT_EQ(receiver.recv(recv_fd, reinterpret_cast<U8*>(recv_buf), recv_buf_len), Drv::SOCK_SUCCESS);
    ASSERT_STREQ(msg, recv_buf);

    // Receiver sends a response back to sender
    const char* reply = "reply from receiver";
    FwSizeType reply_len = static_cast<FwSizeType>(strlen(reply) + 1);
    ASSERT_EQ(receiver.send(recv_fd, reinterpret_cast<const U8*>(reply), reply_len), Drv::SOCK_SUCCESS);

    // Sender receives the response
    char reply_buf[64] = {0};
    FwSizeType reply_buf_len = sizeof(reply_buf);
    ASSERT_EQ(sender.recv(send_fd, reinterpret_cast<U8*>(reply_buf), reply_buf_len), Drv::SOCK_SUCCESS);
    ASSERT_STREQ(reply, reply_buf);

    sender.close(send_fd);
    receiver.close(recv_fd);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
