//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <Drv/Ip/TcpClientSocket.hpp>
#include <Drv/Ip/TcpServerSocket.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Drv/Ip/SocketComponentHelper.hpp>
#include <Os/Console.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Console logger;


void test_with_loop(U32 iterations) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    U16 port = 0; // Choose a port
    Drv::TcpServerSocket server;
    NATIVE_INT_TYPE server_fd = -1;
    NATIVE_INT_TYPE client_fd = -1;
    server.configure("127.0.0.1", port, 0, 100);
    EXPECT_EQ(server.startup(), Drv::SOCK_SUCCESS);
    Drv::Test::force_recv_timeout(server_fd, server);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        Drv::TcpClientSocket client;
        client.configure("127.0.0.1", server.getListenPort(),0,100);
        // client_fd gets assigned a real value here
        status1 = client.open(client_fd);
        EXPECT_EQ(status1, Drv::SOCK_SUCCESS);

        // client_fd gets assigned a real value here
        status2 = server.open(server_fd);
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS);


        // If all the opens worked, then run this
        if (Drv::SOCK_SUCCESS == status1 && Drv::SOCK_SUCCESS == status2) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(client_fd, client);
            Drv::Test::force_recv_timeout(server_fd, server);
            Drv::Test::send_recv(server, client, server_fd, client_fd);
            Drv::Test::send_recv(client, server, client_fd, server_fd);
        }
        client.close(client_fd);
        server.close(server_fd);
    }
    server.shutdown(server_fd);
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
