// ======================================================================
// \title  Version.hpp
// \author sreddy
// \brief  hpp file for Version component implementation class
// ======================================================================

#ifndef Svc_Version_HPP
#define Svc_Version_HPP

#include "Svc/Version/VersionComponentAc.hpp"

namespace Svc {

  class Version :
    public VersionComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct Version object
      Version(
          const char* const compName //!< The component name
      );

      //! Destroy Version object
      ~Version();

  };

}

#endif
