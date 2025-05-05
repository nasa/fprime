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

namespace Drv {

SocketComponentHelper::SocketComponentHelper() {}

SocketComponentHelper::~SocketComponentHelper() {}

void SocketComponentHelper::start(const Fw::StringBase &name,
                                  const Os::Task::ParamType priority,
                                  const Os::Task::ParamType stack,
                                  const Os::Task::ParamType cpuAffinity) {
    FW_ASSERT(m_task.getState() == Os::Task::State::NOT_STARTED);  // It is a coding error to start this task multiple times
    this->m_stop = false;
    // Note: the first step is for the IP socket to open the port
    Os::Task::Arguments arguments(name, SocketComponentHelper::readTask, this, priority, stack, cpuAffinity);
    Os::Task::Status stat = m_task.start(arguments);
    FW_ASSERT(Os::Task::OP_OK == stat, static_cast<FwAssertArgType>(stat));
}

SocketIpStatus SocketComponentHelper::open() {
    SocketIpStatus status = SOCK_ANOTHER_THREAD_OPENING;
    OpenState local_open = OpenState::OPEN;
    // Scope to guard lock
    {
        Os::ScopeLock scopeLock(m_lock);
        if (this->m_open == OpenState::NOT_OPEN) {
            this->m_open = OpenState::OPENING;
            local_open = this->m_open;
        } else {
            local_open = OpenState::SKIP;
        }

    }
    if (local_open == OpenState::OPENING) {
        FW_ASSERT(this->m_descriptor.fd == -1);  // Ensure we are not opening an opened socket
        status = this->getSocketHandler().open(this->m_descriptor);
        // Lock scope
        {
            Os::ScopeLock scopeLock(m_lock);
            if (Drv::SOCK_SUCCESS == status) {
                this->m_open = OpenState::OPEN;
            } else {
                this->m_open = OpenState::NOT_OPEN;
                this->m_descriptor.fd = -1;
            }
        }
        // Notify connection on success outside locked scope
        if (Drv::SOCK_SUCCESS == status) {
            this->connected();
        }
    }

    return status;
}

bool SocketComponentHelper::isOpened() {
    Os::ScopeLock scopedLock(this->m_lock);
    bool is_open = this->m_open == OpenState::OPEN;
    return is_open;
}

void SocketComponentHelper::setAutomaticOpen(bool auto_open) {
    Os::ScopeLock scopedLock(this->m_lock);
    this->m_reopen = auto_open;
}

SocketIpStatus SocketComponentHelper::reopen() {
    SocketIpStatus status = SOCK_SUCCESS;
    if (not this->isOpened()) {
        // Check for auto-open before attempting to reopen
        bool reopen = false;
        {
            Os::ScopeLock scopedLock(this->m_lock);
            reopen = this->m_reopen;
        }
        // Open a network connection if it has not already been open
        if (not reopen) {
            status = SOCK_AUTO_CONNECT_DISABLED;
        } else {
            status = this->open();
            if (status == SocketIpStatus::SOCK_ANOTHER_THREAD_OPENING) {
                status = SocketIpStatus::SOCK_SUCCESS;
            }
        }
    }
    return status;
}

SocketIpStatus SocketComponentHelper::send(const U8* const data, const U32 size) {
    SocketIpStatus status = SOCK_SUCCESS;
    this->m_lock.lock();
    SocketDescriptor descriptor = this->m_descriptor;
    this->m_lock.unlock();
    // Prevent transmission before connection, or after a disconnect
    if (descriptor.fd == -1) {
        status = this->reopen();
        // if reopen wasn't successful, pass the that up to the caller
        if(status != SOCK_SUCCESS) {
            return status;
        }
        // Refresh local copy after reopen
        this->m_lock.lock();
        descriptor = this->m_descriptor;
        this->m_lock.unlock();
    }
    status = this->getSocketHandler().send(descriptor, data, size);
    if (status == SOCK_DISCONNECTED) {
        this->close();
    }
    return status;
}

void SocketComponentHelper::shutdown() {
    Os::ScopeLock scopedLock(this->m_lock);
    this->getSocketHandler().shutdown(this->m_descriptor);
}

void SocketComponentHelper::close() {
    Os::ScopeLock scopedLock(this->m_lock);
    this->getSocketHandler().close(this->m_descriptor);
    this->m_descriptor.fd = -1;
    this->m_open = OpenState::NOT_OPEN;
}

Os::Task::Status SocketComponentHelper::join() {
    return m_task.join();
}

void SocketComponentHelper::stop() {
    // Scope to protect lock
    {
        Os::ScopeLock scopeLock(m_lock);
        this->m_stop = true;
    }
    this->shutdown();  // Break out of any receives and fully shutdown
}

bool SocketComponentHelper::running() {
    Os::ScopeLock scopedLock(this->m_lock);
    bool running = not this->m_stop;
    return running;
}

SocketIpStatus SocketComponentHelper::recv(U8* data, U32 &size) {
    SocketIpStatus status = SOCK_SUCCESS;
    // Check for previously disconnected socket
    this->m_lock.lock();
    SocketDescriptor descriptor = this->m_descriptor;
    this->m_lock.unlock();
    if (descriptor.fd == -1) {
        return SOCK_DISCONNECTED;
    }
    status = this->getSocketHandler().recv(descriptor, data, size);
    if (status == SOCK_DISCONNECTED) {
        this->close();
    }
    return status;
}

void SocketComponentHelper::readLoop() {
    SocketIpStatus status = SOCK_SUCCESS;
    do {
        // Prevent transmission before connection, or after a disconnect
        if ((not this->isOpened()) and this->running()) {
            status = this->reopen();
            // When reopen is disabled, just break as this is a exit condition for the loop
            if (status == SOCK_AUTO_CONNECT_DISABLED) {
                break;
            }
            // If the reconnection failed in any other way, warn, wait, and retry
            else if (status != SOCK_SUCCESS) {
                Fw::Logger::log("[WARNING] Failed to open port with status %d and errno %d\n", status, errno);
                (void)Os::Task::delay(SOCKET_RETRY_INTERVAL);
                continue;
            }
        }
        // If the network connection is open, read from it
        if (this->isOpened() and this->running()) {
            Fw::Buffer buffer = this->getBuffer();
            U8* data = buffer.getData();
            FW_ASSERT(data);
            U32 size = buffer.getSize();
            // recv blocks, so it may have been a while since its done an isOpened check
            status = this->recv(data, size);
            if ((status != SOCK_SUCCESS) && (status != SOCK_INTERRUPTED_TRY_AGAIN) && (status != SOCK_NO_DATA_AVAILABLE)) {
                Fw::Logger::log("[WARNING] Failed to recv from port with status %d and errno %d\n",
                                status,
                                errno);
                this->close();
                buffer.setSize(0);
            } else {
                // Send out received data
                buffer.setSize(size);
            }
            this->sendBuffer(buffer, status);
        }
    }
    // This will loop until stopped. If auto-open is disabled, this will break when reopen returns disabled status
    while (this->running());
    // Close the socket
    this->close(); // Close the port entirely
}

void SocketComponentHelper::readTask(void* pointer) {
    FW_ASSERT(pointer);
    SocketComponentHelper* self = reinterpret_cast<SocketComponentHelper*>(pointer);
    self->readLoop();
}
}  // namespace Drv
