// ======================================================================
// \title Os/File.cpp
// \brief common function implementation for Os::File
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>

extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h>  // borrow CRC
}
namespace Os {

File::File() : m_crc_buffer(), m_handle_storage(), m_delegate(*FileInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

File::~File() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    if (this->m_mode != OPEN_NO_MODE) {
        this->close();
    }
    m_delegate.~FileInterface();
}

File::File(const File& other)
    : m_mode(other.m_mode),
      m_path(other.m_path),
      m_crc(other.m_crc),
      m_crc_buffer(),
      m_handle_storage(),
      m_delegate(*FileInterface::getDelegate(m_handle_storage, &other.m_delegate)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
}

File& File::operator=(const File& other) {
    if (this != &other) {
        this->m_mode = other.m_mode;
        this->m_path = other.m_path;
        this->m_crc = other.m_crc;
        this->m_delegate = *FileInterface::getDelegate(m_handle_storage, &other.m_delegate);
    }
    return *this;
}

File::Status File::open(const CHAR* filepath, File::Mode requested_mode) {
    return this->open(filepath, requested_mode, OverwriteType::NO_OVERWRITE);
}

File::Status File::open(const CHAR* filepath, File::Mode requested_mode, File::OverwriteType overwrite) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(nullptr != filepath);
    FW_ASSERT(File::Mode::OPEN_NO_MODE < requested_mode && File::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    FW_ASSERT((0 <= overwrite) && (overwrite < OverwriteType::MAX_OVERWRITE_TYPE));
    // Check for already opened file
    if (this->isOpen()) {
        return File::Status::INVALID_MODE;
    }
    File::Status status = this->m_delegate.open(filepath, requested_mode, overwrite);
    if (status == File::Status::OP_OK) {
        this->m_mode = requested_mode;
        this->m_path = filepath;
        // Reset any open CRC calculations
        this->m_crc = File::INITIAL_CRC;
    }

    return status;
}

void File::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    this->m_delegate.close();
    this->m_mode = Mode::OPEN_NO_MODE;
    this->m_path = nullptr;
}

bool File::isOpen() const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    return this->m_mode != Mode::OPEN_NO_MODE;
}

File::Status File::size(FwSizeType& size_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->m_delegate.size(size_result);
}

File::Status File::position(FwSizeType& position_result) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->m_delegate.position(position_result);
}

File::Status File::preallocate(FwSizeType offset, FwSizeType length) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return File::Status::INVALID_MODE;
    }
    return this->m_delegate.preallocate(offset, length);
}

File::Status File::seek(FwSignedSizeType offset, File::SeekType seekType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT((0 <= seekType) && (seekType < SeekType::MAX_SEEK_TYPE));
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT((seekType == File::SeekType::RELATIVE) || (offset >= 0));
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->m_delegate.seek(offset, seekType);
}

File::Status File::seek_absolute(FwSizeType offset) {
    Os::File::Status status = File::Status::OTHER_ERROR;
    // If the offset can be represented by a signed value, then we can perform a single seek
    if (static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()) >= offset) {
        // Check that the bounding above is correct
        FW_ASSERT(static_cast<FwSignedSizeType>(offset) >= 0);
        status = this->seek(static_cast<FwSignedSizeType>(offset), File::SeekType::ABSOLUTE);
    }
    // Otherwise, a full seek to any value represented by FwSizeType can be performed
    // by at most 3 seeks of a FwSignedSizeType. Two half seeks (rounded down) that are
    // strictly bounded by std::numeric_limits<FwSignedSizeType>::max() and one seek of
    // a possibile "odd" byte to ensure odds offsets do not introduce an off-by-one-error.
    // Thus we perform 3 seeks to guarantee that we can reach any position.
    else {
        FwSignedSizeType half_offset = static_cast<FwSignedSizeType>(offset >> 1);
        bool is_odd = (offset % 2) == 1;
        status = this->seek(half_offset, File::SeekType::ABSOLUTE);
        if (status == File::Status::OP_OK) {
            status = this->seek(half_offset, File::SeekType::RELATIVE);
        }
        if (status == File::Status::OP_OK) {
            status = this->seek((is_odd) ? 1 : 0, File::SeekType::RELATIVE);
        }
    }
    return status;
}

File::Status File::flush() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return File::Status::INVALID_MODE;
    }
    return this->m_delegate.flush();
}

File::Status File::read(U8* buffer, FwSizeType& size) {
    return this->read(buffer, size, WaitType::WAIT);
}

File::Status File::read(U8* buffer, FwSizeType& size, File::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->m_delegate.read(buffer, size, wait);
}

File::Status File::write(const U8* buffer, FwSizeType& size) {
    return this->write(buffer, size, WaitType::WAIT);
}

File::Status File::write(const U8* buffer, FwSizeType& size, File::WaitType wait) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->m_delegate.write(buffer, size, wait);
}

FileHandle* File::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

File::Status File::calculateCrc(U32& crc) {
    File::Status status = File::Status::OP_OK;
    FwSizeType size = FW_FILE_CHUNK_SIZE;
    crc = 0;
    for (FwSizeType i = 0; i < std::numeric_limits<FwSizeType>::max(); i++) {
        status = this->incrementalCrc(size);
        // Break on eof or error
        if ((size != FW_FILE_CHUNK_SIZE) || (status != File::OP_OK)) {
            break;
        }
    }
    // When successful, finalize the CRC
    if (status == File::OP_OK) {
        status = this->finalizeCrc(crc);
    }
    return status;
}

File::Status File::incrementalCrc(FwSizeType& size) {
    File::Status status = File::Status::OP_OK;
    FW_ASSERT(size <= FW_FILE_CHUNK_SIZE);
    if (OPEN_NO_MODE == this->m_mode) {
        status = File::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        status = File::Status::INVALID_MODE;
    } else {
        // Read data without waiting for additional data to be available
        status = this->read(this->m_crc_buffer, size, File::WaitType::NO_WAIT);
        if (OP_OK == status) {
            for (FwSizeType i = 0; i < size && i < FW_FILE_CHUNK_SIZE; i++) {
                this->m_crc = static_cast<U32>(update_crc_32(this->m_crc, static_cast<CHAR>(this->m_crc_buffer[i])));
            }
        }
    }
    return status;
}

File::Status File::finalizeCrc(U32& crc) {
    File::Status status = File::Status::OP_OK;
    crc = this->m_crc;
    this->m_crc = File::INITIAL_CRC;
    return status;
}

File::Status File::readline(U8* buffer, FwSizeType& size, File::WaitType wait) {
    const FwSizeType requested_size = size;
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    FwSizeType original_location;
    File::Status status = this->position(original_location);
    if (status != Os::File::Status::OP_OK) {
        size = 0;
        (void)this->seek_absolute(original_location);
        return status;
    }
    FwSizeType read = 0;
    // Loop reading chunk by chunk
    for (FwSizeType i = 0; i < size; i += read) {
        FwSizeType current_chunk_size = FW_MIN(size - i, FW_FILE_CHUNK_SIZE);
        read = current_chunk_size;
        status = this->read(buffer + i, read, wait);
        if (status != File::Status::OP_OK) {
            return status;
        }
        // EOF break out now
        if (read == 0) {
            size = i;
            return Os::File::Status::OP_OK;
        }
        // Loop from i to i + current_chunk_size looking for `\n`
        for (FwSizeType j = i; j < (i + read); j++) {
            // Newline seek back to after it, return the size read
            if (buffer[j] == '\n') {
                size = j + 1;
                // Ensure that the computation worked and there is not overflow
                FW_ASSERT(size <= requested_size);
                FW_ASSERT(std::numeric_limits<FwSizeType>::max() - size >= original_location);
                (void)this->seek_absolute(original_location + j + 1);
                return Os::File::Status::OP_OK;
            }
        }
    }
    // Failed to find newline within data available
    return Os::File::Status::OTHER_ERROR;
}
}  // namespace Os
