#include "Ref/Top/RefTopologyDefs.hpp"

namespace Ref {

  namespace Allocation {

    Fw::MallocAllocator mallocator;

  }

  Drv::BlockDriver blockDrv(FW_OPTIONAL_NAME("blockDrv"));

}
