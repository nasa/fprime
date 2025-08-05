// ======================================================================
// \title  LinuxTimerImpl.hpp
// \author tim
// \brief  hpp file for LinuxTimer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxTimer_HPP
#define LinuxTimer_HPP

#include "Os/Mutex.hpp"
#include "Os/RawTime.hpp"
#include "Svc/LinuxTimer/LinuxTimerComponentAc.hpp"

namespace Svc {

class LinuxTimer final : public LinuxTimerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object LinuxTimer
    //!
    LinuxTimer(const char* const compName /*!< The component name*/
    );

    //! Destroy object LinuxTimer
    //!
    ~LinuxTimer();

    //! Start timer
    void startTimer(FwSizeType interval);  //!< interval in milliseconds

    //! Quit timer
    void quit();

  private:
    Os::Mutex m_mutex;  //!< mutex for quit flag

    volatile bool m_quit;  //!< flag to quit

    Os::RawTime m_rawTime;  //!< timestamp to pass to CycleOut port calls
};

}  // end namespace Svc

#endif
