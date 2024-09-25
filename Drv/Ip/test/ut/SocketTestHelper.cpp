//
// Created by mstarch on 12/10/20.
//
#include <Os/Task.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>
#include "STest/Pick/Pick.hpp"
#include <gtest/gtest.h>

#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#include <IpCfg.hpp>

namespace Drv {
namespace Test {

const U32 MAX_DRV_TEST_MESSAGE_SIZE = 1024;

void force_recv_timeout(NATIVE_INT_TYPE fd, Drv::IpSocket& socket) {
    // Set timeout socket option
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50; // 50ms max before test failure
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout));
}

void validate_random_data(U8 *data, U8 *truth, U32 size) {
    for (U32 i = 0; i < size; i++) {
        ASSERT_EQ(data[i], truth[i]);
    }
}

void fill_random_data(U8 *data, U32 size) {
    ASSERT_NE(size, 0u) << "Trying to fill random data of size 0";
    for (U32 i = 0; i < size; i++) {
        data[i] = static_cast<U8>(STest::Pick::any());
    }
}

void validate_random_buffer(Fw::Buffer &buffer, U8 *data) {
    validate_random_data(buffer.getData(), data, buffer.getSize());
    buffer.setSize(0);
}

void fill_random_buffer(Fw::Buffer &buffer) {
    buffer.setSize(STest::Pick::lowerUpper(1, buffer.getSize()));
    fill_random_data(buffer.getData(), buffer.getSize());
}

void send_recv(Drv::IpSocket& sender, Drv::IpSocket& receiver, NATIVE_INT_TYPE sender_fd, NATIVE_INT_TYPE receiver_fd) {
    U32 size = MAX_DRV_TEST_MESSAGE_SIZE;
    U8 buffer_out[MAX_DRV_TEST_MESSAGE_SIZE] = {0};
    U8 buffer_in[MAX_DRV_TEST_MESSAGE_SIZE] = {0};

    // Send receive validate block
    Drv::Test::fill_random_data(buffer_out, MAX_DRV_TEST_MESSAGE_SIZE);
    EXPECT_EQ(sender.send(sender_fd, buffer_out, MAX_DRV_TEST_MESSAGE_SIZE), Drv::SOCK_SUCCESS);
    EXPECT_EQ(receiver.recv(receiver_fd, buffer_in, size), Drv::SOCK_SUCCESS);
    EXPECT_EQ(size, static_cast<U32>(MAX_DRV_TEST_MESSAGE_SIZE));
    Drv::Test::validate_random_data(buffer_out, buffer_in, MAX_DRV_TEST_MESSAGE_SIZE);
}

U64 get_configured_delay_ms() {
    return (static_cast<U64>(SOCKET_RETRY_INTERVAL.getSeconds()) * 1000) +
           (static_cast<U64>(SOCKET_RETRY_INTERVAL.getUSeconds()) / 1000);
}

};
};
