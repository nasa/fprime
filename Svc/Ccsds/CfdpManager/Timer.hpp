// ======================================================================
// \title  Timer.hpp
// \author Brian Campuzano
// \brief  hpp file for CFDP timer that is driven by 
// ======================================================================

#ifndef CCSDS_CFDPTIMER_HPP
#define CCSDS_CFDPTIMER_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

class Timer  {
  // ----------------------------------------------------------------------
  // Class types
  // ----------------------------------------------------------------------
  public:
    enum Status {
      UNINITIALIZED,
      RUNNING,
      EXPIRED
    };

  public:
    // ----------------------------------------------------------------------
    // Class construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Timer object
    Timer();

    //! Destroy Timer object
    ~Timer();

  public:
    // ----------------------------------------------------------------------
    // Class interfaces
    // ----------------------------------------------------------------------

    //! Initialize a CFDP timer and start its execution
    void setTimer(U32 timerDuration //!< The duration of the timer in seconds
    );

    //! Disables a CFDP timer
    void disableTimer(void);

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

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPTIMER_HPP
