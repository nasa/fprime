// ======================================================================
// \title Os/DelegateFile.cpp
// \brief common function implementation for Os::FileInterface and Os::DelegateFile
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/DelegateFile.hpp>
#include <Os/File.hpp>
#include <algorithm>
#include <config/FppConstantsAc.hpp>

namespace Os {

DelegateFile::DelegateFile()
    : m_crc_buffer(), m_handle_storage(), m_delegate(*FileInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

DelegateFile::~DelegateFile() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    if (this->m_mode != OPEN_NO_MODE) {
        this->close();
    }
    m_delegate.~FileInterface();
}

DelegateFile::DelegateFile(const DelegateFile& other)
    : m_mode(other.m_mode),
      m_hash(other.m_hash),
      m_crc_buffer(),
      m_handle_storage(),
      m_delegate(*FileInterface::getDelegate(m_handle_storage, &other.m_delegate)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

DelegateFile& DelegateFile::operator=(const DelegateFile& other) {
    if (this != &other) {
        // The delegate below is constructed over the storage of the existing one. Any file this
        // object currently holds must be closed first or its handle is orphaned permanently.
        if (this->m_mode != OPEN_NO_MODE) {
            this->close();
        }
        this->m_delegate.~FileInterface();
        this->m_mode = other.m_mode;
        this->m_hash = other.m_hash;
        (void)FileInterface::getDelegate(m_handle_storage, &other.m_delegate);
        FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    }
    return *this;
}

DelegateFile::Status DelegateFile::open(const CHAR* filepath, DelegateFile::Mode requested_mode) {
    return this->open(filepath, requested_mode, OverwriteType::NO_OVERWRITE);
}

DelegateFile::Status DelegateFile::open(const CHAR* filepath,
                                        DelegateFile::Mode requested_mode,
                                        DelegateFile::OverwriteType overwrite) {
    FW_ASSERT(nullptr != filepath);
    return this->open(filepath, static_cast<FwSizeType>(FileNameStringSize + 1), requested_mode, overwrite);
}

DelegateFile::Status DelegateFile::open(const CHAR* filepath, FwSizeType length, DelegateFile::Mode requested_mode) {
    return this->open(filepath, length, requested_mode, OverwriteType::NO_OVERWRITE);
}

DelegateFile::Status DelegateFile::open(const CHAR* filepath,
                                        FwSizeType length,
                                        DelegateFile::Mode requested_mode,
                                        DelegateFile::OverwriteType overwrite) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(nullptr != filepath);
    const FwSizeType string_len = static_cast<FwSizeType>(Fw::StringUtils::string_length(filepath, length));
    FW_ASSERT(string_len < length, static_cast<FwAssertArgType>(string_len), static_cast<FwAssertArgType>(length));
    FW_ASSERT(DelegateFile::Mode::OPEN_NO_MODE < requested_mode && DelegateFile::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    FW_ASSERT((0 <= overwrite) && (overwrite < OverwriteType::MAX_OVERWRITE_TYPE));
    // Check for already opened file
    if (this->isOpen()) {
        return DelegateFile::Status::INVALID_MODE;
    }
    DelegateFile::Status status = this->m_delegate.open(filepath, requested_mode, overwrite);
    if (status == DelegateFile::Status::OP_OK) {
        this->m_mode = requested_mode;
        // Reset any open CRC calculations
        this->m_hash.init();
    }

    return status;
}

DelegateFile::Status DelegateFile::open(const Fw::ConstStringBase& path, DelegateFile::Mode requested_mode) {
    return this->open(path.toChar(), static_cast<FwSizeType>(path.getCapacity()), requested_mode,
                      OverwriteType::NO_OVERWRITE);
}

DelegateFile::Status DelegateFile::open(const Fw::ConstStringBase& path,
                                        DelegateFile::Mode requested_mode,
                                        DelegateFile::OverwriteType overwrite) {
    return this->open(path.toChar(), static_cast<FwSizeType>(path.getCapacity()), requested_mode, overwrite);
}

void DelegateFile::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    this->m_delegate.close();
    this->m_mode = Mode::OPEN_NO_MODE;
}

bool DelegateFile::isOpen() const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    return this->m_mode != Mode::OPEN_NO_MODE;
}

DelegateFile::Status DelegateFile::size(FwSizeType& size_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    if (OPEN_NO_MODE == this->m_mode) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.size(size_result);
}

DelegateFile::Status DelegateFile::position(FwSizeType& position_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.position(position_result);
}

DelegateFile::Status DelegateFile::preallocate(FwSizeType offset, FwSizeType length) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.preallocate(offset, length);
}

DelegateFile::Status DelegateFile::seek(FwSignedSizeType offset, DelegateFile::SeekType seekType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= seekType) && (seekType < SeekType::MAX_SEEK_TYPE));
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT((seekType == DelegateFile::SeekType::RELATIVE) || (offset >= 0));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return DelegateFile::Status::NOT_OPENED;
    }
    return this->m_delegate.seek(offset, seekType);
}

DelegateFile::Status DelegateFile::seek_absolute(FwSizeType offset) {
    DelegateFile::Status status = DelegateFile::Status::OTHER_ERROR;
    // If the offset can be represented by a signed value, then we can perform a single seek
    if (static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()) >= offset) {
        // Check that the bounding above is correct
        FW_ASSERT(static_cast<FwSignedSizeType>(offset) >= 0);
        status = this->seek(static_cast<FwSignedSizeType>(offset), DelegateFile::SeekType::ABSOLUTE);
    }
    // Otherwise, a full seek to any value represented by FwSizeType can be performed
    // by at most 3 seeks of a FwSignedSizeType. Two half seeks (rounded down) that are
    // strictly bounded by std::numeric_limits<FwSignedSizeType>::max() and one seek of
    // a possibile "odd" byte to ensure odds offsets do not introduce an off-by-one-error.
    // Thus we perform 3 seeks to guarantee that we can reach any position.
    else {
        FwSignedSizeType half_offset = static_cast<FwSignedSizeType>(offset >> 1);
        bool is_odd = (offset % 2) == 1;
        status = this->seek(half_offset, DelegateFile::SeekType::ABSOLUTE);
        if (status == DelegateFile::Status::OP_OK) {
            status = this->seek(half_offset, DelegateFile::SeekType::RELATIVE);
        }
        if (status == DelegateFile::Status::OP_OK) {
            status = this->seek((is_odd) ? 1 : 0, DelegateFile::SeekType::RELATIVE);
        }
    }
    return status;
}

DelegateFile::Status DelegateFile::flush() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.flush();
}

DelegateFile::Status DelegateFile::read(U8* buffer, FwSizeType& size) {
    return this->read(buffer, size, WaitType::WAIT);
}

DelegateFile::Status DelegateFile::read(U8* buffer, FwSizeType& size, DelegateFile::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.read(buffer, size, wait);
}

DelegateFile::Status DelegateFile::write(const U8* buffer, FwSizeType& size) {
    return this->write(buffer, size, WaitType::WAIT);
}

DelegateFile::Status DelegateFile::write(const U8* buffer, FwSizeType& size, DelegateFile::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        size = 0;
        return DelegateFile::Status::INVALID_MODE;
    }
    return this->m_delegate.write(buffer, size, wait);
}

FileHandle* DelegateFile::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

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
    if (OPEN_NO_MODE == this->m_mode) {
        status = DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
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
    DelegateFile::Status status = DelegateFile::Status::OP_OK;
    this->m_hash.finalize(crc);
    // Historically, the CRC calculation in File omitted the final 1's complement step. Utils::Hash performs that step
    // and as such, we must undo it before returning the value to ensure backwards compatibility.
    crc = ~crc;
    this->m_hash.init();
    return status;
}

DelegateFile::Status DelegateFile::readline(U8* buffer, FwSizeType& size, DelegateFile::WaitType wait) {
    const FwSizeType requested_size = size;
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return DelegateFile::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return DelegateFile::Status::INVALID_MODE;
    }
    FwSizeType original_location = 0;
    DelegateFile::Status status = this->position(original_location);
    if (status != DelegateFile::Status::OP_OK) {
        size = 0;
        (void)this->seek_absolute(original_location);
        return status;
    }
    FwSizeType read = 0;
    // Loop reading chunk by chunk
    for (FwSizeType i = 0; i < size; i += read) {
        // read in chunks to avoid large buffer allocations
        FwSizeType current_chunk_size = std::min(size - i, static_cast<FwSizeType>(FW_FILE_CHUNK_SIZE));
        read = current_chunk_size;
        status = this->read(buffer + i, read, wait);
        if (status != DelegateFile::Status::OP_OK) {
            // Contract: on error, seek back to the original location
            size = 0;
            (void)this->seek_absolute(original_location);
            return status;
        }
        // EOF break out now
        if (read == 0) {
            size = i;
            return DelegateFile::Status::OP_OK;
        }
        // Loop from i to i + current_chunk_size looking for `\n`
        const FwSizeType chunk_end = i + read;
        for (FwSizeType j = i; j < chunk_end; j++) {
            // Newline seek back to after it, return the size read
            if (buffer[j] == '\n') {
                size = j + 1;
                // Ensure that the computation worked and there is not overflow
                FW_ASSERT(size <= requested_size);
                FW_ASSERT(std::numeric_limits<FwSizeType>::max() - size >= original_location);
                (void)this->seek_absolute(original_location + j + 1);
                return DelegateFile::Status::OP_OK;
            }
        }
    }
    // Failed to find newline within data available
    // Contract: on error, seek back to the original location
    size = 0;
    (void)this->seek_absolute(original_location);
    return DelegateFile::Status::OTHER_ERROR;
}
}  // namespace Os
