// ======================================================================
// \title  RefTopologyDefs.hpp
// \author mstarch
// \brief required header file containing the required definitions for the topology autocoder
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef REF_REFTOPOLOGYDEFS_HPP
#define REF_REFTOPOLOGYDEFS_HPP

#include "Ref/BlockDriver/BlockDriver.hpp"
#include "Ref/Top/FppConstantsAc.hpp"

// Subtopology PingEntries includes
#include "Svc/Subtopologies/CdhCore/PingEntries.hpp"
#include "Svc/Subtopologies/ComCcsds/PingEntries.hpp"
#include "Svc/Subtopologies/DataProducts/PingEntries.hpp"
#include "Svc/Subtopologies/FileHandling/PingEntries.hpp"

// SubtopologyTopologyDefs includes
#include "Svc/Subtopologies/CdhCore/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsds/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/DataProducts/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/FileHandling/SubtopologyTopologyDefs.hpp"

// ComCcsds Enum Includes
#include "Svc/Subtopologies/ComCcsds/Ports_ComBufferQueueEnumAc.hpp"
#include "Svc/Subtopologies/ComCcsds/Ports_ComPacketQueueEnumAc.hpp"

/**
 * \brief required ping constants
 *
 * The topology autocoder requires a WARN and FATAL constant definition for each component that supports the health-ping
 * interface. These are expressed as enum constants placed in a namespace named for the component instance. These
 * are all placed in the PingEntries namespace.
 *
 * Each constant specifies how many missed pings are allowed before a WARNING_HI/FATAL event is triggered. In the
 * following example, the health component will emit a WARNING_HI event if the component instance cmdDisp does not
 * respond for 3 pings and will FATAL if responses are not received after a total of 5 pings.
 *
 * ```c++
 * namespace PingEntries {
 * namespace cmdDisp {
 *     enum { WARN = 3, FATAL = 5 };
 * }
 * }
 * ```
 */
namespace PingEntries {
namespace Ref_blockDrv {
enum { WARN = 3, FATAL = 5 };
}
namespace Ref_pingRcvr {
enum { WARN = 3, FATAL = 5 };
}
namespace Ref_rateGroup1Comp {
enum { WARN = 3, FATAL = 5 };
}
namespace Ref_rateGroup2Comp {
enum { WARN = 3, FATAL = 5 };
}
namespace Ref_rateGroup3Comp {
enum { WARN = 3, FATAL = 5 };
}
namespace Ref_cmdSeq {
enum { WARN = 3, FATAL = 5 };
}
}  // namespace PingEntries

// Definitions are placed within a namespace named after the deployment
namespace Ref {

/**
 * \brief required type definition to carry state
 *
 * The topology autocoder requires an object that carries state with the name `Ref::TopologyState`. Only the type
 * definition is required by the autocoder and the contents of this object are otherwise opaque to the autocoder. The
 * contents are entirely up to the definition of the project. This reference application specifies hostname and port
 * fields, which are derived by command line inputs.
 */
struct TopologyState {
    const char* hostname;                         //!< Hostname for TCP communication
    U16 port;                                     //!< Port for TCP communication
    CdhCore::SubtopologyState cdhCore;            //!< Subtopology state for CdhCore
    ComCcsds::SubtopologyState comCcsds;          //!< Subtopology state for ComCcsds
    DataProducts::SubtopologyState dataProducts;  //!< Subtopology state for DataProducts
    FileHandling::SubtopologyState fileHandling;  //!< Subtopology state for FileHandling
};

namespace PingEntries = ::PingEntries;
}  // namespace Ref
#endif
