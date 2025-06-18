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
#include <config/IpCfg.hpp>

namespace Drv {
namespace Test {

const U32 MAX_DRV_TEST_MESSAGE_SIZE = 1024;

void force_recv_timeout(int fd, Drv::IpSocket& socket, const TestTimeouts* custom_timeouts) {
    // Set timeout socket option
    struct timeval timeout;
    if (custom_timeouts != nullptr) {
        timeout.tv_sec = static_cast<time_t>(custom_timeouts->m_sec);
        timeout.tv_usec = static_cast<suseconds_t>(custom_timeouts->m_usec);
    } else {
        // Default timeout if no custom one is provided
        timeout.tv_sec = 0;
        timeout.tv_usec = static_cast<suseconds_t>(100000); // 100ms default
    }
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

U32 fill_random_buffer(Fw::Buffer &buffer) {
    buffer.setSize(STest::Pick::lowerUpper(1, buffer.getSize()));
    fill_random_data(buffer.getData(), buffer.getSize());
    return static_cast<U32>(buffer.getSize());
}

void drain(Drv::IpSocket& receiver, Drv::SocketDescriptor& receiver_fd) {
    Drv::SocketIpStatus status = SOCK_SUCCESS;
    // Drain the server in preparation for close
    while (status == Drv::SOCK_SUCCESS || status == Drv::SOCK_NO_DATA_AVAILABLE) {
        U8 buffer[1];
        U32 size = sizeof buffer;
        status = receiver.recv(receiver_fd, buffer, size);
    }
    ASSERT_EQ(status, Drv::SocketIpStatus::SOCK_DISCONNECTED) << "Socket did not disconnect as expected";
}

void receive_all(Drv::IpSocket& receiver, Drv::SocketDescriptor& receiver_fd, U8* buffer, U32 size) {
    ASSERT_NE(buffer, nullptr);
    U32 received_size = 0;
    Drv::SocketIpStatus status;
    do {
        U32 bytes_to_request = size - received_size;
        U32 bytes_actually_received = bytes_to_request; // Will be updated by receiver.recv
        status = receiver.recv(receiver_fd, buffer + received_size, bytes_actually_received);

        if (status == Drv::SOCK_SUCCESS) {
            received_size += bytes_actually_received;
        } else if (status == Drv::SOCK_NO_DATA_AVAILABLE) {
            // Socket timeout occurred before all expected data was received.
            FAIL() << "Drv::Test::receive_all timed out (SOCK_NO_DATA_AVAILABLE) expecting " << size
                   << " bytes, but received only " << received_size << " bytes so far.";
            return; // Exit to prevent infinite loop and mark test as failed
        } else {
            // Other unexpected socket error
            FAIL() << "Drv::Test::receive_all encountered an unexpected socket error: " << status
                   << " while expecting " << size << " bytes. Received " << received_size << " bytes so far.";
            return; // Exit to prevent infinite loop and mark test as failed
        }
    } while (size > received_size);
    EXPECT_EQ(received_size, size);
}

void send_recv(Drv::IpSocket& sender, Drv::IpSocket& receiver, Drv::SocketDescriptor& sender_fd, Drv::SocketDescriptor& receiver_fd) {
    U32 size = MAX_DRV_TEST_MESSAGE_SIZE;

    U8 buffer_out[MAX_DRV_TEST_MESSAGE_SIZE] = {0};
    U8 buffer_in[MAX_DRV_TEST_MESSAGE_SIZE] = {0};

    // Send receive validate block
    Drv::Test::fill_random_data(buffer_out, MAX_DRV_TEST_MESSAGE_SIZE);
    EXPECT_EQ(sender.send(sender_fd, buffer_out, MAX_DRV_TEST_MESSAGE_SIZE), Drv::SOCK_SUCCESS);
    receive_all(receiver, receiver_fd, buffer_in, size);
    Drv::Test::validate_random_data(buffer_out, buffer_in, MAX_DRV_TEST_MESSAGE_SIZE);
}

U64 get_configured_delay_ms() {
    return (static_cast<U64>(SOCKET_RETRY_INTERVAL.getSeconds()) * 1000) +
           (static_cast<U64>(SOCKET_RETRY_INTERVAL.getUSeconds()) / 1000);
}

}
}
