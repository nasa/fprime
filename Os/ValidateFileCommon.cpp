#include <Os/ValidateFile.hpp>
#include <Os/File.hpp>
#include <Utils/Hash/Hash.hpp>
#include <Os/FileSystem.hpp>

namespace Os {

    File::Status computeHash(const char* fileName, Utils::HashBuffer &hashBuffer) {

        File::Status status;

        // Open file:
        File file;
        status = file.open(fileName, File::OPEN_READ);
        if( File::OP_OK != status ) {
            return status;
        }

        // Get the file size:
        FileSystem::Status fs_status;
        FwSignedSizeType fileSize = 0;
        fs_status = FileSystem::getFileSize(fileName, fileSize); //!< gets the size of the file (in bytes) at location path
        // fileSize will be used as a NATIVE_INT_TYPE below and thus must cast cleanly to that type
        if( FileSystem::OP_OK != fs_status) {
            return File::BAD_SIZE;
        }
        const NATIVE_INT_TYPE max_itr = static_cast<NATIVE_INT_TYPE>(fileSize/VFILE_HASH_CHUNK_SIZE + 1);

        // Read all data from file and update hash:
        Utils::Hash hash;
        hash.init();
        U8 buffer[VFILE_HASH_CHUNK_SIZE];
        FwSignedSizeType size = 0;
        FwSignedSizeType cnt = 0;
        while( cnt <= max_itr ) {
            // Read out chunk from file:
            size = sizeof(buffer);
            status = file.read(buffer, size, Os::File::WaitType::NO_WAIT);
            if( File::OP_OK != status ) {
                return status;
            }
            // If end of file, break:
            if( size == 0 ) {
              break;
            }
            // Add chunk to hash calculation:
            hash.update(&buffer, size);
            cnt++;
        }
        file.close();

        // We should not have left the loop because of cnt > max_itr:
        FW_ASSERT(size == 0);
        FW_ASSERT(cnt <= max_itr);

        // Calculate hash:
        Utils::HashBuffer computedHashBuffer;
        hash.final(computedHashBuffer);
        hashBuffer = computedHashBuffer;

        return status;
    }

    File::Status readHash(const char* hashFileName, Utils::HashBuffer &hashBuffer) {

        File::Status status;

        // Open hash file:
        File hashFile;
        status = hashFile.open(hashFileName, File::OPEN_READ);
        if( File::OP_OK != status ) {
            return status;
        }

        // Read hash from checksum file:
        unsigned char savedHash[HASH_DIGEST_LENGTH];
        FwSignedSizeType size = hashBuffer.getBuffCapacity();
        status = hashFile.read(savedHash, size);
        if( File::OP_OK != status ) {
            return status;
        }
        if( size != static_cast<NATIVE_INT_TYPE>(hashBuffer.getBuffCapacity()) ) {
            return File::BAD_SIZE;
        }
        hashFile.close();

        // Return the hash buffer:
        Utils::HashBuffer savedHashBuffer(savedHash, size);
        hashBuffer = savedHashBuffer;

        return status;
    }

    File::Status writeHash(const char* hashFileName, Utils::HashBuffer hashBuffer) {
        // Open hash file:
        File hashFile;
        File::Status status;
        status = hashFile.open(hashFileName, File::OPEN_WRITE);
        if( File::OP_OK != status ) {
            return status;
        }

        // Write out the hash
        FwSignedSizeType size = hashBuffer.getBuffLength();
        status = hashFile.write(hashBuffer.getBuffAddr(), size, Os::File::WaitType::NO_WAIT);
        if( File::OP_OK != status ) {
            return status;
        }
        if( size != static_cast<NATIVE_INT_TYPE>(hashBuffer.getBuffLength()) ) {
            return File::BAD_SIZE;
        }
        hashFile.close();

        return status;
    }

    // Enum and function for translating from a status to a validation status:
    typedef enum {
        FileType,
        HashFileType
    } StatusFileType;

    ValidateFile::Status translateStatus(File::Status status, StatusFileType type) {

        switch (type) {
            case FileType:
                switch (status) {
                    case File::OP_OK:
                        return ValidateFile::VALIDATION_OK;
                    case File::DOESNT_EXIST:
                        return ValidateFile::FILE_DOESNT_EXIST;
                    case File::NO_SPACE:
                        return ValidateFile::NO_SPACE;
                    case File::NO_PERMISSION:
                        return ValidateFile::FILE_NO_PERMISSION;
                    case File::BAD_SIZE:
                        return ValidateFile::FILE_BAD_SIZE;
                    case File::NOT_OPENED:
                        return ValidateFile::OTHER_ERROR;
                    case File::OTHER_ERROR:
                        return ValidateFile::OTHER_ERROR;
                    default:
                        FW_ASSERT(0, status);
                }
                break;
            case HashFileType:
                switch (status) {
                    case File::OP_OK:
                        return ValidateFile::VALIDATION_OK;
                    case File::DOESNT_EXIST:
                        return ValidateFile::VALIDATION_FILE_DOESNT_EXIST;
                    case File::NO_SPACE:
                        return ValidateFile::NO_SPACE;
                    case File::NO_PERMISSION:
                        return ValidateFile::VALIDATION_FILE_NO_PERMISSION;
                    case File::BAD_SIZE:
                        return ValidateFile::VALIDATION_FILE_BAD_SIZE;
                    case File::NOT_OPENED:
                        return ValidateFile::OTHER_ERROR;
                    case File::OTHER_ERROR:
                        return ValidateFile::OTHER_ERROR;
                    default:
                        FW_ASSERT(0, status);
                }
                break;
            default:
                FW_ASSERT(0, type);
        }

        return ValidateFile::OTHER_ERROR;
    }

    ValidateFile::Status ValidateFile::validate(const char* fileName, const char* hashFileName) {
        Utils::HashBuffer hashBuffer; // pass by reference - final value is unused
        return validate(fileName, hashFileName, hashBuffer);
    }

    ValidateFile::Status ValidateFile::validate(const char* fileName, const char* hashFileName, Utils::HashBuffer &hashBuffer) {

        File::Status status;

        // Read the hash file:
        Utils::HashBuffer savedHash;
        status = readHash(hashFileName, savedHash);
        if( File::OP_OK != status ) {
            return translateStatus(status, HashFileType);
        }

        // Compute the file's hash:
        Utils::HashBuffer computedHash;
        status = computeHash(fileName, computedHash);
        if( File::OP_OK != status ) {
            return translateStatus(status, FileType);
        }

        // Compare hashes and return:
        if( savedHash != computedHash ){
            return ValidateFile::VALIDATION_FAIL;
        }

        hashBuffer = savedHash;

        return ValidateFile::VALIDATION_OK;
    }

    ValidateFile::Status ValidateFile::createValidation(const char* fileName, const char* hashFileName, Utils::HashBuffer &hashBuffer) {

        File::Status status;

        // Compute the file's hash:
        status = computeHash(fileName, hashBuffer);
        if( File::OP_OK != status ) {
            return translateStatus(status, FileType);
        }

        status = writeHash(hashFileName, hashBuffer);
        if( File::OP_OK != status ) {
            return translateStatus(status, HashFileType);
        }

        return ValidateFile::VALIDATION_OK;
    }

    ValidateFile::Status ValidateFile::createValidation(const char* fileName, const char* hashFileName) {
        Utils::HashBuffer hashBuffer; // pass by reference - final value is unused
        return createValidation(fileName, hashFileName, hashBuffer);
    }

}
