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
}  // namespace FileManagerConfig
}  // namespace Svc

#endif
