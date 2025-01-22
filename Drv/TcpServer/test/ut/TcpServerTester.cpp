// ======================================================================
// \title  TcpServerTester.cpp
// \author mstarch
// \brief  cpp file for TcpServerTester for TcpServer
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "TcpServerTester.hpp"
#include "STest/Pick/Pick.hpp"
#include "Os/Console.hpp"
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Console logger;

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

void TcpServerTester ::setup_helper(bool recv_thread, bool reconnect) {
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    U16 port =  0;
    EXPECT_FALSE(component.isStarted());
    status1 = this->component.configure("127.0.0.1", port, 0, 100);
    EXPECT_EQ(status1, Drv::SOCK_SUCCESS);
    // Start up a receive thread
    if (recv_thread) {
        Os::TaskString name("receiver thread");
        this->component.setAutomaticOpen(reconnect);
        this->component.start(name, Os::Task::TASK_DEFAULT, Os::Task::TASK_DEFAULT);
    }
    // Component should always launch the listening server on configure
    // The thread will retry if the configure fails
    EXPECT_TRUE(this->wait_on_started(true, Drv::Test::get_configured_delay_ms()/10 + 1));
    EXPECT_TRUE(component.isStarted());
}

void TcpServerTester ::test_with_loop(U32 iterations, bool recv_thread) {
    U8 buffer[sizeof(m_data_storage)] = {};
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;
    Drv::SocketDescriptor client_fd;

    this->setup_helper(recv_thread, recv_thread);

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations && status1 == SOCK_SUCCESS; i++) {
        Drv::TcpClientSocket client;
        client.configure("127.0.0.1", this->component.getListenPort(), 0, 100);
        status2 = client.open(client_fd);
        EXPECT_EQ(status2, Drv::SocketIpStatus::SOCK_SUCCESS) << "Failed to connect client";
        U32 size = sizeof(m_data_storage);

        // Not testing with reconnect thread, we will need to open ourselves
        if (not recv_thread) {
            status1 = this->component.open();
        } else {
            EXPECT_TRUE(this->wait_on_change(true, Drv::Test::get_configured_delay_ms()/10 + 1)) <<
                "On iteration: " << i << " and receive thread: " << recv_thread;
        }
        EXPECT_TRUE(this->component.isOpened()) <<
            "On iteration: " << i << " and receive thread: " << recv_thread;

        EXPECT_EQ(status1, Drv::SOCK_SUCCESS) <<
            "On iteration: " << i << " and receive thread: " << recv_thread;
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS) <<
            "On iteration: " << i << " and receive thread: " << recv_thread;

        // If all the opens worked, then run this
        if ((Drv::SOCK_SUCCESS == status1) && (Drv::SOCK_SUCCESS == status2) &&
            (this->component.isOpened())) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(this->component.m_descriptor.fd, this->component.getSocketHandler());
            Drv::Test::force_recv_timeout(client_fd.fd, client);
            m_data_buffer.setSize(sizeof(m_data_storage));
            size = Drv::Test::fill_random_buffer(m_data_buffer);
            Drv::SendStatus status = invoke_to_send(0, m_data_buffer);
            EXPECT_EQ(status, SendStatus::SEND_OK) <<
                "On iteration: " << i << " and receive thread: " << recv_thread;
            Drv::Test::receive_all(client, client_fd, buffer, size);
            EXPECT_EQ(status2, Drv::SOCK_SUCCESS) <<
                "On iteration: " << i << " and receive thread: " << recv_thread << " and errno " << errno;
            EXPECT_EQ(size, m_data_buffer.getSize()) <<
                "On iteration: " << i << " and receive thread: " << recv_thread;
            Drv::Test::validate_random_buffer(m_data_buffer, buffer);

            // If receive thread is live, try the other way
            if (recv_thread) {
                m_spinner = false;
                m_data_buffer.setSize(sizeof(m_data_storage));
                status2 = client.send(client_fd, m_data_buffer.getData(), m_data_buffer.getSize());
                EXPECT_EQ(status2, Drv::SOCK_SUCCESS) <<
                    "On iteration: " << i << " and receive thread: " << recv_thread;
                if (status2 == Drv::SOCK_SUCCESS) {
                    while (not m_spinner) {}
                }
            }
        }

        // Properly stop the client on the last iteration
        if (((1 + i) == iterations) && recv_thread) {
            this->component.stop();
            this->component.join();
        } else {
            // Server initiates shutdown. It thus must drain its data until it receives
            // a socket disconnection. Then it can safely close.
            this->component.shutdown();
            Drv::Test::drain(this->component.m_socket, this->component.m_descriptor);
            this->component.close();
        }
        // Server should have shutdown cleanly and waited for this to be shut down.  It is safe
        // to release the file descriptor.
        client.close(client_fd);
    }
    this->component.terminate();
    ASSERT_from_ready_SIZE(iterations);
}

bool TcpServerTester::wait_on_change(bool open, U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        if (open == this->component.isOpened()) {
            return true;
        }
        Os::Task::delay(Fw::TimeInterval(0, 10000));
    }
    return false;
}


bool TcpServerTester::wait_on_started(bool open, U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        if (open == this->component.isStarted()) {
            return true;
        }
        Os::Task::delay(Fw::TimeInterval(0, 10000));
    }
    return false;
}

TcpServerTester ::TcpServerTester()
    : TcpServerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TcpServer"),
      m_data_buffer(m_data_storage, 0), m_spinner(true) {
    this->initComponents();
    this->connectPorts();
    ::memset(m_data_storage, 0, sizeof(m_data_storage));
}

TcpServerTester ::~TcpServerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TcpServerTester ::test_basic_messaging() {
    test_with_loop(1);
}

void TcpServerTester ::test_multiple_messaging() {
    test_with_loop(100);
}

void TcpServerTester ::test_receive_thread() {
    test_with_loop(1, true);
}

void TcpServerTester ::test_advanced_reconnect() {
    test_with_loop(10, true); // Up to 10 * RECONNECT_MS
}

void TcpServerTester ::test_no_automatic_send_connection() {
    Drv::TcpClientSocket client;
    Drv::SocketDescriptor client_fd;
    
    // Set up the server without automatic connection
    this->setup_helper(false, true);
    this->component.setAutomaticOpen(false);
    Drv::Test::force_recv_timeout(client_fd.fd, client);

    // Connect a client to the server so it is waiting in the "listen" queue
    client.configure("127.0.0.1", this->component.getListenPort(), 0, 100);
    ASSERT_EQ(client.open(client_fd), Drv::SOCK_SUCCESS);

    // Send a message from the server and ensure that the server is not opened
    ASSERT_EQ(this->component.send(reinterpret_cast<const U8*>("a"), 1), Drv::SOCK_AUTO_CONNECT_DISABLED);
    ASSERT_FALSE(this->component.isOpened());

    // Clean-up even if the send worked
    client.close(client_fd);
    this->component.terminate();
}

void TcpServerTester ::test_no_automatic_recv_connection() {
    Drv::TcpClientSocket client;
    Drv::SocketDescriptor client_fd;

    // Set up the server without automatic connection
    this->setup_helper(true, false);

    // Connect a client to the server so it is waiting in the "listen" queue
    // The read thread should not automatically connect and will thus exit with a failure
    client.configure("127.0.0.1", this->component.getListenPort(), 0, 100);
    ASSERT_EQ(client.open(client_fd), Drv::SOCK_FAILED_TO_CONNECT);
    ASSERT_FALSE(this->component.isOpened());

    // Clean-up even if the receive worked
    client.close(client_fd);
    this->component.terminate();
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void TcpServerTester ::from_recv_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& recvBuffer, const RecvStatus& recvStatus) {
    // this function will still receive a status of error because the recv port is always called
    this->pushFromPortEntry_recv(recvBuffer, recvStatus);
    if (recvStatus == RecvStatus::RECV_OK) {
        // Make sure we can get to unblocking the spinner
        EXPECT_EQ(m_data_buffer.getSize(), recvBuffer.getSize()) << "Invalid transmission size";
        Drv::Test::validate_random_buffer(m_data_buffer, recvBuffer.getData());
        m_spinner = true;
    }
    delete[] recvBuffer.getData();
}

void TcpServerTester ::from_ready_handler(const NATIVE_INT_TYPE portNum) {
    this->pushFromPortEntry_ready();
}

Fw::Buffer TcpServerTester ::
    from_allocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_allocate(size);
    Fw::Buffer buffer(new U8[size], size);
    return buffer;
  }

  void TcpServerTester ::
    from_deallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_deallocate(fwBuffer);
  }
}  // end namespace Drv
