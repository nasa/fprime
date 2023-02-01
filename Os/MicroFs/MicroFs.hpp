#ifndef _MICROFS_HPP_
#define _MICROFS_HPP_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/MemAllocator.hpp>
#include <MicroFsCfg.hpp>

namespace Os {

struct MicroFsBin {
    PlatformUIntType fileSize;  //<! The size of the files in the bin
    PlatformUIntType numFiles;  //<! The number of files in the bin
};

struct MicroFsConfig {
    PlatformUIntType numBins;           //!< The number of bins configured. Must be <= than MAX_MICROFS_BINS
    MicroFsBin bins[MAX_MICROFS_BINS];  //!< The bins containing file sizes and numbers of files
};

//!< initialize MicroFs memory by passing the configuration, a memory id (if needed), and a memory allocator

void MicroFsInit(const MicroFsConfig& cfg,      //!< the configuration of the memory space
                 const PlatformUIntType id,     //!< The memory id. Value doesn't matter if allocator doesn't need it
                 Fw::MemAllocator& allocator);  //!< Memory allocator to use for memory

void MicroFsCleanup(const PlatformUIntType id,     //!< The memory id. Value doesn't matter if allocator doesn't need it
                 Fw::MemAllocator& allocator);  //!< Memory allocator to to deallocate. Should match MicroFsInit allocator                 

}  // namespace Os

#endif
