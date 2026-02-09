// ======================================================================
// \title  Timer.cpp
// \author campuzan
// \brief  cpp file for the Timer class implementation
// ======================================================================

#include <Svc/Ccsds/CfdpManager/CfdpTimer.hpp>

#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Class construction and destruction
// ----------------------------------------------------------------------

Timer ::Timer() : timerStatus(UNITIALIZED), secondsRemaining(0) {}

Timer ::~Timer() {}

// ----------------------------------------------------------------------
// Class interfaces
// ----------------------------------------------------------------------

void Timer ::setTimer(U32 timerDuration)
{
    this->timerStatus = RUNNING;
    this->secondsRemaining = timerDuration;
}

void Timer ::disableTimer(void)
{
    this->timerStatus = EXPIRED;
    this->secondsRemaining = 0;
}

Timer::Status Timer ::getStatus(void)
{
    return this->timerStatus;
}

void Timer ::run(void)
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

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
