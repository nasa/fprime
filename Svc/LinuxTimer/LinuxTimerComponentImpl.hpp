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
#include "Svc/LinuxTimer/LinuxTimerComponentAc.hpp"

namespace Svc {

  class LinuxTimerComponentImpl :
    public LinuxTimerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxTimer
      //!
      LinuxTimerComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object LinuxTimer
      //!
      ~LinuxTimerComponentImpl();

      //! Start timer
      void startTimer(NATIVE_INT_TYPE interval); //!< interval in milliseconds

      //! Quit timer
      void quit();

    PRIVATE:

      Os::Mutex m_mutex; //!< mutex for quit flag

      volatile bool m_quit; //!< flag to quit

      Svc::TimerVal m_timer;


    };

} // end namespace Svc

#endif
