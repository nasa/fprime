// ======================================================================
// \title  UdpTester.cpp
// \author mstarch
// \brief  cpp file for UdpTester for Udp
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "UdpTester.hpp"
#include "STest/Pick/Pick.hpp"
#include <Drv/Ip/test/ut/PortSelector.hpp>
#include <Drv/Ip/test/ut/SocketTestHelper.hpp>
#include "Os/Log.hpp"
#include <sys/socket.h>

Os::Log logger;

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

void UdpTester::test_with_loop(U32 iterations, bool recv_thread) {
    U8 buffer[sizeof(m_data_storage)] = {};
    Drv::SocketIpStatus status1 = Drv::SOCK_SUCCESS;
    Drv::SocketIpStatus status2 = Drv::SOCK_SUCCESS;

    U16 port1 = Drv::Test::get_free_port(true);
    ASSERT_NE(0, port1);
    U16 port2 = port1;
    uint8_t attempt_to_find_available_port = std::numeric_limits<uint8_t>::max();

    while ((port1 == port2) && attempt_to_find_available_port > 0)
    {
        port2 = Drv::Test::get_free_port(true);
        ASSERT_NE(0, port2);
        --attempt_to_find_available_port;
    }
    if (port2 == port1) {
        GTEST_SKIP() << "Could not find two unique and available UDP ports. SKipping test.";
    }
    ASSERT_NE(port1, port2);

    // Configure the component
    this->component.configureSend("127.0.0.1", port1, 0, 100);
    this->component.configureRecv("127.0.0.1", port2);

    // Start up a receive thread
    if (recv_thread) {
        Os::TaskString name("receiver thread");
        this->component.start(name, true, Os::Task::TASK_DEFAULT, Os::Task::TASK_DEFAULT);
    }

    // Loop through a bunch of client disconnects
    for (U32 i = 0; i < iterations; i++) {
        Drv::UdpSocket udp2;
        U32 size = sizeof(m_data_storage);

        // Not testing with reconnect thread, we will need to open ourselves
        if (not recv_thread) {
            status1 = this->component.open();

            EXPECT_EQ(status1, Drv::SOCK_SUCCESS)
                << "UDP socket open error: " << strerror(errno)
                << "Port1: " << port1
                << "Port2: " << port2;

        } else {
            EXPECT_TRUE(Drv::Test::wait_on_change(this->component.getSocketHandler(), true, Drv::Test::get_configured_delay_ms()/10 + 1));
        }
        EXPECT_TRUE(this->component.getSocketHandler().isOpened());

        udp2.configureSend("127.0.0.1", port2, 0, 100);
        udp2.configureRecv("127.0.0.1", port1);
        status2 = udp2.open();

        EXPECT_EQ(status2, Drv::SOCK_SUCCESS)
            << "UDP socket open error: " << strerror(errno) << std::endl
            << "Port1: " << port1 << std::endl
            << "Port2: " << port2 << std::endl;

        // If all the opens worked, then run this
        if ((Drv::SOCK_SUCCESS == status1) && (Drv::SOCK_SUCCESS == status2) &&
            (this->component.getSocketHandler().isOpened())) {
            // Force the sockets not to hang, if at all possible
            Drv::Test::force_recv_timeout(this->component.getSocketHandler());
            Drv::Test::force_recv_timeout(udp2);
            m_data_buffer.setSize(sizeof(m_data_storage));
            Drv::Test::fill_random_buffer(m_data_buffer);
            Drv::SendStatus status = invoke_to_send(0, m_data_buffer);
            EXPECT_EQ(status, SendStatus::SEND_OK);
            status2 = udp2.recv(buffer, size);
            EXPECT_EQ(status2, Drv::SOCK_SUCCESS);
            EXPECT_EQ(size, m_data_buffer.getSize());
            Drv::Test::validate_random_buffer(m_data_buffer, buffer);
            // If receive thread is live, try the other way
            if (recv_thread) {
                m_spinner = false;
                m_data_buffer.setSize(sizeof(m_data_storage));
                udp2.send(m_data_buffer.getData(), m_data_buffer.getSize());
                while (not m_spinner) {}
            }
        }
        // Properly stop the client on the last iteration
        if ((1 + i) == iterations && recv_thread) {
            this->component.stop();
            this->component.join();
        } else {
            this->component.close();
        }
        udp2.close();
    }
    ASSERT_from_ready_SIZE(iterations);
}

UdpTester ::UdpTester()
    : UdpGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Udp"),
      m_data_buffer(m_data_storage, 0), m_spinner(true) {
    this->initComponents();
    this->connectPorts();
    ::memset(m_data_storage, 0, sizeof(m_data_storage));
}

UdpTester ::~UdpTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void UdpTester ::test_basic_messaging() {
    test_with_loop(1);
}

void UdpTester ::test_multiple_messaging() {
    test_with_loop(100);
}

void UdpTester ::test_receive_thread() {
    test_with_loop(1, true);
}

void UdpTester ::test_advanced_reconnect() {
    test_with_loop(10, true); // Up to 10 * RECONNECT_MS
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void UdpTester ::from_recv_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& recvBuffer, const RecvStatus& recvStatus) {
    this->pushFromPortEntry_recv(recvBuffer, recvStatus);
    // Make sure we can get to unblocking the spinner
    EXPECT_EQ(m_data_buffer.getSize(), recvBuffer.getSize()) << "Invalid transmission size";
    Drv::Test::validate_random_buffer(m_data_buffer, recvBuffer.getData());
    m_spinner = true;
    delete[] recvBuffer.getData();
}

void UdpTester ::from_ready_handler(const NATIVE_INT_TYPE portNum) {
    this->pushFromPortEntry_ready();
}

Fw::Buffer UdpTester ::
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

  void UdpTester ::
    from_deallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_deallocate(fwBuffer);
  }

}  // end namespace Drv
