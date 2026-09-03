// ======================================================================
// \title Os/DelegateFile.cpp
// \brief link-time delegate implementation for Os::DelegateFile
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateFile.hpp>
#include <config/FppConstantsAc.hpp>
#include <limits>

namespace Os {

DelegateFile::DelegateFile()
    : FileInterface(), m_handle_storage(), m_delegate(*FileInterface::getDelegate(m_handle_storage)), m_crc_buffer() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

DelegateFile::~DelegateFile() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    if (this->isOpen()) {
        this->close();
    }
    m_delegate.~FileInterface();
}

DelegateFile::DelegateFile(const DelegateFile& other)
    : FileInterface(other),
      m_handle_storage(),
      m_delegate(*FileInterface::getDelegate(m_handle_storage, &other.m_delegate)),
      m_hash(other.m_hash),
      m_crc_buffer() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

DelegateFile& DelegateFile::operator=(const DelegateFile& other) {
    if (this != &other) {
        // The delegate below is constructed over the storage of the existing one. Any file this
        // object currently holds must be closed first or its handle is orphaned permanently.
        if (this->isOpen()) {
            this->close();
        }
        this->m_delegate.~FileInterface();
        FileInterface::operator=(other);
        this->m_hash = other.m_hash;
        (void)FileInterface::getDelegate(m_handle_storage, &other.m_delegate);
        FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    }
    return *this;
}

DelegateFile::Status DelegateFile::open(const char* filepath,
                                        DelegateFile::Mode requested_mode,
                                        DelegateFile::OverwriteType overwrite) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(nullptr != filepath);
    FW_ASSERT(DelegateFile::Mode::OPEN_NO_MODE < requested_mode && DelegateFile::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT((0 <= overwrite) && (overwrite < OverwriteType::MAX_OVERWRITE_TYPE));
    // Check for already opened file
    if (this->isOpen()) {
        return DelegateFile::Status::INVALID_MODE;
    }
    DelegateFile::Status status = this->m_delegate.open(filepath, requested_mode, overwrite);
    if (status == DelegateFile::Status::OP_OK) {
        this->setMode(requested_mode);
        // Reset any open CRC calculation to match the freshly-opened file
        this->m_hash.init();
    }
    return status;
}

void DelegateFile::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.close();
    this->setMode(Mode::OPEN_NO_MODE);
}

DelegateFile::Status DelegateFile::size(FwSizeType& size_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    if (not this->isOpen()) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.size(size_result);
}

DelegateFile::Status DelegateFile::position(FwSizeType& position_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.position(position_result);
}

DelegateFile::Status DelegateFile::preallocate(FwSizeType offset, FwSizeType length) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        return DelegateFile::Status::NOT_OPENED;
    } else if (this->getMode() == FileInterface::Mode::OPEN_READ) {
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.preallocate(offset, length);
}

DelegateFile::Status DelegateFile::seek(FwSignedSizeType offset, DelegateFile::SeekType seekType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= seekType) && (seekType < SeekType::MAX_SEEK_TYPE));
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT((seekType == DelegateFile::SeekType::RELATIVE) || (offset >= 0));
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.seek(offset, seekType);
}

DelegateFile::Status DelegateFile::flush() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        return DelegateFile::Status::NOT_OPENED;
    } else if (this->getMode() == FileInterface::Mode::OPEN_READ) {
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.flush();
}

DelegateFile::Status DelegateFile::read(U8* buffer, FwSizeType& size, DelegateFile::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        size = 0;
        return DelegateFile::Status::NOT_OPENED;
    } else if (this->getMode() != FileInterface::Mode::OPEN_READ) {
        size = 0;
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.read(buffer, size, wait);
}

DelegateFile::Status DelegateFile::write(const U8* buffer, FwSizeType& size, DelegateFile::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    // Check that the file is open before attempting operation
    if (not this->isOpen()) {
        size = 0;
        return DelegateFile::Status::NOT_OPENED;
    } else if (this->getMode() == FileInterface::Mode::OPEN_READ) {
        size = 0;
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.write(buffer, size, wait);
}

FileHandle* DelegateFile::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

// ----------------------------------------------------------------------
// CRC implementation
//
// Relocated from FileInterface so the scratch buffer is charged to this
// wrapper's (unconstrained) storage rather than the fixed-size delegate
// handle. The algorithm is unchanged; it drives I/O through the virtual
// `this->read()`, which dispatches to the selected delegate.
// ----------------------------------------------------------------------

DelegateFile::Status DelegateFile::calculateCrc(U32& crc) {
    DelegateFile::Status status = DelegateFile::Status::OP_OK;
    FwSizeType size = FW_FILE_CHUNK_SIZE;
    crc = 0;
    for (FwSizeType i = 0; i < std::numeric_limits<FwSizeType>::max(); i++) {
        status = this->incrementalCrc(size);
        // Break on eof or error
        if ((size != FW_FILE_CHUNK_SIZE) || (status != DelegateFile::OP_OK)) {
            break;
        }
    }
    // When successful, finalize the CRC
    if (status == DelegateFile::OP_OK) {
        status = this->finalizeCrc(crc);
    }
    return status;
}

DelegateFile::Status DelegateFile::incrementalCrc(FwSizeType& size) {
    DelegateFile::Status status = DelegateFile::Status::OP_OK;
    FW_ASSERT(size <= FW_FILE_CHUNK_SIZE, FwAssertArgType(size));
    if (not this->isOpen()) {
        status = DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ != this->getMode()) {
        status = DelegateFile::Status::INVALID_MODE;
    } else {
        // Read data without waiting for additional data to be available
        status = this->read(this->m_crc_buffer, size, DelegateFile::WaitType::NO_WAIT);
        if (OP_OK == status) {
            FW_ASSERT(size <= FW_FILE_CHUNK_SIZE, FwAssertArgType(size));
            this->m_hash.update(this->m_crc_buffer, size);
        }
    }
    return status;
}

DelegateFile::Status DelegateFile::finalizeCrc(U32& crc) {
    this->m_hash.finalize(crc);
    // Historically, the CRC calculation in File omitted the final 1's complement step. Utils::Hash performs that step
    // and as such, we must undo it before returning the value to ensure backwards compatibility.
    crc = ~crc;
    this->m_hash.init();
    return DelegateFile::Status::OP_OK;
}

}  // namespace Os
