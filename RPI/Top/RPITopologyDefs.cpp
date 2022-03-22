#include "RPI/Top/RPITopologyDefs.hpp"

namespace RPI {

  namespace Allocation {

    Fw::MallocAllocator mallocator;

  }

  namespace Init {

    bool status = true;

  }

  Svc::LinuxTimer linuxTimer(FW_OPTIONAL_NAME("linuxTimer"));

}
