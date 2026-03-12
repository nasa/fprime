// ======================================================================
// \title Os/Generic/MultiFileSystem/FileSystemRegistry.hpp
// \brief OsalRegistry for managing multiple OSAL implementations
// ======================================================================
#ifndef OS_GENERIC_MULTIFILESYSTEM_OSALIMPLREGISTRY_HPP
#define OS_GENERIC_MULTIFILESYSTEM_OSALIMPLREGISTRY_HPP

#include "Fw/DataStructures/Array.hpp"
#include "Fw/DataStructures/ArrayMap.hpp"
#include "Os/Directory.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"

namespace Os {
namespace Generic {

enum class RegistryStatus { SUCCESS, INVALID_PATH, OTHER_ERROR };

static constexpr FwSizeType MAX_MULTIFS_PATH_PREFIX_LENGTH = 10;

//! \brief Backing OSAL implementation set
//!
//! Contains all three interface implementations for a single OSAL implementation,
//! ensuring they are registered and maintained together.
//!
struct OsalImplSet {
    FileSystemInterface* filesystem = nullptr;  //!< FileSystem interface
    FileInterface* file = nullptr;              //!< File interface
    DirectoryInterface* directory = nullptr;    //!< Directory interface
};

struct OsalImplMapping {
    // + 1 for null terminator ??
    const char path_prefix[MAX_MULTIFS_PATH_PREFIX_LENGTH] = {};  //!< Path prefix to match for routing
    OsalImplSet* impl_set = nullptr;                              //!< Backing implementation set to route to
};

//! \brief Registry for managing multiple OSAL backing implementation sets
//!
//! OsalRegistry provides centralized management of multiple backing
//! implementation sets (FileSystem, File, and Directory), allowing MultiFileSystem,
//! MultiFile, and MultiDirectory to route operations to the appropriate underlying
//! implementations based on path.
//!
class OsalRegistry {
  private:
    static constexpr FwSizeType MAX_FILESYSTEMS = 4;

  public:
    //! \brief Register a complete backing implementation set
    //!
    //! Registers a backing implementation set containing FileSystem, File, and
    //! Directory interfaces. All three must be provided. Implementations are
    //! registered in order and the first non-nullptr registration is considered
    //! the default filesystem.
    //!
    //! \param implementation The backing implementation set to register
    //! \param path_prefix The path prefix to use for routing to this implementation
    //! \return RegistryStatus::SUCCESS if successfully registered, RegistryStatus::INVALID_PATH if incomplete,
    //!         RegistryStatus::OTHER_ERROR if registry is full
    static RegistryStatus registerImplementation(OsalImplMapping* implementation_map);

    //! \brief Route a path to the appropriate backing implementation set
    //!
    //! Routes a path to the appropriate backing implementation set based on the path prefix.
    //! Special prefixes like "/Z" route to specific backing implementation sets, otherwise the
    //! default backing implementation set (first registered) is used.
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path to route
    //! \return Routed backing implementation set, or `nullptr` if no route matches or path is invalid
    static OsalImplSet* routePathToImplementation(const char* path);

  private:
    //! Array of registered backing implementation sets
    static Fw::Array<OsalImplMapping*, MAX_FILESYSTEMS> s_implMappings;
    // static Fw::ArrayMap<const char*, OsalImplSet, MAX_FILESYSTEMS> s_implMap;

};  // class OsalRegistry

}  // namespace Generic
}  // namespace Os
#endif  // OS_GENERIC_MULTIFILESYSTEM_OSALIMPLREGISTRY_HPP
