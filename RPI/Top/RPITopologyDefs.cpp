#include "RPI/Top/RPITopologyDefs.hpp"

namespace RPI {

  namespace Allocation {

    Fw::MallocAllocator mallocator;

  }

  Svc::LinuxTimer linuxTimer(FW_OPTIONAL_NAME("linuxTimer"));

}
