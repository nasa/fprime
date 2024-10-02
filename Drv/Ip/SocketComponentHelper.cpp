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
    is_started = this->m_started;
    return is_started;
}

SocketIpStatus SocketComponentHelper::open() {
    SocketIpStatus status = SOCK_FAILED_TO_GET_SOCKET;
    NATIVE_INT_TYPE fd = -1;
    this->m_lock.lock();
    if (not this->m_open) {
        FW_ASSERT(this->m_fd == -1 and not this->m_open); // Ensure we are not opening an opened socket
        status = this->getSocketHandler().open(fd);
        this->m_fd = fd;
        // Call connected any time the open is successful
        if (Drv::SOCK_SUCCESS == status) {
            this->m_open = true;
            this->m_lock.unlock();
            this->connected();
            return status;
        }
    }
    this->m_lock.unlock();
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
    if (this->isStarted() and (not this->isOpened())) {
        status = this->open();
    }
    return status;
}

SocketIpStatus SocketComponentHelper::send(const U8* const data, const U32 size) {
    SocketIpStatus status = SOCK_SUCCESS;
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    // Prevent transmission before connection, or after a disconnect
    if (fd == -1) {
        status = this->reconnect();
        // if reconnect wasn't successful, pass the that up to the caller
        if(status != SOCK_SUCCESS) {
            return status;
        }
        this->m_lock.lock();
        fd = this->m_fd;
        this->m_lock.unlock();
    }
    status = this->getSocketHandler().send(fd, data, size);
    if (status == SOCK_DISCONNECTED) {
        this->close();
    }
    return status;
}

void SocketComponentHelper::shutdown() {
    this->m_lock.lock();
    this->getSocketHandler().shutdown(this->m_fd);
    this->m_started = false;
    this->m_fd = -1;
    this->m_lock.unLock();
}

void SocketComponentHelper::close() {
    this->m_lock.lock();
    if (this->m_fd != -1) {
        this->getSocketHandler().close(this->m_fd);
        this->m_fd = -1;
    }
    this->m_open = false;
    this->m_lock.unlock();
}

Os::Task::Status SocketComponentHelper::join() {
    return m_task.join();
}

void SocketComponentHelper::stop() {
    this->m_lock.lock();
    this->m_stop = true;
    this->m_lock.unlock();
    this->shutdown();  // Break out of any receives and fully shutdown
}

SocketIpStatus SocketComponentHelper::recv(U8* data, U32 &size) {
    SocketIpStatus status = SOCK_SUCCESS;
    // Check for previously disconnected socket
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    if (fd == -1) {
        return SOCK_DISCONNECTED;
    }
    status = this->getSocketHandler().recv(fd, data, size);
    if (status == SOCK_DISCONNECTED) {
        this->close();
    }
    return status;
}

void SocketComponentHelper::readTask(void* pointer) {
    FW_ASSERT(pointer);
    SocketIpStatus status = SOCK_SUCCESS;
    SocketComponentHelper* self = reinterpret_cast<SocketComponentHelper*>(pointer);
    do {
        // Prevent transmission before connection, or after a disconnect
        if ((not self->isOpened()) and (not self->m_stop)) {
            status = self->reconnect();
            if(status != SOCK_SUCCESS) {
                Fw::Logger::log(
                    "[WARNING] Failed to open port with status %d and errno %d\n",
                    status,
                    errno);
                (void) Os::Task::delay(SOCKET_RETRY_INTERVAL);
                continue;
            }
        }
        // If the network connection is open, read from it
        if (self->isStarted() and self->isOpened() and (not self->m_stop)) {
            Fw::Buffer buffer = self->getBuffer();
            U8* data = buffer.getData();
            FW_ASSERT(data);
            U32 size = buffer.getSize();
            // recv blocks, so it may have been a while since its done an isOpened check
            status = self->recv(data, size);
            if ((status != SOCK_SUCCESS) && (status != SOCK_INTERRUPTED_TRY_AGAIN && (status != SOCK_NO_DATA_AVAILABLE))) {
                Fw::Logger::log("[WARNING] Failed to recv from port with status %d and errno %d\n",
                    status,
                    errno);
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
