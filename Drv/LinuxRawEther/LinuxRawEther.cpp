// ======================================================================
// \title  LinuxRawEther.cpp
// \brief  Linux AF_PACKET raw ethernet driver (MDJ rover, bulletproof)
// ======================================================================
#include <Drv/LinuxRawEther/LinuxRawEther.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <errno.h>

namespace Drv {

LinuxRawEther::LinuxRawEther(const char* const compName)
    : LinuxRawEtherComponentBase(compName) {}

LinuxRawEther::~LinuxRawEther() {
    if (m_fd >= 0) ::close(m_fd);
}

bool LinuxRawEther::open(const char* const interface, FwSizeType allocationSize) {
    FW_ASSERT(interface != nullptr);
    FW_ASSERT(allocationSize > 0);
    m_interface = interface;
    m_allocationSize = allocationSize;

    m_fd = ::socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (m_fd < 0) {
        Fw::String arg(interface);
        this->log_WARNING_HI_OpenError(arg, errno);
        return false;
    }

    struct ifreq ifr{};
    (void)strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (::ioctl(m_fd, SIOCGIFINDEX, &ifr) < 0) {
        Fw::String arg(interface);
        this->log_WARNING_HI_BindError(arg, errno);
        ::close(m_fd); m_fd = -1;
        return false;
    }

    struct sockaddr_ll sll{};
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);
    if (::bind(m_fd, reinterpret_cast<struct sockaddr*>(&sll), sizeof(sll)) < 0) {
        Fw::String arg(interface);
        this->log_WARNING_HI_BindError(arg, errno);
        ::close(m_fd); m_fd = -1;
        return false;
    }

    return true;
}

void LinuxRawEther::start(FwTaskPriorityType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity) {
    Os::Task::Arguments args("LinuxRawEtherRecv", recvTaskEntry, this, priority, stackSize, cpuAffinity);
    Os::Task::Status stat = m_recvTask.start(args);
    FW_ASSERT(stat == Os::Task::OP_OK, static_cast<FwAssertArgType>(stat));
}

void LinuxRawEther::quitReadThread() { m_quit = true; }
Os::Task::Status LinuxRawEther::join() { return m_recvTask.join(); }

void LinuxRawEther::run_handler(FwIndexType portNum, U32 context) {
    this->tlmWrite_BytesSent(static_cast<U32>(m_bytesSent.load()));
    this->tlmWrite_BytesRecv(static_cast<U32>(m_bytesRecv.load()));
    this->tlmWrite_SendErrors(m_sendErrors.load());
    this->tlmWrite_RecvErrors(m_recvErrors.load());
}

Drv::ByteStreamStatus LinuxRawEther::send_handler(FwIndexType portNum, Fw::Buffer& serBuffer) {
    FW_ASSERT(m_fd >= 0);
    FwSizeType size = serBuffer.getSize();
    U8* data = serBuffer.getData();
    // Bounded, no heap, explicit error handling (CPP-1, CPP-32)
    ssize_t sent = ::send(m_fd, data, size, 0);
    if (sent < 0) {
        m_sendErrors++;
        this->log_WARNING_HI_SendError(errno);
        return Drv::ByteStreamStatus::OTHER_ERROR;
    }
    if (static_cast<FwSizeType>(sent) != size) {
        m_sendErrors++;
        this->log_WARNING_HI_SendError(EIO);
        return Drv::ByteStreamStatus::SEND_RETRY;
    }
    m_bytesSent += static_cast<FwSizeType>(sent);
    return Drv::ByteStreamStatus::OP_OK;
}

void LinuxRawEther::recvReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Return buffer to deallocate port
    this->deallocate_out(0, fwBuffer);
}

void LinuxRawEther::recvTaskEntry(void* ptr) {
    LinuxRawEther* self = static_cast<LinuxRawEther*>(ptr);
    self->recvTask();
}

void LinuxRawEther::recvTask() {
    while (!m_quit.load()) {
        if (m_fd < 0) { ::usleep(100000); continue; }
        Fw::Buffer buf;
        // Allocate via Fw::BufferGet (bounded, no heap post-init)
        Fw::BufferGetStatus stat = this->allocate_out(0, buf, static_cast<FwSizeType>(m_allocationSize));
        if (stat != Fw::BufferGetStatus::SUCCESS) { ::usleep(10000); continue; }
        // Bounded recv
        ssize_t recvd = ::recv(m_fd, buf.getData(), buf.getSize(), 0);
        if (recvd < 0) {
            if (errno == EAGAIN || errno == EINTR) { this->deallocate_out(0, buf); continue; }
            m_recvErrors++;
            this->log_WARNING_HI_RecvError(errno);
            this->deallocate_out(0, buf);
            ::usleep(10000);
            continue;
        }
        if (recvd == 0) { this->deallocate_out(0, buf); continue; }
        buf.setSize(static_cast<FwSizeType>(recvd));
        m_bytesRecv += static_cast<FwSizeType>(recvd);
        // Forward to ByteStreamDriver recv port
        this->recv_out(0, buf, Drv::ByteStreamStatus::OP_OK);
        // Ownership transferred to recv_out; do not deallocate here
    }
}

}  // namespace Drv
