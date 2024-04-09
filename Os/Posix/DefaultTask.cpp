// ======================================================================
// \title Os/Posix/DefaultTask.cpp
// \brief sets default Os::Task to posix implementation via linker
// ======================================================================
#include <cerrno>
#include <new>

#include "Os/Task.hpp"
#include "Os/Posix/Task.hpp"
#include "Fw/Types/Assert.hpp"

#include <cstdio>

namespace Os {
    Os::Task::Status TaskInterface::delay(Fw::Time interval) {
        Os::Task::Status task_status = Os::Task::OP_OK;
        timespec sleep_interval;
        sleep_interval.tv_sec = interval.getSeconds();
        sleep_interval.tv_nsec = interval.getUSeconds() * 1000;

        timespec remaining_interval;
        remaining_interval.tv_sec = 0;
        remaining_interval.tv_nsec = 0;

        while (true) {
            PlatformIntType status = nanosleep(&sleep_interval, &remaining_interval);
            // Success, return ok
            if (0 == status) {
                break;
            }
            // Interrupted, reset sleep and iterate
            else if (EINTR == errno) {
                sleep_interval = remaining_interval;
                continue;
            }
            // Anything else is an error
            else {
                task_status = Os::Task::Status::DELAY_ERROR;
                break;
            }
        }
        return task_status;
    }

    //! \brief get implementation file interface for use as delegate
    //!
    //! Added via linker to ensure that the posix implementation of Os::File is the default.
    //!
    //! \param aligned_placement_new_memory: memory to placement-new into
    //! \return FileInterface pointer result of placement new
    //!
    TaskInterface* TaskInterface::getDelegate(U8* aligned_placement_new_memory) {
        FW_ASSERT(aligned_placement_new_memory != nullptr);
        // Placement-new the file handle into the opaque file-handle storage
        static_assert(sizeof(Os::Posix::Task::PosixTask) <= sizeof Os::Task::m_handle_storage,
                      "Handle size not large enough");
        static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Posix::Task::PosixTask)) == 0, "Handle alignment invalid");
        Os::Posix::Task::PosixTask *interface = new(aligned_placement_new_memory) Os::Posix::Task::PosixTask;
        FW_ASSERT(interface != nullptr);
        return interface;
    }
}
