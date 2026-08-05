#ifndef COMCCSDSSDLSSUBTOPOLOGY_DEFS_HPP
#define COMCCSDSSDLSSUBTOPOLOGY_DEFS_HPP

#include "Svc/Subtopologies/ComCcsds/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsdsSdls/ComCcsdsSdlsConfig/FppConstantsAc.hpp"

namespace ComCcsdsSdls {
struct SubtopologyState {
    // Empty - no external state needed for ComCcsdsSdls subtopology
};

struct TopologyState {
    SubtopologyState comCcsdsSdls;
};
}  // namespace ComCcsdsSdls

#endif
