// ======================================================================
// \title Os/test/ut/file/SyntheticFileSystem.cpp
// \brief standard template library driven synthetic file system implementation
// ======================================================================
#include "Os/test/ut/file/SyntheticFileSystem.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Test {

SyntheticFileSystem::OpenData SyntheticFileSystem::open(const CHAR* char_path, const Os::File::Mode open_mode, const File::OverwriteType overwrite) {
    SyntheticFileSystem::OpenData return_value;
    std::string path = char_path;
    bool exists = (this->m_filesystem.count(path) > 0);

    // Error case: read on file that does not exist
    if ((not exists) && (Os::File::Mode::OPEN_READ == open_mode)) {
        return_value.status = Os::File::Status::DOESNT_EXIST;
    }
    // Error case: create on existing file without overwrite
    else if (exists and (not overwrite) && (open_mode == Os::File::Mode::OPEN_CREATE)) {
        return_value.status = Os::File::Status::FILE_EXISTS;
    }
    // Case where file should be opened correctly
    else {
        const bool truncate = (Os::File::Mode::OPEN_CREATE == open_mode) && overwrite;

        // Add new shadow file data when the file is new
        if (not exists) {
            this->m_filesystem[path] = std::make_shared<SyntheticFileData>();
        }
        return_value.file = this->m_filesystem[path];
        return_value.status = Os::File::Status::OP_OK;

        // Set file properties
        if (truncate) {
            return_value.file->m_data.clear();
        }
        return_value.file->m_pointer = 0;
        return_value.file->m_mode = open_mode;
        return_value.file->m_path = path;
        // Checks on the shadow data to ensure consistency
        FW_ASSERT(return_value.file->m_mode != Os::File::OPEN_NO_MODE);
        FW_ASSERT(return_value.file->m_pointer == 0);
        FW_ASSERT(return_value.file->m_path == path);
        FW_ASSERT(not truncate || return_value.file->m_data.empty());
    }
    return return_value;
}

bool SyntheticFileSystem::exists(const CHAR* char_path) {
    std::string path = char_path;
    FW_ASSERT(this->m_filesystem.count(path) <= 1);
    return this->m_filesystem.count(path) == 1;
}

void SyntheticFileSystem::remove(const CHAR* char_path) {
    std::string path = char_path;
    this->m_filesystem.erase(path);
}

std::unique_ptr<SyntheticFileSystem> SyntheticFile::s_file_system = std::unique_ptr<SyntheticFileSystem>(new SyntheticFileSystem());

void SyntheticFile::setFileSystem(std::unique_ptr<SyntheticFileSystem> new_file_system) {
    s_file_system = std::move(new_file_system);
}

void SyntheticFile::remove(const CHAR* char_path) {
    FW_ASSERT(s_file_system != nullptr);
    s_file_system->remove(char_path);
}

File::Status SyntheticFile::open(const CHAR* char_path, const Os::File::Mode open_mode, const File::OverwriteType overwrite) {
    SyntheticFileSystem::OpenData data = s_file_system->open(char_path, open_mode, overwrite);
    if (data.status == Os::File::Status::OP_OK) {
        this->m_data = data.file;
        FW_ASSERT(this->m_data != nullptr);
    }
    return data.status;
}

void SyntheticFile::close() {
    if (this->m_data != nullptr) {
        this->m_data->m_mode = Os::File::Mode::OPEN_NO_MODE;
        this->m_data->m_path.clear();
        // Checks on the shadow data to ensure consistency
        FW_ASSERT(this->m_data->m_mode == Os::File::Mode::OPEN_NO_MODE);
        FW_ASSERT(this->m_data->m_path.empty());
    }
}

Os::File::Status SyntheticFile::read(U8* buffer, FwSignedSizeType& size, WaitType wait) {
    (void) wait;
    FW_ASSERT(this->m_data != nullptr);
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->m_data->m_mode < Os::File::Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (Os::File::Mode::OPEN_NO_MODE == this->m_data->m_mode) {
        size = 0;
        return Os::File::Status::NOT_OPENED;
    } else if (Os::File::Mode::OPEN_READ != this->m_data->m_mode) {
        size = 0;
        return Os::File::Status::INVALID_MODE;
    }
    std::vector<U8> output;
    FwSignedSizeType original_pointer = this->m_data->m_pointer;
    FwSignedSizeType original_size = this->m_data->m_data.size();
    // Check expected read bytes
    FwSignedSizeType i = 0;
    for (i = 0; i < size; i++, this->m_data->m_pointer++) {
        // End of file
        if (this->m_data->m_pointer >= static_cast<FwSignedSizeType>(this->m_data->m_data.size())) {
            break;
        }
        buffer[i] = this->m_data->m_data.at(this->m_data->m_pointer);
    }
    size = i;
    // Checks on the shadow data to ensure consistency
    FW_ASSERT(this->m_data->m_data.size() == static_cast<size_t>(original_size));
    FW_ASSERT(this->m_data->m_pointer == ((original_pointer > original_size) ? original_pointer : FW_MIN(original_pointer + size, original_size)));
    FW_ASSERT(size == ((original_pointer > original_size) ? 0 : FW_MIN(size, original_size - original_pointer)));
    return Os::File::Status::OP_OK;
}

Os::File::Status SyntheticFile::write(const U8* buffer, FwSignedSizeType& size, WaitType wait) {
    (void) wait;
    FW_ASSERT(this->m_data != nullptr);
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size >= 0);
    FW_ASSERT(this->m_data->m_mode < Os::File::Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (Os::File::Mode::OPEN_NO_MODE == this->m_data->m_mode) {
        size = 0;
        return Os::File::Status::NOT_OPENED;
    } else if (Os::File::Mode::OPEN_READ == this->m_data->m_mode) {
        size = 0;
        return Os::File::Status::INVALID_MODE;
    }
    FwSignedSizeType original_position = this->m_data->m_pointer;
    FwSignedSizeType original_size = this->m_data->m_data.size();
    const U8* write_data = reinterpret_cast<const U8*>(buffer);

    // Appends seek to end before writing
    if (Os::File::Mode::OPEN_APPEND == this->m_data->m_mode) {
        this->m_data->m_pointer = this->m_data->m_data.size();
    }

    // First add in zeros to account for a pointer past the end of the file
    const FwSignedSizeType zeros = static_cast<FwSignedSizeType>(this->m_data->m_pointer) - static_cast<FwSignedSizeType>(this->m_data->m_data.size());
    for (FwSignedSizeType i = 0; i < zeros; i++) {
        this->m_data->m_data.push_back(0);
    }
    // Interim checks to ensure zeroing performed correctly
    FW_ASSERT(static_cast<size_t>(this->m_data->m_pointer) <= this->m_data->m_data.size());
    FW_ASSERT(this->m_data->m_data.size() ==
              static_cast<size_t>((Os::File::Mode::OPEN_APPEND == this->m_data->m_mode) ? original_size : FW_MAX(original_position, original_size)));

    FwSignedSizeType pre_write_position = this->m_data->m_pointer;
    FwSignedSizeType pre_write_size = this->m_data->m_data.size();

    // Next write data
    FwSignedSizeType i = 0;
    for (i = 0; i < size; i++, this->m_data->m_pointer++) {
        // Overwrite case
        if (static_cast<size_t>(this->m_data->m_pointer) < this->m_data->m_data.size()) {
            this->m_data->m_data.at(this->m_data->m_pointer) = write_data[i];
        }
        // Append case
        else {
            this->m_data->m_data.push_back(write_data[i]);
        }
    }
    size = i;
    // Checks on the shadow data to ensure consistency
    FW_ASSERT(this->m_data->m_data.size() ==
              static_cast<size_t>(FW_MAX(pre_write_position + size, pre_write_size)));
    FW_ASSERT(this->m_data->m_pointer ==
              ((Os::File::Mode::OPEN_APPEND == this->m_data->m_mode) ? pre_write_size : pre_write_position) + size);
    return Os::File::Status::OP_OK;
}

Os::File::Status SyntheticFile::seek(const FwSignedSizeType offset, const SeekType absolute) {
    FW_ASSERT(this->m_data != nullptr);
    Os::File::Status status = Os::File::Status::OP_OK;
    // Cannot do a seek with a negative offset in absolute mode
    FW_ASSERT(not absolute || offset >= 0);
    FW_ASSERT(this->m_data->m_mode < Os::File::Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (Os::File::Mode::OPEN_NO_MODE == this->m_data->m_mode) {
        status = Os::File::Status::NOT_OPENED;
    } else {
        FwSignedSizeType new_offset = (absolute) ? offset : (offset + this->m_data->m_pointer);
        if (new_offset > 0) {
            this->m_data->m_pointer = new_offset;
        } else {
            status = Os::File::Status::INVALID_ARGUMENT;
        }
    }
    return status;
}

Os::File::Status SyntheticFile::preallocate(const FwSignedSizeType offset, const FwSignedSizeType length) {
    FW_ASSERT(this->m_data != nullptr);
    Os::File::Status status = Os::File::Status::OP_OK;
    FW_ASSERT(offset >= 0);
    FW_ASSERT(length >= 0);
    FW_ASSERT(this->m_data->m_mode < Os::File::Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (Os::File::Mode::OPEN_NO_MODE == this->m_data->m_mode) {
        status = Os::File::Status::NOT_OPENED;
    } else if (Os::File::Mode::OPEN_READ == this->m_data->m_mode) {
        status = Os::File::Status::INVALID_MODE;
    } else {
        const FwSignedSizeType original_size = this->m_data->m_data.size();
        const FwSignedSizeType new_length = offset + length;
        // Loop from existing size to new size adding zeros
        for (FwSignedSizeType i = static_cast<FwSignedSizeType>(this->m_data->m_data.size()); i < new_length; i++) {
            this->m_data->m_data.push_back(0);
        }
        FW_ASSERT(this->m_data->m_data.size() == static_cast<size_t>(FW_MAX(offset + length, original_size)));
    }
    return status;
}

Os::File::Status SyntheticFile::flush() {
    FW_ASSERT(this->m_data != nullptr);
    Os::File::Status status = Os::File::Status::OP_OK;
    FW_ASSERT(this->m_data->m_mode < Os::File::Mode::MAX_OPEN_MODE);
    // Check that the file is open before attempting operation
    if (Os::File::Mode::OPEN_NO_MODE == this->m_data->m_mode) {
        status = Os::File::Status::NOT_OPENED;
    } else if (Os::File::Mode::OPEN_READ == this->m_data->m_mode) {
        status = Os::File::Status::INVALID_MODE;
    }
    return status;
}

Os::File::Status SyntheticFile::position(FwSignedSizeType &position) {
    position = this->m_data->m_pointer;
    return Os::File::OP_OK;
}

Os::File::Status SyntheticFile::size(FwSignedSizeType &size) {
    size = static_cast<FwSignedSizeType>(this->m_data->m_data.size());
    return Os::File::OP_OK;
}

FileHandle* SyntheticFile::getHandle() {
    return this->m_data.get();
}

bool SyntheticFile::exists(const CHAR* path) {
    return s_file_system->exists(path);
}

} // namespace Test
} // namespace Os
