// ======================================================================
// \title Os/Directory.cpp
// \brief common function implementation for Os::Directory
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Directory.hpp>

namespace Os {

Directory::Directory() : m_handle_storage(), m_delegate(*DirectoryInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
}

Directory::~Directory() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    m_delegate.~DirectoryInterface();
}

DirectoryHandle* Directory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

Directory::Status Directory::open(const char* path, OpenMode mode) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.open(path, mode);
}

bool Directory::isOpen() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.isOpen();
}
Directory::Status Directory::rewind() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (!this->m_delegate.isOpen()) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.rewind();
}

Directory::Status Directory::read(char * fileNameBuffer, FwSizeType bufSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (!this->m_delegate.isOpen()) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.read(fileNameBuffer, bufSize);
}

void Directory::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.close();
}

// ------------ Common Directory Functions (non-OS-specific) ------------

Directory::Status Directory::getFileCount(FwSizeType& fileCount) {
    if (this->isOpen() == false) {
        return Status::NOT_OPENED;
    }
    // REVIEW NOTE: should getFileCount rewind before counting?
    // Likely yes?? Otherwise it's just weird
    // TODO: check return value (how to handle if not OP_OK?)
    this->rewind();

    const U32 loopLimit = std::numeric_limits<U32>::max();
    FwSizeType count = 0;
    const FwSizeType unusedBufferSize = 1;
    char unusedBuffer[unusedBufferSize];
    Status readStatus = Status::OP_OK;
    for (U32 iter = 0; iter < loopLimit; ++iter) {
        readStatus = this->read(unusedBuffer, unusedBufferSize);
        if (readStatus == Status::NO_MORE_FILES) {
            break;
        } else if (readStatus != Status::OP_OK) {
            return Status::OTHER_ERROR;
        }
        ++count;
    }
    fileCount = count;
    // and after??
    this->rewind();
    return Status::OP_OK;
}


Directory::Status Directory::readDirectory(Fw::String filenameArray[], const FwSizeType filenameArraySize, FwSizeType& filenameCount) {
    FW_ASSERT(filenameArray != nullptr);
    FW_ASSERT(filenameArraySize > 0);
    if (this->isOpen() == false) {
        return Status::NOT_OPENED;
    }
    // same thing here - should we rewind before?
    this->rewind();

    Status readStatus = Status::OP_OK;
    Status returnStatus = Status::OP_OK;

    FwIndexType index;
    constexpr FwIndexType loopLimit = std::numeric_limits<FwIndexType>::max();

    char fileName[FPP_CONFIG_FILENAME_MAX_SIZE]; // should be FW_FIXED_LENGTH_STRING_SIZE instead??

    for (index = 0; ((index < loopLimit) && (index < static_cast<FwIndexType>(filenameArraySize))); index++) {
        readStatus = this->read(fileName, FPP_CONFIG_FILENAME_MAX_SIZE);
        if (readStatus == Status::NO_MORE_FILES) {
            break;
        } else if (readStatus != Status::OP_OK) {
            return Status::OTHER_ERROR;
        }
        
        filenameArray[index] = Fw::String(fileName);
    }

    filenameCount = index;

    if (index == loopLimit) {
        returnStatus = Status::FILE_LIMIT;
    }
    this->rewind();

    return returnStatus;

}


}  // namespace Os
