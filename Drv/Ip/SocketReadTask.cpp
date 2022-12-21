// ======================================================================
// \title  SocketReadTask.cpp
// \author mstarch
// \brief  cpp file for SocketReadTask implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/Ip/SocketReadTask.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <cerrno>

#define MAXIMUM_SIZE 0x7FFFFFFF

namespace Drv {

SocketReadTask::SocketReadTask() : m_reconnect(false), m_stop(false) {}

SocketReadTask::~SocketReadTask() {}

void SocketReadTask::startSocketTask(const Fw::StringBase &name,
                                     const bool reconnect,
                                     const NATIVE_UINT_TYPE priority,
                                     const NATIVE_UINT_TYPE stack,
                                     const NATIVE_UINT_TYPE cpuAffinity) {
    FW_ASSERT(not m_task.isStarted());  // It is a coding error to start this task multiple times
    FW_ASSERT(not this->m_stop);        // It is a coding error to stop the thread before it is started
    m_reconnect = reconnect;
    // Note: the first step is for the IP socket to open the port
    Os::Task::TaskStatus stat = m_task.start(name, SocketReadTask::readTask, this, priority, stack, cpuAffinity);
    FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
}

SocketIpStatus SocketReadTask::open() {
    SocketIpStatus status = this->getSocketHandler().open();
    // Call connected any time the open is successful
    if (Drv::SOCK_SUCCESS == status) {
        this->connected();
    }
    return status;
}

void SocketReadTask::close() {
    this->getSocketHandler().close();
}

Os::Task::TaskStatus SocketReadTask::joinSocketTask(void** value_ptr) {
    return m_task.join(value_ptr);
}

void SocketReadTask::stopSocketTask() {
    this->m_stop = true;
    this->getSocketHandler().close();  // Break out of any receives
}

void SocketReadTask::readTask(void* pointer) {
    FW_ASSERT(pointer);
    SocketIpStatus status = SOCK_SUCCESS;
    SocketReadTask* self = reinterpret_cast<SocketReadTask*>(pointer);
    do {
        // Open a network connection if it has not already been open
        if ((not self->getSocketHandler().isOpened()) and (not self->m_stop) and
            ((status = self->open()) != SOCK_SUCCESS)) {
            Fw::Logger::logMsg("[WARNING] Failed to open port with status %d and errno %d\n", status, errno);
            Os::Task::delay(SOCKET_RETRY_INTERVAL_MS);
        }

        // If the network connection is open, read from it
        if (self->getSocketHandler().isOpened() and (not self->m_stop)) {
            Fw::Buffer buffer = self->getBuffer();
            U8* data = buffer.getData();
            FW_ASSERT(data);
            I32 size = static_cast<I32>(buffer.getSize());
            size = (size >= 0) ? size : MAXIMUM_SIZE; // Handle max U32 edge case
            status = self->getSocketHandler().recv(data, size);
            if ((status != SOCK_SUCCESS) && (status != SOCK_INTERRUPTED_TRY_AGAIN)) {
                Fw::Logger::logMsg("[WARNING] Failed to recv from port with status %d and errno %d\n", status, errno);
                self->getSocketHandler().close();
                buffer.setSize(0);
            } else {
                // Send out received data
                buffer.setSize(size);
            }
            self->sendBuffer(buffer, status);
        }
    }
    // As long as not told to stop, and we are successful interrupted or ordered to retry, keep receiving
    while (not self->m_stop &&
           (status == SOCK_SUCCESS || status == SOCK_INTERRUPTED_TRY_AGAIN || self->m_reconnect));
    self->getSocketHandler().close(); // Close the handler again, in case it reconnected
}
};  // namespace Drv
