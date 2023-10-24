//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <Drv/Ip/TcpClientSocket.hpp>
#include <Drv/Ip/TcpServerSocket.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Os/Log.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Log logger;


void test_with_loop(U32 iterations) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    U16 port =  Drv::Test::get_free_port();
    ASSERT_NE(0, port);
    Drv::TcpServerSocket server;
    server.configure("127.0.0.1", port, 0, 100);
    EXPECT_EQ(server.startup(), Drv::SOCK_SUCCESS);
    Drv::Test::force_recv_timeout(server);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        Drv::TcpClientSocket client;
        ASSERT_NE(port, 0);
        client.configure("127.0.0.1",port,0,100);
        status1 = client.open();
        EXPECT_EQ(status1, Drv::SOCK_SUCCESS);

        status2 = server.open();
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS);


        // If all the opens worked, then run this
        if (Drv::SOCK_SUCCESS == status1 && Drv::SOCK_SUCCESS == status2) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(client);
            Drv::Test::force_recv_timeout(server);
            Drv::Test::send_recv(server, client);
            Drv::Test::send_recv(client, server);
        }
        client.close();
        server.close();
    }
    server.shutdown();
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
