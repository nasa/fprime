// ======================================================================
// \title  ChronoTime.cpp
// \author mstarch
// \brief  cpp file for ChronoTime component implementation class
// ======================================================================

#include "Svc/ChronoTime/ChronoTime.hpp"
#include <chrono>
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ChronoTime ::ChronoTime(const char* const compName) : ChronoTimeComponentBase(compName) {}

ChronoTime ::~ChronoTime() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ChronoTime ::timeGetPort_handler(FwIndexType portNum, Fw::Time& time) {
    const auto time_now = std::chrono::system_clock::now();
    time.set(TimeBase::TB_WORKSTATION_TIME,
             static_cast<U32>(std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch()).count()),
             static_cast<U32>(
                 std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch()).count() % 1000000));
}

}  // namespace Svc
