// ======================================================================
// \title  LinuxRawEther.hpp
// \brief  header for LinuxRawEther raw ethernet driver (F Prime, MDJ rover)
// ======================================================================
#ifndef LinuxRawEther_HPP
#define LinuxRawEther_HPP

#include <Drv/LinuxRawEther/LinuxRawEtherComponentAc.hpp>
#include <Os/Mutex.hpp>
#include <Os/Task.hpp>
#include <atomic>

namespace Drv {

class LinuxRawEther final : public LinuxRawEtherComponentBase {
  public:
    explicit LinuxRawEther(const char* const compName);
    ~LinuxRawEther() override;

    // Open raw socket on interface (e.g. "eth0"), allocationSize for recv buffers
    bool open(const char* const interface, FwSizeType allocationSize);

    // Start receive thread
    void start(FwTaskPriorityType priority = Os::Task::TASK_PRIORITY_DEFAULT,
               Os::Task::ParamType stackSize = Os::Task::TASK_DEFAULT,
               Os::Task::ParamType cpuAffinity = Os::Task::TASK_DEFAULT);

    void quitReadThread();
    Os::Task::Status join();

  private:
    void run_handler(FwIndexType portNum, U32 context) override;
    Drv::ByteStreamStatus send_handler(FwIndexType portNum, Fw::Buffer& serBuffer) override;
    void recvReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) override;

    static void recvTaskEntry(void* ptr);
    void recvTask();

    int m_fd = -1;
    FwSizeType m_allocationSize = 0;
    const char* m_interface = nullptr;

    Os::Task m_recvTask;
    std::atomic<bool> m_quit{false};
    std::atomic<FwSizeType> m_bytesSent{0};
    std::atomic<FwSizeType> m_bytesRecv{0};
    std::atomic<U32> m_sendErrors{0};
    std::atomic<U32> m_recvErrors{0};
};

}  // namespace Drv

#endif
