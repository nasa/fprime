#include "RPI/Top/RPITopologyDefs.hpp"

namespace RPI {

  namespace Allocation {

    Fw::MallocAllocator mallocator;

  }

  namespace ComQueueCfg {

    Svc::ComQueue::QueueConfigurationTable configurationTable;

  }

  namespace Init {

    bool status = true;

  }

}
