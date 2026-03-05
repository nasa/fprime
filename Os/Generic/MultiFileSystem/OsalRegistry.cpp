// ======================================================================
// \title Os/Generic/MultiFileSystem/OsalRegistry.cpp
// \brief OsalRegistry implementation for managing OSAL implementations
// ======================================================================

#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

// Initialize static registry array
Fw::Array<OsalImplMapping*, OsalRegistry::MAX_FILESYSTEMS> OsalRegistry::s_implMappings = {};

RegistryStatus OsalRegistry::registerImplementation(OsalImplMapping* implementation_map) {
    // Find the first available slot
    for (FwSizeType i = 0; i < MAX_FILESYSTEMS; i++) {
        if (OsalRegistry::s_implMappings[i] == nullptr) {
            OsalRegistry::s_implMappings[i] = implementation_map;
            return RegistryStatus::SUCCESS;
        }
    }

    // No available slots
    return RegistryStatus::OTHER_ERROR;
}

OsalImplSet* OsalRegistry::routePathToImplementation(const char* path) {
    if (path == nullptr) {
        return nullptr;
    }

    // Iterate through registered implementations and find the first matching path prefix
    for (FwSizeType i = 0; i < MAX_FILESYSTEMS; i++) {
        if (OsalRegistry::s_implMappings[i] == nullptr) {
            break;  // No more registered implementations to check
        }

        OsalImplSet* impl = OsalRegistry::s_implMappings[i]->impl_set;
        if (impl->filesystem != nullptr) {
            // Check if path starts with the implementation's path prefix
            // Manual character-by-character comparison to verify prefix match
            const char* prefix = OsalRegistry::s_implMappings[i]->path_prefix;
            FwSizeType j = 0;

            while (j < MAX_MULTIFS_PATH_PREFIX_LENGTH && prefix[j] != '\0') {
                if (path[j] != prefix[j]) {
                    break;
                }
                j++;
            }

            // If we reached the end of prefix (null terminator), it's a match
            if (j < MAX_MULTIFS_PATH_PREFIX_LENGTH && prefix[j] == '\0') {
                return impl;
            }
        }
    }

    // No matching implementation found
    return nullptr;
}

}  // namespace Generic
}  // namespace Os
