//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <Drv/Ip/IpSocket.hpp>
#include <Drv/Ip/SocketComponentHelper.hpp>
#include <Drv/Ip/TcpClientSocket.hpp>
#include <Drv/Ip/TcpServerSocket.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Console.hpp>
#include <cerrno>

Os::Console logger;

class InterruptOnceSocket final : public Drv::IpSocket {
  public:
    U32 recv_calls = 0;

  private:
    Drv::SocketIpStatus openProtocol(Drv::SocketDescriptor& fd) override {
        fd.fd = 0;
        return Drv::SOCK_SUCCESS;
    }

    FwSignedSizeType sendProtocol(const Drv::SocketDescriptor&, const U8* const, const FwSizeType size) override {
        return static_cast<FwSignedSizeType>(size);
    }

    FwSignedSizeType recvProtocol(const Drv::SocketDescriptor&, U8* const data, const FwSizeType) override {
        this->recv_calls++;
        if (this->recv_calls == 1) {
            errno = EINTR;
            return -1;
        }
        data[0] = 0xA5;
        return 1;
    }
};

TEST(ErrorHandling, TestRecvRetriesEintr) {
    InterruptOnceSocket socket;
    Drv::SocketDescriptor fd;
    U8 data[1] = {0};
    FwSizeType size = sizeof data;

    EXPECT_EQ(socket.recv(fd, data, size), Drv::SOCK_SUCCESS);
    EXPECT_EQ(socket.recv_calls, 2u);
    EXPECT_EQ(size, 1u);
    EXPECT_EQ(data[0], 0xA5);
}

void test_with_loop(U32 iterations) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    U16 port = 0;  // Choose a port
    Drv::TcpServerSocket server;
    Drv::SocketDescriptor server_fd;
    Drv::SocketDescriptor client_fd;
    server.configure("127.0.0.1", port, 0, 100);
    EXPECT_EQ(server.startup(server_fd), Drv::SOCK_SUCCESS);
    Drv::Test::force_recv_timeout(server_fd.fd, server);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        Drv::TcpClientSocket client;
        client.configure("127.0.0.1", server.getListenPort(), 0, 100);
        // client_fd gets assigned a real value here
        status1 = client.open(client_fd);
        EXPECT_EQ(status1, Drv::SOCK_SUCCESS) << "With errno: " << errno;

        // client_fd gets assigned a real value here
        status2 = server.open(server_fd);
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS);

        // If all the opens worked, then run this
        if (Drv::SOCK_SUCCESS == status1 && Drv::SOCK_SUCCESS == status2) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(client_fd.fd, client);
            Drv::Test::force_recv_timeout(server_fd.fd, server);
            Drv::Test::send_recv(server, client, server_fd, client_fd);
            Drv::Test::send_recv(client, server, client_fd, server_fd);
        }
        server.shutdown(client_fd);
        // Drain the server before close
        Drv::Test::drain(server, server_fd);
        server.close(server_fd);
        client.close(client_fd);
    }
    server.terminate(server_fd);
}

TEST(Nominal, TestNominalTcp) {
    test_with_loop(1);
}

TEST(Nominal, TestMultipleTcp) {
    test_with_loop(100);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
