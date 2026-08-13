#ifndef DEFAULT_CONFIG_FILEMANAGERCONFIG_HPP
#define DEFAULT_CONFIG_FILEMANAGERCONFIG_HPP

#include <config/FpConfig.hpp>

namespace Svc {
namespace FileManagerConfig {
//! Number of directory entries to process per rate group tick
//! Higher values = faster directory listing but more events per tick
//! Lower values = slower directory listing but bounded event rate
//! Default: 1
static constexpr U32 FILES_PER_RATE_TICK = 1;

//! Maximum number of file bytes read per chunk when generating a data product.
//! The GenerateDp chunkSize argument is clamped to this value, which bounds
//! the size of the member read buffer (no dynamic allocation in flight code).
static constexpr U32 GENERATE_DP_MAX_CHUNK_SIZE = 1024;

//! Number of file chunks to process per rate group tick during
//! data product generation (same pacing pattern as directory listing).
static constexpr U32 CHUNKS_PER_RATE_TICK = 1;
}  // namespace FileManagerConfig
}  // namespace Svc

#endif
