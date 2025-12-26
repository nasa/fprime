// ======================================================================
// \title  CfdpTimer.hpp
// \author campuzan
// \brief  hpp file for CFDP timer that is driven by 
// ======================================================================

#ifndef CCSDS_CFDPTIMER_HPP
#define CCSDS_CFDPTIMER_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace Ccsds {

class CfdpTimer  {
  // ----------------------------------------------------------------------
  // Class types
  // ----------------------------------------------------------------------
  public:
    enum Status {
      UNITIALIZED,
      RUNNING,
      EXPIRED
    };

  public:
    // ----------------------------------------------------------------------
    // Class construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CfdpTimer object
    CfdpTimer();

    //! Destroy CfdpTimer object
    ~CfdpTimer();

  public:
    // ----------------------------------------------------------------------
    // Class interfaces
    // ----------------------------------------------------------------------

    //! Initialize a CFDP timer and start its execution
    void setTimer(U32 timerDuration //!< The duration of the timer in seconds
    );

    //! Get the status of a CFDP timer
    Status getStatus(void);

    //! Runs a one second increment of the CFDP timers
    void run(void);

    private:
    // ----------------------------------------------------------------------
    // Class member variables
    // ----------------------------------------------------------------------

    //! Number of seconds until the timer expires
    Status timerStatus;

    //! Number of seconds until the timer expires
    U32 secondsRemaining;
};

}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPTIMER_HPP
