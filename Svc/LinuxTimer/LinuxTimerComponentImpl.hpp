// ====================================================================== 
// \title  LinuxTimerImpl.hpp
// \author tim
// \brief  hpp file for LinuxTimer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef LinuxTimer_HPP
#define LinuxTimer_HPP

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
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object LinuxTimer
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object LinuxTimer
      //!
      ~LinuxTimerComponentImpl(void);

      //! Start timer
      void startTimer(NATIVE_INT_TYPE interval); //!< interval in milliseconds

      //! Quit timer
      void quit(void);

      bool m_quit; //!< flag to quit

      Svc::TimerVal m_timer;


    };

} // end namespace Svc

#endif
