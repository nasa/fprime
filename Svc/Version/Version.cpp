// ======================================================================
// \title  Version.cpp
// \author sreddy
// \brief  cpp file for Version component implementation class
// ======================================================================

#include "FpConfig.hpp"
#include "Svc/Version/Version.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  Version ::
    Version(const char* const compName) :
      VersionComponentBase(compName)
  {

  }

  Version ::
    ~Version()
  {

  }

}
