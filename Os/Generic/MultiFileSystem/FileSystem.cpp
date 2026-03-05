// ======================================================================
// \title Os/Generic/MultiFileSystem/FileSystem.cpp
// \brief MultiFileSystem implementation for Os::FileSystem
// ======================================================================
#include "Os/Generic/MultiFileSystem/FileSystem.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

MultiFileSystem::Status MultiFileSystem::_removeDirectory(const char* path) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    // status==SUCCESS should guarantee non-nullptr ?
    // Or could an implementation have null FileSystem but not File ? TODO: think about this
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_removeDirectory(path_after_prefix);
}

MultiFileSystem::Status MultiFileSystem::_removeFile(const char* path) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_removeFile(path_after_prefix);
}

MultiFileSystem::Status MultiFileSystem::_rename(const char* originPath, const char* destPath) {
    FwIndexType origin_prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(originPath, origin_prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* originPath_after_prefix = originPath + origin_prefix_len;
    FwIndexType dest_prefix_len = 0;
    OsalImplSet* dest_impl_set = OsalRegistry::routePathToImplementation(destPath, dest_prefix_len);
    if (dest_impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set == dest_impl_set);  // For now, require that source and destination route to the same filesystem
    // NOTE: assert that originPath and destPath route to the same filesystem ???
    // Wooowww?? Could we move stuff across partitions easily?? With Os::File and Os::Directory implemented?
    const char* destPath_after_prefix = destPath + dest_prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_rename(originPath_after_prefix, destPath_after_prefix);
}

MultiFileSystem::Status MultiFileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    char* path_after_prefix = path + prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    // TODO: make sure this works as expected, but since we're dealing with pointers it should be fine???
    return impl_set->filesystem->_getWorkingDirectory(path_after_prefix, bufferSize);
}

MultiFileSystem::Status MultiFileSystem::_changeWorkingDirectory(const char* path) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_changeWorkingDirectory(path_after_prefix);
}

MultiFileSystem::Status MultiFileSystem::_getFreeSpace(const char* path,
                                                       FwSizeType& totalBytes,
                                                       FwSizeType& freeBytes) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_getFreeSpace(path_after_prefix, totalBytes, freeBytes);
}

FileSystemHandle* MultiFileSystem::getHandle() {
    return &this->m_handle;
}

MultiFileSystem::Status MultiFileSystem::_getPathType(const char* path, PathType& pathType) {
    FwIndexType prefix_len = 0;
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_getPathType(path_after_prefix, pathType);
}

}  // namespace Generic
}  // namespace Os
