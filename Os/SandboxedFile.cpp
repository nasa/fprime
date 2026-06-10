// ======================================================================
// \title Os/SandboxedFile.cpp
// \brief Implementation of directory-sandboxed file wrapper
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/SandboxedFile.hpp>
#include <cstring>

namespace Os {

SandboxedFile::SandboxedFile() : m_file(), m_allowedDirectory(), m_configured(false) {
    m_allowedDirectory[0] = '\0';
}

SandboxedFile::~SandboxedFile() {
    if (m_file.isOpen()) {
        m_file.close();
    }
}

bool SandboxedFile::configure(const char* allowedDirectory) {
    FW_ASSERT(allowedDirectory != nullptr);

    // Cannot reconfigure while a file is open
    if (m_file.isOpen()) {
        return false;
    }

    // Must be an absolute path
    if (allowedDirectory[0] != '/') {
        return false;
    }

    const FwSizeType dirLen = Fw::StringUtils::string_length(allowedDirectory, FilePathValidator::MAX_PATH_LENGTH);

    if (dirLen == 0 || dirLen >= FilePathValidator::MAX_PATH_LENGTH) {
        return false;
    }

    // Copy and ensure trailing '/'
    (void)std::memcpy(m_allowedDirectory, allowedDirectory, dirLen);
    FwSizeType pos = dirLen;
    if (m_allowedDirectory[pos - 1] != '/') {
        if (pos + 1 >= FilePathValidator::MAX_PATH_LENGTH) {
            return false;
        }
        m_allowedDirectory[pos++] = '/';
    }
    m_allowedDirectory[pos] = '\0';

    m_configured = true;
    return true;
}

bool SandboxedFile::isConfigured() const {
    return m_configured;
}

Os::FileInterface::Status SandboxedFile::open(const char* path,
                                              Os::FileInterface::Mode mode,
                                              Os::FileInterface::OverwriteType overwrite) {
    FW_ASSERT(path != nullptr);

    if (!m_configured) {
        return Os::FileInterface::Status::NO_PERMISSION;
    }

    // Resolve the path to canonical form for validation AND for the actual open.
    // This ensures the path that gets opened is exactly the path that was validated.
    char resolvedPath[FilePathValidator::MAX_PATH_LENGTH];
    FilePathValidator::Status resolveStatus =
        FilePathValidator::resolvePath(path, m_allowedDirectory, resolvedPath, FilePathValidator::MAX_PATH_LENGTH);

    if (resolveStatus != FilePathValidator::VALID) {
        return Os::FileInterface::Status::NO_PERMISSION;
    }

    // Verify the resolved path is within the sandbox
    const FilePathValidator::Status containmentStatus =
        FilePathValidator::checkContainment(resolvedPath, m_allowedDirectory);

    if (containmentStatus != FilePathValidator::VALID) {
        return Os::FileInterface::Status::NO_PERMISSION;
    }

    // Open the resolved (canonical) path, not the original
    return m_file.open(resolvedPath, mode, overwrite);
}

void SandboxedFile::close() {
    m_file.close();
}

bool SandboxedFile::isOpen() const {
    return m_file.isOpen();
}

Os::FileInterface::Status SandboxedFile::size(FwSizeType& size_result) {
    return m_file.size(size_result);
}

Os::FileInterface::Status SandboxedFile::position(FwSizeType& position_result) {
    return m_file.position(position_result);
}

Os::FileInterface::Status SandboxedFile::preallocate(FwSizeType offset, FwSizeType length) {
    return m_file.preallocate(offset, length);
}

Os::FileInterface::Status SandboxedFile::seek(FwSignedSizeType offset, Os::FileInterface::SeekType seekType) {
    return m_file.seek(offset, seekType);
}

Os::FileInterface::Status SandboxedFile::flush() {
    return m_file.flush();
}

Os::FileInterface::Status SandboxedFile::read(U8* buffer, FwSizeType& size, Os::FileInterface::WaitType wait) {
    return m_file.read(buffer, size, wait);
}

Os::FileInterface::Status SandboxedFile::read(U8* buffer, FwSizeType& size) {
    return m_file.read(buffer, size);
}

Os::FileInterface::Status SandboxedFile::write(const U8* buffer, FwSizeType& size, Os::FileInterface::WaitType wait) {
    return m_file.write(buffer, size, wait);
}

Os::FileInterface::Status SandboxedFile::write(const U8* buffer, FwSizeType& size) {
    return m_file.write(buffer, size);
}

Os::FileInterface::Status SandboxedFile::calculateCrc(U32& crc) {
    return m_file.calculateCrc(crc);
}

const char* SandboxedFile::getSandboxDirectory() const {
    if (!m_configured) {
        return nullptr;
    }
    return m_allowedDirectory;
}

}  // namespace Os
