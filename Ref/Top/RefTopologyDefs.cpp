#include "Ref/Top/RefTopologyDefs.hpp"

namespace Ref {

  namespace Allocation {

    Fw::MallocAllocator mallocator;

  }

  // Declare block driver here so it is visible in Main
  Drv::BlockDriverImpl blockDrv(FW_OPTIONAL_NAME("blockDrv"));

}
