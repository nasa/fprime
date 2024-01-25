// ======================================================================
// \title Os/File.cpp
// \brief common function implementation for Os::File
// ======================================================================
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC
#ifdef __cplusplus
}
#endif // __cplusplus
namespace Os {

File::File() {
    this->constructInternal();
    FW_ASSERT(reinterpret_cast<U8*>(m_handle) == m_handle_storage);
}

File::~File() {
    if (this->m_mode != OPEN_NO_MODE) {
        this->close();
    }
    this->destructInternal();
}

File::Status File::open(const CHAR* filepath, File::Mode requested_mode, File::OverwriteType overwrite) {
    FW_ASSERT(nullptr != filepath);
    FW_ASSERT(File::Mode::OPEN_NO_MODE < requested_mode && File::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check for already opened file
    if (this->isOpen()) {
        return File::Status::INVALID_MODE;
    }
    File::Status status = this->openInternal(filepath, requested_mode, overwrite);
    if (status == File::Status::OP_OK) {
        this->m_mode = requested_mode;
        this->m_path = filepath;
    }
    return status;
}

void File::close() {
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    this->closeInternal();
    this->m_mode = Mode::OPEN_NO_MODE;
    this->m_path = nullptr;
}

bool File::isOpen() const {
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    return this->m_mode != Mode::OPEN_NO_MODE;
}

File::Status File::size(FwSignedSizeType& size_result) {
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->sizeInternal(size_result);
}

File::Status File::position(FwSignedSizeType &position_result) {
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->positionInternal(position_result);
}

File::Status File::preallocate(FwSignedSizeType offset, FwSignedSizeType length) {
    FW_ASSERT(offset >= 0);
    FW_ASSERT(length >= 0);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return File::Status::INVALID_MODE;
    }
    return this->preallocateInternal(offset, length);
}

File::Status File::seek(FwSignedSizeType offset, File::SeekType seekType) {
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT((seekType == File::SeekType::RELATIVE) || (offset >= 0));
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    }
    return this->seekInternal(offset, seekType);
}

File::Status File::flush() {
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        return File::Status::INVALID_MODE;
    }
    return this->flushInternal();
}

File::Status File::read(U8* buffer, FwSignedSizeType &size, File::WaitType wait) {
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->readInternal(buffer, size, wait);
}

File::Status File::write(const U8* buffer, FwSignedSizeType &size, File::WaitType wait) {
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->m_mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->writeInternal(buffer, size, wait);
}

File::CrcWorkingSet::CrcWorkingSet() : m_offset(0), m_crc(INITIAL_CRC), m_buffer(), m_eof(false) {}

File::Status File::updateCRC(Os::File::CrcWorkingSet& data, bool nice) {
    FwSignedSizeType size = sizeof data.m_buffer;
    File::Status status = File::Status::OP_OK;
    FW_ASSERT(this->m_mode < Mode::MAX_OPEN_MODE);

    // Reopen file and seek in nice mode
    if (nice && not this->isOpen()) {
        status = this->open(this->m_path, this->m_mode);
        if (File::Status::OP_OK == status) {
            status = this->seek(data.m_offset, File::SeekType::ABSOLUTE);
        }
    }
    // On good status, read data
    if (File::Status::OP_OK == status) {
        status = this->read(data.m_buffer, size, File::WaitType::NO_WAIT);
        // Zero size read, end-of-file reached
        if (File::Status::OP_OK == status && size == 0) {
            data.m_eof = true;
        }
        // Successful read, update CRC
        else if (File::Status::OP_OK == status) {
            // Close file before heavy computations in nice mode
            if (nice) {
                this->close();
            }
            for (FwSignedSizeType i = 0; i < size; i++) {
                data.m_crc = update_crc_32(data.m_crc, data.m_buffer[i]);
            }
            data.m_offset += size;
        }
    }
    return status;
}


File::Status File::calculateCRC32(U32 &crc, bool nice) {
    // Make sure file is opened correctly
    if (OPEN_NO_MODE == this->m_mode) {
        crc = 0;
        return NOT_OPENED;
    } else if (OPEN_READ != this->m_mode) {
        crc = 0;
        return INVALID_MODE;
    }
    FW_ASSERT(this->m_path != nullptr);
    File::CrcWorkingSet crc_data;
    File::Status status = File::Status::OP_OK;
    for (FwSignedSizeType i = 0; i < std::numeric_limits<FwSignedSizeType>::max(); i++) {
        status = File::updateCRC(crc_data, nice);
        // End loop on bad status or end-of-file
        if (File::Status::OP_OK != status || crc_data.m_eof) {
            break;
        }
    }
    // Check if the whole file was read
    if (crc_data.m_eof) {
        crc = crc_data.m_crc;
    }
    // Correct for the condition when we had valid read but did not reach end of file
    else if (File::Status::OP_OK == status) {
        crc = 0;
        status = File::Status::OTHER_ERROR;
    }
    return status;
}
} // Os

