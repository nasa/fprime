// ======================================================================
// \title  TcpClientTester.cpp
// \author mstarch
// \brief  cpp file for TcpClientTester of TcpClient
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "TcpClientTester.hpp"
#include "STest/Pick/Pick.hpp"
#include <Os/Console.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Console logger;

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

void TcpClientTester ::setup_helper(Drv::TcpServerSocket& server, Drv::SocketDescriptor& server_fd, bool recv_thread, bool reconnect) {
    Drv::SocketIpStatus serverStat = Drv::SOCK_SUCCESS;

    U16 port =  0;
    server.configure("127.0.0.1", port, 0, 100);

    serverStat = server.startup(server_fd);
    this->component.configure("127.0.0.1", server.getListenPort(), 0, 100);

    ASSERT_EQ(serverStat, SOCK_SUCCESS)
        << "TCP server startup error: " << strerror(errno) << std::endl
        << "Port: " << port << std::endl;

    // Start up a receive thread
    if (recv_thread) {
        Os::TaskString name("receiver thread");
        this->component.setAutomaticOpen(reconnect);
        this->component.start(name, Os::Task::TASK_DEFAULT, Os::Task::TASK_DEFAULT);
    }
}


void TcpClientTester ::test_with_loop(U32 iterations, bool recv_thread) {
    U8 buffer[sizeof(m_data_storage)] = {};
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    Drv::TcpServerSocket server;
    Drv::SocketDescriptor server_fd;
    setup_helper(server, server_fd, recv_thread, true);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        U32 size = sizeof(m_data_storage);

        // Not testing with reconnect thread, we will need to open ourselves
        if (not recv_thread) {
            status1 = this->component.open();
        } else {
            EXPECT_TRUE(this->wait_on_change(this->component.getSocketHandler(), true, Drv::Test::get_configured_delay_ms()/10 + 1));
        }
        EXPECT_TRUE(this->component.isOpened());
        // fd has now been updated to be a value we need to keep track of
        status2 = server.open(server_fd);

        EXPECT_EQ(status1, Drv::SOCK_SUCCESS);
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS);

        // If all the opens worked, then run this
        if ((Drv::SOCK_SUCCESS == status1) && (Drv::SOCK_SUCCESS == status2) &&
            (this->component.isOpened())) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(this->component.m_descriptor.fd, this->component.getSocketHandler());
            Drv::Test::force_recv_timeout(server_fd.serverFd, server);
            m_data_buffer.setSize(sizeof(m_data_storage));
            size = Drv::Test::fill_random_buffer(m_data_buffer);
            Drv::SendStatus status = invoke_to_send(0, m_data_buffer);
            EXPECT_EQ(status, SendStatus::SEND_OK);
            Drv::Test::receive_all(server, server_fd, buffer, size);
            Drv::Test::validate_random_buffer(m_data_buffer, buffer);
            // If receive thread is live, try the other way
            if (recv_thread) {
                m_spinner = false;
                m_data_buffer.setSize(sizeof(m_data_storage));
                status2 = server.send(server_fd, m_data_buffer.getData(), m_data_buffer.getSize());
                EXPECT_EQ(status2, Drv::SOCK_SUCCESS);
                from_deallocate_handler(0, m_data_buffer);
                while (not m_spinner) {}
            }
        }
        // Properly stop the client on the last iteration
        if (((1 + i) == iterations) && recv_thread) {
            this->component.stop();
            this->component.join();
        } else {
            // Client should close to initiate a clean shutdown
            // This is because the server "can't know" if the client is done until
            // this close is hit, or the server initiates the shutdown.
            this->component.close();
        }
        // Safe server shutdown after client
        Drv::Test::drain(server, server_fd);
        server.close(server_fd);
    }
    server.terminate(server_fd);
    ASSERT_from_ready_SIZE(iterations);
}

TcpClientTester ::TcpClientTester()
    : TcpClientGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TcpClient"),
      m_data_buffer(m_data_storage, 0), m_spinner(true) {
    this->initComponents();
    this->connectPorts();
    ::memset(m_data_storage, 0, sizeof(m_data_storage));
}

TcpClientTester ::~TcpClientTester() {}

bool TcpClientTester::wait_on_change(Drv::IpSocket &socket, bool open, U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        if (open == this->component.isOpened()) {
            return true;
        }
        Os::Task::delay(Fw::TimeInterval(0, 10000));
    }
    return false;
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------


void TcpClientTester ::test_basic_messaging() {
    test_with_loop(1);
}

void TcpClientTester ::test_multiple_messaging() {
    test_with_loop(100);
}

void TcpClientTester ::test_receive_thread() {
    test_with_loop(1, true);
}

void TcpClientTester ::test_advanced_reconnect() {
    test_with_loop(10, true); // Up to 10 * RECONNECT_MS
}

void TcpClientTester ::test_no_automatic_send_connection() {
    Drv::TcpServerSocket server;
    Drv::SocketDescriptor server_fd;
    this->setup_helper(server, server_fd, false, false);
    this->component.setAutomaticOpen(false);
    ASSERT_EQ(this->component.send(reinterpret_cast<const U8*>("a"), 1), Drv::SOCK_AUTO_CONNECT_DISABLED);
    ASSERT_FALSE(this->component.isOpened());
    // Clean-up even if the send worked
    Drv::Test::drain(server, server_fd);
    server.terminate(server_fd);
}

void TcpClientTester ::test_no_automatic_recv_connection() {
    Drv::TcpServerSocket server;
    Drv::SocketDescriptor server_fd;
    this->setup_helper(server, server_fd, true, false);
    // Wait for connection to not start
    EXPECT_FALSE(this->wait_on_change(this->component.getSocketHandler(), true, Drv::Test::get_configured_delay_ms()/10 + 1));
    ASSERT_FALSE(this->component.isOpened());
    // Clean-up even if the thread (incorrectly) started
    this->component.stop();
    this->component.join();
    Drv::Test::drain(server, server_fd);
    server.terminate(server_fd);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

  void TcpClientTester ::
    from_recv_handler(
        const FwIndexType portNum,
        Fw::Buffer &recvBuffer,
        const RecvStatus &recvStatus
    )
  {
    this->pushFromPortEntry_recv(recvBuffer, recvStatus);
    if (recvStatus == RecvStatus::RECV_OK){
        // Make sure we can get to unblocking the spinner
        EXPECT_EQ(m_data_buffer.getSize(), recvBuffer.getSize()) << "Invalid transmission size";
        Drv::Test::validate_random_buffer(m_data_buffer, recvBuffer.getData());
        m_data_buffer.setSize(0);
        m_spinner = true;
    }
    delete[] recvBuffer.getData();
}

void TcpClientTester ::from_ready_handler(const FwIndexType portNum) {
    this->pushFromPortEntry_ready();
}

Fw::Buffer TcpClientTester ::
    from_allocate_handler(
        const FwIndexType portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_allocate(size);
    Fw::Buffer buffer(new U8[size], size);
    m_data_buffer2 = buffer;
    return buffer;
  }

  void TcpClientTester ::
    from_deallocate_handler(
        const FwIndexType portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_deallocate(fwBuffer);
  }

}  // end namespace Drv
