// ======================================================================
// \title  ChronoTime.hpp
// \author mstarch
// \brief  hpp file for ChronoTime component implementation class
// ======================================================================

#ifndef Svc_ChronoTime_HPP
#define Svc_ChronoTime_HPP

#include "Svc/ChronoTime/ChronoTimeComponentAc.hpp"

namespace Svc {

class ChronoTime : public ChronoTimeComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ChronoTime object
    ChronoTime(const char* const compName  //!< The component name
    );

    //! Destroy ChronoTime object
    ~ChronoTime();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for timeGetPort
    //!
    //! Port to retrieve time
    void timeGetPort_handler(FwIndexType portNum,  //!< The port number
                             Fw::Time& time        //!< Reference to Time object
                             ) override;
};

}  // namespace Svc

#endif
