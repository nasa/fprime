// ======================================================================
// \title  Tester.cpp
// \author mstarch
// \brief  cpp file for Tester of TcpClient
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "Tester.hpp"
#include "STest/Pick/Pick.hpp"
#include <Os/Log.hpp>
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>

Os::Log logger;

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

void Tester ::test_with_loop(U32 iterations, bool recv_thread) {
    U8 buffer[sizeof(m_data_storage)] = {};
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus serverStat = Drv::SOCK_SUCCESS;

    U16 port =  Drv::Test::get_free_port();
    ASSERT_NE(0, port);

    Drv::TcpServerSocket server;
    server.configure("127.0.0.1", port, 0, 100);
    this->component.configure("127.0.0.1", port, 0, 100);
    serverStat = server.startup();
    ASSERT_EQ(serverStat, SOCK_SUCCESS);

    // Start up a receive thread
    if (recv_thread) {
        Os::TaskString name("receiver thread");
        this->component.startSocketTask(name, true, Os::Task::TASK_DEFAULT, Os::Task::TASK_DEFAULT);
    }

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        I32 size = sizeof(m_data_storage);

        // Not testing with reconnect thread, we will need to open ourselves
        if (not recv_thread) {
            status1 = this->component.open();
        } else {
            EXPECT_TRUE(Drv::Test::wait_on_change(this->component.getSocketHandler(), true, SOCKET_RETRY_INTERVAL_MS/10 + 1));
        }
        EXPECT_TRUE(this->component.getSocketHandler().isOpened());
        status2 = server.open();

        EXPECT_EQ(status1, Drv::SOCK_SUCCESS);
        EXPECT_EQ(status2, Drv::SOCK_SUCCESS);

        // If all the opens worked, then run this
        if ((Drv::SOCK_SUCCESS == status1) && (Drv::SOCK_SUCCESS == status2) &&
            (this->component.getSocketHandler().isOpened())) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(this->component.getSocketHandler());
            Drv::Test::force_recv_timeout(server);
            m_data_buffer.setSize(sizeof(m_data_storage));
            Drv::Test::fill_random_buffer(m_data_buffer);
            Drv::SendStatus status = invoke_to_send(0, m_data_buffer);
            EXPECT_EQ(status, SendStatus::SEND_OK);
            status2 = server.recv(buffer, size);
            EXPECT_EQ(status2, Drv::SOCK_SUCCESS);
            EXPECT_EQ(size, m_data_buffer.getSize());
            Drv::Test::validate_random_buffer(m_data_buffer, buffer);
            // If receive thread is live, try the other way
            if (recv_thread) {
                m_spinner = false;
                m_data_buffer.setSize(sizeof(m_data_storage));
                server.send(m_data_buffer.getData(), m_data_buffer.getSize());
                while (not m_spinner) {}
            }
        }
        // Properly stop the client on the last iteration
        if ((1 + i) == iterations && recv_thread) {
            this->component.stopSocketTask();
            this->component.joinSocketTask(nullptr);
        } else {
            this->component.close();
        }
        server.close();
    }
    server.shutdown();
    ASSERT_from_ready_SIZE(iterations);
}

Tester ::Tester()
    : ByteStreamDriverModelGTestBase("Tester", MAX_HISTORY_SIZE),
      component("ByteStreamDriverModel"),
      m_data_buffer(m_data_storage, 0), m_spinner(true) {
    this->initComponents();
    this->connectPorts();
    ::memset(m_data_storage, 0, sizeof(m_data_storage));
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------


void Tester ::test_basic_messaging() {
    test_with_loop(1);
}

void Tester ::test_multiple_messaging() {
    test_with_loop(100);
}

void Tester ::test_receive_thread() {
    test_with_loop(1, true);
}

void Tester ::test_advanced_reconnect() {
    test_with_loop(10, true); // Up to 10 * RECONNECT_MS
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

  void Tester ::
    from_recv_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &recvBuffer,
        const RecvStatus &recvStatus
    )
  {
    this->pushFromPortEntry_recv(recvBuffer, recvStatus);
    // Make sure we can get to unblocking the spinner
    EXPECT_EQ(m_data_buffer.getSize(), recvBuffer.getSize()) << "Invalid transmission size";
    Drv::Test::validate_random_buffer(m_data_buffer, recvBuffer.getData());
    m_spinner = true;
    delete[] recvBuffer.getData();
}

void Tester ::from_ready_handler(const NATIVE_INT_TYPE portNum) {
    this->pushFromPortEntry_ready();
}

Fw::Buffer Tester ::
    from_allocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_allocate(size);
    Fw::Buffer buffer(new U8[size], size);
    m_data_buffer2 = buffer;
    return buffer;
  }

  void Tester ::
    from_deallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_deallocate(fwBuffer);
  }

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // send
    this->connect_to_send(
        0,
        this->component.get_send_InputPort(0)
    );

    // poll
    this->connect_to_poll(
        0,
        this->component.get_poll_InputPort(0)
    );

    // recv
    this->component.set_recv_OutputPort(
        0,
        this->get_from_recv(0)
    );

    // recv
    this->component.set_ready_OutputPort(
      0,
      this->get_from_ready(0)
    );

    // allocate
    this->component.set_allocate_OutputPort(
        0,
        this->get_from_allocate(0)
    );

    // deallocate
    this->component.set_deallocate_OutputPort(
        0,
        this->get_from_deallocate(0)
    );

}

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Drv
