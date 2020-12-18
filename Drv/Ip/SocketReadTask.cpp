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
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "errno.h"

namespace Drv {

SocketReadTask::SocketReadTask() : m_stop(false) {}

SocketReadTask::~SocketReadTask() {}

void SocketReadTask::startSocketTask(const Fw::StringBase &name,
                                     const NATIVE_INT_TYPE priority,
                                     const NATIVE_INT_TYPE stack,
                                     const bool reconnect,
                                     const NATIVE_INT_TYPE cpuAffinity) {
    FW_ASSERT(not m_task.isStarted());  // It is a coding error to start this task multiple times
    FW_ASSERT(not this->m_stop);        // It is a coding error to stop the thread before it is started
    m_reconnect = reconnect;
    // Note: the first step is for the IP socket to open the the port
    Os::Task::TaskStatus stat = m_task.start(name, 0, priority, stack, SocketReadTask::readTask, this, cpuAffinity);
    FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
}

SocketIpStatus SocketReadTask::open() {
    return this->getSocketHandler().open();
}

void SocketReadTask::close() {
    this->getSocketHandler().close();
}

Os::Task::TaskStatus SocketReadTask::joinSocketTask(void** value_ptr) {
    return m_task.join(value_ptr);
}

void SocketReadTask::stopSocketTask() {
    this->m_stop = true;
    this->getSocketHandler().close();
}

void SocketReadTask::readTask(void* pointer) {
    FW_ASSERT(pointer);
    SocketIpStatus status = SOCK_SUCCESS;
    SocketReadTask* self = reinterpret_cast<SocketReadTask*>(pointer);
    do {
        // Open a network connection if it has not already been open
        if ((not self->m_stop) and (not self->getSocketHandler().isOpened()) and
            ((status = self->getSocketHandler().open()) != SOCK_SUCCESS)) {
            Fw::Logger::logMsg("[WARNING] Failed to open port with status %d and errno %d\n", status, errno);
            Os::Task::delay(SOCKET_RETRY_INTERVAL_MS);
        }

        // If the network connection is open, read from it
        if (self->getSocketHandler().isOpened()) {
            Fw::Buffer buffer = self->getBuffer();
            U8* data = buffer.getData();
            FW_ASSERT(data);
            I32 size = buffer.getSize();
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
}
};  // namespace Drv