// ======================================================================
// \title Os/Generic/MultiFileSystem/FileSystem.cpp
// \brief MultiFileSystem implementation for Os::FileSystem
// ======================================================================
#include "Os/Generic/MultiFileSystem/FileSystem.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

MultiFileSystem::Status MultiFileSystem::_removeDirectory(const char* path) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    // status==SUCCESS should guarantee non-nullptr ?
    // Or could an implementation have null FileSystem but not File ? TODO: think about this
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_removeDirectory(path);
}

MultiFileSystem::Status MultiFileSystem::_removeFile(const char* path) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_removeFile(path);
}

MultiFileSystem::Status MultiFileSystem::_rename(const char* originPath, const char* destPath) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(originPath);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    // NOTE: assert that originPath and destPath route to the same filesystem ???
    // Wooowww?? Could we move stuff across partitions easily?? With Os::File and Os::Directory implemented?
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_rename(originPath, destPath);
}

MultiFileSystem::Status MultiFileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_getWorkingDirectory(path, bufferSize);
}

MultiFileSystem::Status MultiFileSystem::_changeWorkingDirectory(const char* path) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_changeWorkingDirectory(path);
}

MultiFileSystem::Status MultiFileSystem::_getFreeSpace(const char* path,
                                                       FwSizeType& totalBytes,
                                                       FwSizeType& freeBytes) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_getFreeSpace(path, totalBytes, freeBytes);
}

FileSystemHandle* MultiFileSystem::getHandle() {
    return &this->m_handle;
}

MultiFileSystem::Status MultiFileSystem::_getPathType(const char* path, PathType& pathType) {
    OsalImplSet* impl_set = OsalRegistry::routePathToImplementation(path);
    if (impl_set == nullptr) {
        return Status::OTHER_ERROR;
    }
    FW_ASSERT(impl_set->filesystem != nullptr);
    return impl_set->filesystem->_getPathType(path, pathType);
}

}  // namespace Generic
}  // namespace Os
