// ======================================================================
// \title Os/DelegateDirectory.cpp
// \brief common function implementation for Os::DelegateDirectory
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateDirectory.hpp>
#include <Os/Directory.hpp>
#include <limits>

namespace Os {

DelegateDirectory::DelegateDirectory()
    : m_is_open(false), m_handle_storage(), m_delegate(*DirectoryInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
}

DelegateDirectory::~DelegateDirectory() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (this->m_is_open) {
        this->close();
    }
    this->m_delegate.~DirectoryInterface();
}

// ------------------------------------------------------------
// Directory operations delegating to implementation
// ------------------------------------------------------------
DirectoryHandle* DelegateDirectory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateDirectory::Status DelegateDirectory::open(const char* path, OpenMode mode) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    FW_ASSERT(mode >= 0 and mode < OpenMode::MAX_OPEN_MODE);
    Status status = this->m_delegate.open(path, mode);
    if (status == Status::OP_OK) {
        this->m_is_open = true;
    }
    return status;
}

bool DelegateDirectory::isOpen() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_is_open;
}

DelegateDirectory::Status DelegateDirectory::rewind() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (not this->m_is_open) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.rewind();
}

DelegateDirectory::Status DelegateDirectory::read(char* fileNameBuffer, FwSizeType bufSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (not this->m_is_open) {
        return Status::NOT_OPENED;
    }
    FW_ASSERT(fileNameBuffer != nullptr);
    FW_ASSERT(bufSize > 0);
    Status status = this->m_delegate.read(fileNameBuffer, bufSize);
    fileNameBuffer[bufSize - 1] = '\0';  // Guarantee null-termination
    return status;
}

DelegateDirectory::Status DelegateDirectory::read(Fw::StringBase& filename) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (not this->m_is_open) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.read(filename);
}

void DelegateDirectory::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    this->m_is_open = false;
    return this->m_delegate.close();
}

// ------------------------------------------------------------
// Common functions built on top of OS-specific functions
// ------------------------------------------------------------

DelegateDirectory::Status DelegateDirectory::getFileCount(FwSizeType& fileCount) {
    if (not this->m_is_open) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.getFileCount(fileCount);
}

DelegateDirectory::Status DelegateDirectory::readDirectory(Fw::ExternalArray<Fw::String>& filenameArray,
                                                           FwSizeType& filenameCount) {
    FW_ASSERT(filenameArray.getElements() != nullptr);
    FW_ASSERT(filenameArray.getSize() > 0);
    if (not this->m_is_open) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.readDirectory(filenameArray, filenameCount);
}

DelegateDirectory::Status DelegateDirectory::readDirectory(Fw::String filenameArray[],
                                                           const FwSizeType filenameArraySize,
                                                           FwSizeType& filenameCount) {
    Fw::ExternalArray<Fw::String> array(filenameArray, filenameArraySize);
    return this->readDirectory(array, filenameCount);
}

// ------------------------------------------------------------
// DirectoryInterface helper implementations
// ------------------------------------------------------------

DirectoryInterface::Status DirectoryInterface::read(Fw::StringBase& filename) {
    return this->read(const_cast<char*>(filename.toChar()), filename.getCapacity());
}

DirectoryInterface::Status DirectoryInterface::readDirectory(Fw::ExternalArray<Fw::String>& filenameArray,
                                                             FwSizeType& filenameCount) {
    // Rewind to ensure we start reading from the beginning of the stream
    if (this->rewind() != Status::OP_OK) {
        return Status::OTHER_ERROR;
    }

    Status readStatus = Status::OP_OK;
    Status returnStatus = Status::OP_OK;
    FwSizeType index;
    filenameCount = 0;
    // Iterate through the directory and read the filenames into the array
    for (index = 0; index < filenameArray.getSize(); index++) {
        readStatus = this->read(filenameArray[index]);
        if (readStatus == Status::NO_MORE_FILES) {
            break;
        } else if (readStatus != Status::OP_OK) {
            return Status::OTHER_ERROR;
        }
    }
    filenameCount = index;

    if (this->rewind() != Status::OP_OK) {
        return Status::OTHER_ERROR;
    }

    return returnStatus;
}

DirectoryInterface::Status DirectoryInterface::getFileCount(FwSizeType& fileCount) {
    // Rewind to ensure we start from the beginning of the stream
    if (this->rewind() != Status::OP_OK) {
        return Status::OTHER_ERROR;
    }
    const FwSizeType loopLimit = std::numeric_limits<FwSizeType>::max();
    FwSizeType count = 0;
    char unusedBuffer[1];  // buffer must have size but is unused
    Status readStatus = Status::OP_OK;
    fileCount = 0;
    // Count files by reading each file entry until there is NO_MORE_FILES
    for (FwSizeType iter = 0; iter < loopLimit; ++iter) {
        readStatus = this->read(unusedBuffer, sizeof(unusedBuffer));
        if (readStatus == Status::NO_MORE_FILES) {
            break;
        } else if (readStatus != Status::OP_OK) {
            return Status::OTHER_ERROR;
        }
        count++;
    }
    fileCount = count;
    if (this->rewind() != Status::OP_OK) {
        return Status::OTHER_ERROR;
    }
    return Status::OP_OK;
}

}  // namespace Os
