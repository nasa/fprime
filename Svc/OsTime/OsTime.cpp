// ======================================================================
// \title  OsTime.cpp
// \author kubiak
// \brief  cpp file for OsTime component implementation class
// ======================================================================

#include "Svc/OsTime/OsTime.hpp"
#include "FpConfig.hpp"

#include <Fw/Time/TimeInterval.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

OsTime ::OsTime(const char* const compName) :
    OsTimeComponentBase(compName),
    m_epoch_fw_time(Fw::ZERO_TIME),
    m_epoch_os_time(),
    m_epoch_valid(false),
    m_epoch_lock()
{}

OsTime ::~OsTime()
{}

void OsTime::set_epoch(const Fw::Time& fw_time, const Os::RawTime& os_time) {
    Os::ScopeLock lock(m_epoch_lock);
    m_epoch_fw_time = fw_time;
    m_epoch_os_time = os_time;
    m_epoch_valid = true;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void OsTime ::timeGetPort_handler(FwIndexType portNum, Fw::Time& time) {
    Fw::Time temp_epoch_fw_time;
    Os::RawTime temp_epoch_os_time;
    bool temp_epoch_valid;

    // Copy class state inside of a mutex
    {
        Os::ScopeLock lock(m_epoch_lock);
        temp_epoch_fw_time = m_epoch_fw_time;
        temp_epoch_os_time = m_epoch_os_time;
        temp_epoch_valid = m_epoch_valid;
    }

    time = Fw::ZERO_TIME;
    if (!temp_epoch_valid) {
        return;
    }

    Os::RawTime time_now;
    Os::RawTime::Status stat = time_now.now();
    if (stat != Os::RawTime::OP_OK) {
        return;
    }

    Fw::TimeInterval elapsed;
    stat = time_now.getTimeInterval(temp_epoch_os_time, elapsed);
    if (stat != Os::RawTime::OP_OK) {
        return;
    }

    time = temp_epoch_fw_time;
    time.add(elapsed.getSeconds(), elapsed.getUSeconds());
}

void OsTime ::setEpoch_handler(FwIndexType portNum, const Fw::Time& fw_time, const Os::RawTime& os_time) {
    set_epoch(fw_time, os_time);
}

}  // namespace Svc
