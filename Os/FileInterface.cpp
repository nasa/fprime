// ======================================================================
// \title Os/FileInterface.cpp
// \brief common function implementation for Os::FileInterface
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/FileInterface.hpp>
#include <algorithm>
#include <config/FppConstantsAc.hpp>

namespace Os {

FileInterface::FileInterface(const FileInterface& other) : m_mode(other.m_mode) {}

FileInterface& FileInterface::operator=(const FileInterface& other) {
    if (this != &other) {
        this->m_mode = other.m_mode;
    }
    return *this;
}

FileInterface::Status FileInterface::open(const char* filepath, FileInterface::Mode requested_mode) {
    return this->open(filepath, static_cast<FwSizeType>(FileNameStringSize + 1), requested_mode,
                      OverwriteType::NO_OVERWRITE);
}

FileInterface::Status FileInterface::open(const char* filepath, FwSizeType length, FileInterface::Mode requested_mode) {
    return this->open(filepath, length, requested_mode, OverwriteType::NO_OVERWRITE);
}

FileInterface::Status FileInterface::open(const char* filepath,
                                          FwSizeType length,
                                          FileInterface::Mode requested_mode,
                                          FileInterface::OverwriteType overwrite) {
    FW_ASSERT(nullptr != filepath);
    const FwSizeType string_len = static_cast<FwSizeType>(Fw::StringUtils::string_length(filepath, length));
    FW_ASSERT(string_len < length, static_cast<FwAssertArgType>(string_len), static_cast<FwAssertArgType>(length));
    FW_ASSERT(FileInterface::Mode::OPEN_NO_MODE < requested_mode &&
              FileInterface::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT((0 <= overwrite) && (overwrite < OverwriteType::MAX_OVERWRITE_TYPE));
    return this->open(filepath, requested_mode, overwrite);
}

FileInterface::Status FileInterface::open(const Fw::ConstStringBase& path, FileInterface::Mode requested_mode) {
    return this->open(path.toChar(), static_cast<FwSizeType>(path.getCapacity()), requested_mode,
                      OverwriteType::NO_OVERWRITE);
}

FileInterface::Status FileInterface::open(const Fw::ConstStringBase& path,
                                          FileInterface::Mode requested_mode,
                                          FileInterface::OverwriteType overwrite) {
    return this->open(path.toChar(), static_cast<FwSizeType>(path.getCapacity()), requested_mode, overwrite);
}

bool FileInterface::isOpen() const {
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    return this->m_mode != Mode::OPEN_NO_MODE;
}

FileInterface::Mode FileInterface::getMode() const {
    FW_ASSERT((0 <= this->m_mode) && (this->m_mode < Mode::MAX_OPEN_MODE));
    return this->m_mode;
}

void FileInterface::setMode(FileInterface::Mode mode) {
    FW_ASSERT((0 <= mode) && (mode < Mode::MAX_OPEN_MODE));
    this->m_mode = mode;
}

FileInterface::Status FileInterface::seek_absolute(FwSizeType offset) {
    Os::FileInterface::Status status = FileInterface::Status::OTHER_ERROR;
    // If the offset can be represented by a signed value, then we can perform a single seek
    if (static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()) >= offset) {
        // Check that the bounding above is correct
        FW_ASSERT(static_cast<FwSignedSizeType>(offset) >= 0);
        status = this->seek(static_cast<FwSignedSizeType>(offset), FileInterface::SeekType::ABSOLUTE);
    }
    // Otherwise, a full seek to any value represented by FwSizeType can be performed
    // by at most 3 seeks of a FwSignedSizeType. Two half seeks (rounded down) that are
    // strictly bounded by std::numeric_limits<FwSignedSizeType>::max() and one seek of
    // a possibile "odd" byte to ensure odds offsets do not introduce an off-by-one-error.
    // Thus we perform 3 seeks to guarantee that we can reach any position.
    else {
        FwSignedSizeType half_offset = static_cast<FwSignedSizeType>(offset >> 1);
        bool is_odd = (offset % 2) == 1;
        status = this->seek(half_offset, FileInterface::SeekType::ABSOLUTE);
        if (status == FileInterface::Status::OP_OK) {
            status = this->seek(half_offset, FileInterface::SeekType::RELATIVE);
        }
        if (status == FileInterface::Status::OP_OK) {
            status = this->seek((is_odd) ? 1 : 0, FileInterface::SeekType::RELATIVE);
        }
    }
    return status;
}

FileInterface::Status FileInterface::read(U8* buffer, FwSizeType& size) {
    return this->read(buffer, size, WaitType::WAIT);
}

FileInterface::Status FileInterface::write(const U8* buffer, FwSizeType& size) {
    return this->write(buffer, size, WaitType::WAIT);
}

// The CRC scratch state (`Utils::Hash` accumulator + `FW_FILE_CHUNK_SIZE`
// read buffer) and the working algorithm deliberately do not live on this
// base class -- see the design note in Os/FileInterface.hpp. These base
// implementations report the capability is unavailable; `Os::DelegateFile`
// (which `Os::File` aliases) overrides all three with the real logic.
FileInterface::Status FileInterface::calculateCrc(U32& crc) {
    crc = 0;
    return FileInterface::Status::NOT_SUPPORTED;
}

FileInterface::Status FileInterface::incrementalCrc(FwSizeType& size) {
    (void)size;
    return FileInterface::Status::NOT_SUPPORTED;
}

FileInterface::Status FileInterface::finalizeCrc(U32& crc) {
    crc = 0;
    return FileInterface::Status::NOT_SUPPORTED;
}

FileInterface::Status FileInterface::readline(U8* buffer, FwSizeType& size, FileInterface::WaitType wait) {
    const FwSizeType requested_size = size;
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return FileInterface::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return FileInterface::Status::INVALID_MODE;
    }
    FwSizeType original_location = 0;
    FileInterface::Status status = this->position(original_location);
    if (status != Os::FileInterface::Status::OP_OK) {
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
        if (status != FileInterface::Status::OP_OK) {
            // Contract: on error, seek back to the original location
            size = 0;
            (void)this->seek_absolute(original_location);
            return status;
        }
        // EOF break out now
        if (read == 0) {
            size = i;
            return Os::FileInterface::Status::OP_OK;
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
                return Os::FileInterface::Status::OP_OK;
            }
        }
    }
    // Failed to find newline within data available
    // Contract: on error, seek back to the original location
    size = 0;
    (void)this->seek_absolute(original_location);
    return Os::FileInterface::Status::OTHER_ERROR;
}
}  // namespace Os
