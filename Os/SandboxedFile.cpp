// ======================================================================
// \title Os/SandboxedFile.cpp
// \brief Implementation of directory-sandboxed file wrapper
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/SandboxedFile.hpp>
#include <cstring>

namespace Os {

SandboxedFile::SandboxedFile() : m_file(), m_allowedDirectory(), m_configured(false) {}

SandboxedFile::~SandboxedFile() {
    if (m_file.isOpen()) {
        m_file.close();
    }
}

void SandboxedFile::configure(const char* allowedDirectory) {
    FW_ASSERT(allowedDirectory != nullptr);
    FW_ASSERT(!m_file.isOpen());

    const FwSizeType dirLen = Fw::StringUtils::string_length(allowedDirectory, FilePathUtils::MAX_PATH_LENGTH);
    FW_ASSERT(dirLen > 0);
    FW_ASSERT(dirLen < FilePathUtils::MAX_PATH_LENGTH);
    FW_ASSERT(allowedDirectory[0] == '/');
    FW_ASSERT(allowedDirectory[dirLen - 1] == '/');

    m_allowedDirectory = allowedDirectory;
    m_configured = true;
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

    char resolvedPath[FilePathUtils::MAX_PATH_LENGTH];
    const FilePathUtils::Status status =
        FilePathUtils::isSubDirectory(path, m_allowedDirectory.toChar(), resolvedPath, sizeof(resolvedPath));

    if (status != FilePathUtils::VALID) {
        return Os::FileInterface::Status::NO_PERMISSION;
    }

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
    return m_allowedDirectory.toChar();
}

}  // namespace Os
