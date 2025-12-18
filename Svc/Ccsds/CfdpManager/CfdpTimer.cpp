// ======================================================================
// \title  CfdpTimer.cpp
// \author campuzan
// \brief  cpp file for the CfdpTimer class implementation
// ======================================================================

#include "Svc/Ccsds/CfdpManager/CfdpTimer.hpp"

#include "Fw/Types/Assert.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Class construction and destruction
// ----------------------------------------------------------------------

CfdpTimer ::CfdpTimer() : timerStatus(UNITIALIZED), secondsRemaining(0) {}

CfdpTimer ::~CfdpTimer() {}

// ----------------------------------------------------------------------
// Class interfaces
// ----------------------------------------------------------------------

void CfdpTimer ::setTimer(U32 timerDuration)
{
    // TODO Do we care about the current timer status at this point
    // Assuming no for now
    this->timerStatus = RUNNING;
    this->secondsRemaining = timerDuration;
}

CfdpTimer::CfdpTimerStatus CfdpTimer ::getStatus(void)
{
    return this->timerStatus;
}

void CfdpTimer ::run(void)
{
    if(this->timerStatus == RUNNING)
    {
        FW_ASSERT(this->secondsRemaining > 0);
        this->secondsRemaining--;

        if(this->secondsRemaining == 0)
        {
            this->timerStatus = EXPIRED;
        }
    }
}

}  // namespace Ccsds
}  // namespace Svc
