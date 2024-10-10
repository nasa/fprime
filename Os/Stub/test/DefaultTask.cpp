// ======================================================================
// \title Os/Stub/test/DefaultTask.cpp
// \brief sets default Os::Task to test stub implementation via linker
// ======================================================================
#include <cerrno>
#include "Os/Task.hpp"
#include "Os/Stub/test/Task.hpp"
#include "Os/Delegate.hpp"
#include <sys/time.h>

namespace Os {
    Os::Task::Status TaskInterface::delay(Fw::Time interval) {
        Os::Stub::Task::Test::StaticData::data.lastCalled = Os::Stub::Task::Test::StaticData::LastFn::DELAY_FN;
        Os::Stub::Task::Test::StaticData::data.delay = interval;

        // For testing stub, the default implementation of delay for a "task" is a busy wait. This acts as a synthetic
        // albeit inefficient implementation.
        timeval start;
        timeval end;
        if (gettimeofday(&start, nullptr) == 0) {
            end.tv_usec = (start.tv_usec + interval.getUSeconds()) % 1000000;
            end.tv_sec = start.tv_sec + interval.getSeconds() + (start.tv_usec + interval.getUSeconds())/1000000;
            // Bounded busy wait
            for (U64 wait = 0; wait < std::numeric_limits<U64>::max(); wait++) {
                gettimeofday(&start, nullptr);
                if (((start.tv_sec >= end.tv_sec) && (start.tv_usec >= end.tv_usec)) ||
                    (start.tv_sec > end.tv_sec)) {
                    break;
                }
            }
        }
        return Os::Stub::Task::Test::StaticData::data.delayStatus;
    }

    TaskInterface* TaskInterface::getDelegate(TaskHandleStorage& aligned_new_memory) {
        return Os::Delegate::makeDelegate<TaskInterface, Os::Stub::Task::Test::TestTask>(aligned_new_memory);
    }

}
