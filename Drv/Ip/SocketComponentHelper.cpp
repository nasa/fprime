// ======================================================================
// \title  SocketComponentHelper.cpp
// \author mstarch, crsmith
// \brief  cpp file for SocketComponentHelper implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/Ip/SocketComponentHelper.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <cerrno>

#define MAXIMUM_SIZE 0x7FFFFFFF

namespace Drv {

SocketComponentHelper::SocketComponentHelper() : m_fd(-1), m_reconnect(false), m_stop(false), m_started(false), m_open(false) {}

SocketComponentHelper::~SocketComponentHelper() {}

void SocketComponentHelper::start(const Fw::StringBase &name,
                                     const bool reconnect,
                                     const Os::Task::ParamType priority,
                                     const Os::Task::ParamType stack,
                                     const Os::Task::ParamType cpuAffinity) {
    FW_ASSERT(m_task.getState() == Os::Task::State::NOT_STARTED);  // It is a coding error to start this task multiple times
    FW_ASSERT(not this->m_stop);        // It is a coding error to stop the thread before it is started
    m_reconnect = reconnect;
    // Note: the first step is for the IP socket to open the port
    Os::Task::Arguments arguments(name, SocketComponentHelper::readTask, this, priority, stack, cpuAffinity);
    Os::Task::Status stat = m_task.start(arguments);
    FW_ASSERT(Os::Task::OP_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
}

SocketIpStatus SocketComponentHelper::startup() {
    this->m_started = true;
    return this->getSocketHandler().startup();
}

bool SocketComponentHelper::isStarted() {
    bool is_started = false;
    this->m_lock.lock();
    is_started = this->m_started;
    this->m_lock.unlock();
    return is_started;
}

SocketIpStatus SocketComponentHelper::open() {
    NATIVE_INT_TYPE fd = -1;
    this->m_lock.lock();
    FW_ASSERT(this->m_fd == -1 and not this->m_open); // Ensure we are not opening an opened socket
    this->m_lock.unlock();
    SocketIpStatus status = this->getSocketHandler().open(fd);
    // Call connected any time the open is successful
    if (Drv::SOCK_SUCCESS == status) {
        this->m_lock.lock();
        this->m_fd = fd;
        this->m_open = true;
        this->m_lock.unlock();
        this->connected();
    }
    return status;
}

bool SocketComponentHelper::isOpened() {
    bool is_open = false;
    this->m_lock.lock();
    is_open = this->m_open;
    this->m_lock.unlock();
    return is_open;
}

SocketIpStatus SocketComponentHelper::reconnect() {
    SocketIpStatus status = SOCK_SUCCESS;

    // Handle opening

    // Open a network connection if it has not already been open
    if ((not this->isOpened()) and
        ((status = this->open()) != SOCK_SUCCESS)) {
        Fw::Logger::log(
            "[WARNING] Failed to open port with status %d and errno %d\n",
            static_cast<POINTER_CAST>(status),
            static_cast<POINTER_CAST>(errno));
        (void) Os::Task::delay(SOCKET_RETRY_INTERVAL);
        return status;
    }
    return status;
}

SocketIpStatus SocketComponentHelper::send(const U8* const data, const U32 size) {
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    // Prevent transmission before connection, or after a disconnect
    if (fd == -1) {
        this->reconnect();
    }
    return this->getSocketHandler().send(fd, data, size);
}

void SocketComponentHelper::shutdown() {
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    this->getSocketHandler().shutdown(fd);
    this->m_lock.lock();
    this->m_started = false;
    this->m_fd = fd;
    this->m_lock.unLock();
}

void SocketComponentHelper::close() {
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    if (fd != -1) {
        this->getSocketHandler().close(fd);
    }
    this->m_fd = fd;
    this->m_open = false;
    this->m_lock.unlock();
}

Os::Task::Status SocketComponentHelper::join() {
    return m_task.join();
}

void SocketComponentHelper::stop() {
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    this->m_stop = true;
    this->getSocketHandler().shutdown(fd);  // Break out of any receives and fully shutdown
}

SocketIpStatus SocketComponentHelper::recv(U8* data, U32 size) {
    SocketIpStatus status = SOCK_SUCCESS;
    // Check for previously disconnected socket
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    if (fd == -1) {
        return SOCK_DISCONNECTED;
    }
    status = this->getSocketHandler().recv(fd, data, size);
    // fd could change to closed in recv 
    this->m_lock.lock();
    this->m_fd = fd;
    this->m_lock.unlock();
    return status;
}

void SocketComponentHelper::readTask(void* pointer) {
    FW_ASSERT(pointer);
    SocketIpStatus status = SOCK_SUCCESS;
    SocketComponentHelper* self = reinterpret_cast<SocketComponentHelper*>(pointer);
    do {
        self->m_lock.lock();
        NATIVE_INT_TYPE fd = self->m_fd;
        self->m_lock.unlock();
        // Prevent transmission before connection, or after a disconnect
        if (fd == -1 and (not self->m_stop)) {
            self->reconnect();
        }
        // If the network connection is open, read from it
        if (self->isStarted() and self->isOpened() and (not self->m_stop)) {
            Fw::Buffer buffer = self->getBuffer();
            U8* data = buffer.getData();
            FW_ASSERT(data);
            U32 size = buffer.getSize();
            self->recv(data, size);
            if ((status != SOCK_SUCCESS) && (status != SOCK_INTERRUPTED_TRY_AGAIN)) {
                Fw::Logger::log("[WARNING] Failed to recv from port with status %d and errno %d\n",
                static_cast<POINTER_CAST>(status),
                static_cast<POINTER_CAST>(errno));
                self->close();
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
    self->shutdown(); // Shutdown the port entirely
}
}  // namespace Drv
