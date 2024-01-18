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
    FW_ASSERT(reinterpret_cast<U8*>(handle) == handle_storage);
}

File::~File() {
    if (this->mode != OPEN_NO_MODE) {
        this->close();
    }
    this->destructInternal();
}

File::Status File::open(const CHAR* filepath, File::Mode requested_mode, bool overwrite) {
    FW_ASSERT(nullptr != filepath);
    FW_ASSERT(File::Mode::OPEN_NO_MODE < requested_mode && File::Mode::MAX_OPEN_MODE > requested_mode);
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check for already opened file
    if (this->isOpen()) {
        return File::Status::INVALID_MODE;
    }
    File::Status status = this->openInternal(filepath, requested_mode, overwrite);
    if (status == File::Status::OP_OK) {
        this->mode = requested_mode;
        this->path = filepath;
    }
    return status;
}

void File::close() {
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    this->closeInternal();
    this->mode = Mode::OPEN_NO_MODE;
    this->path = nullptr;
}

bool File::isOpen() const {
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    return this->mode != Mode::OPEN_NO_MODE;
}


File::Status File::preallocate(FwSizeType offset, FwSizeType length) {
    FW_ASSERT(offset >= 0);
    FW_ASSERT(length >= 0);
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->mode) {
        return File::Status::INVALID_MODE;
    }
    return this->preallocateInternal(offset, length);
}

File::Status File::seek(FwSizeType offset, bool absolute) {
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT(not absolute || offset >= 0);
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->mode) {
        return File::Status::NOT_OPENED;
    }
    return this->seekInternal(offset, absolute);
}

File::Status File::flush() {
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->mode) {
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->mode) {
        return File::Status::INVALID_MODE;
    }
    return this->flushInternal();
}

File::Status File::read(U8* buffer, FwSizeType &size, bool wait) {
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ != this->mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->readInternal(buffer, size, wait);
}

File::Status File::write(const void* buffer, FwSizeType &size, bool wait) {
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (OPEN_NO_MODE == this->mode) {
        size = 0;
        return File::Status::NOT_OPENED;
    } else if (OPEN_READ == this->mode) {
        size = 0;
        return File::Status::INVALID_MODE;
    }
    return this->writeInternal(buffer, size, wait);
}

File::CrcWorkingSet::CrcWorkingSet() : crc(INITIAL_CRC), eof(false) {}

File::Status File::updateCRC(Os::File::CrcWorkingSet& data, bool nice) {
    FwSizeType size = sizeof data.buffer;
    File::Status status = File::Status::OP_OK;
    FW_ASSERT(this->mode < Mode::MAX_OPEN_MODE);

    // Reopen file and seek in nice mode
    if (nice && not this->isOpen()) {
        status = this->open(this->path, this->mode);
        if (File::Status::OP_OK == status) {
            status = this->seek(data.offset, true);
        }
    }
    // On good status, read data
    if (File::Status::OP_OK == status) {
        status = this->read(data.buffer, size, false);
        // Zero size read, end-of-file reached
        if (File::Status::OP_OK == status && size == 0) {
            data.eof = true;
        }
        // Successful read, update CRC
        else if (File::Status::OP_OK == status) {
            // Close file before heavy computations in nice mode
            if (nice) {
                this->close();
            }
            for (FwSizeType i = 0; i < size; i++) {
                data.crc = update_crc_32(data.crc, data.buffer[i]);
            }
            data.offset += size;
        }
    }
    return status;
}


File::Status File::calculateCRC32(U32 &crc, bool nice) {
    // Make sure file is opened correctly
    if (OPEN_NO_MODE == this->mode) {
        crc = 0;
        return NOT_OPENED;
    } else if (OPEN_READ != this->mode) {
        crc = 0;
        return INVALID_MODE;
    }
    FW_ASSERT(this->path != nullptr);
    File::CrcWorkingSet crc_data;
    File::Status status = File::Status::OP_OK;
    for (FwSizeType i = 0; i < std::numeric_limits<FwSizeType>::max(); i++) {
        status = File::updateCRC(crc_data, nice);
        // End loop on bad status or end-of-file
        if (File::Status::OP_OK != status || crc_data.eof) {
            break;
        }
    }
    // Check if the whole file was read
    if (crc_data.eof) {
        crc = crc_data.crc;
    }
    // Correct for the condition when we had valid read but did not reach end of file
    else if (File::Status::OP_OK == status) {
        crc = 0;
        status = File::Status::OTHER_ERROR;
    }
    return status;
}
} // Os

