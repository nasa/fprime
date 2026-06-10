// ======================================================================
// \title Os/SandboxedFile.hpp
// \brief A directory-sandboxed wrapper around Os::File
// ======================================================================
#ifndef Os_SandboxedFile_hpp_
#define Os_SandboxedFile_hpp_

#include <Fw/Types/FileNameString.hpp>
#include <Os/File.hpp>
#include <Os/FilePathUtils.hpp>

namespace Os {

//! \brief A wrapper around Os::File that restricts file access to an allowed directory
//!
//! SandboxedFile provides the same interface as Os::File but validates all paths on `open()`
//! to ensure they resolve to a location within a configured allowed directory. This prevents
//! path-traversal attacks (e.g., `../../etc/shadow`) from components that accept externally
//! supplied file paths (such as FileUplink or CFDP receivers).
//!
//! `configure()` must be called before `open()`. Calling `open()` without configuring
//! returns `NO_PERMISSION`.
//!
//! Note: path validation is purely textual (no `realpath()` calls) — it does not follow
//! symlinks. For deployments where symlink-based escapes are a concern, additional
//! measures are needed.
//!
//! Usage:
//! ```cpp
//! Os::SandboxedFile file;
//! file.configure("/data/uplink/");
//! Os::File::Status status = file.open("/data/uplink/mission/seq.bin", Os::File::OPEN_WRITE);
//! // -> OP_OK
//! status = file.open("/data/uplink/../../etc/shadow", Os::File::OPEN_WRITE);
//! // -> NO_PERMISSION
//! ```
//!
class SandboxedFile {
  public:
    //! \brief Construct an unconfigured SandboxedFile
    //!
    //! `configure()` must be called before `open()`.
    //!
    SandboxedFile();

    //! \brief Destroy the SandboxedFile (closes the underlying file if open)
    ~SandboxedFile();

    //! \brief Configure the allowed sandbox directory
    //!
    //! Sets the directory that all file operations are restricted to.
    //! The directory must be an absolute path ending with `/`.
    //!
    //! Intended to be called once at startup. Preconditions are enforced
    //! with FW_ASSERT: must not be called while a file is open, directory
    //! must be absolute, must not be empty, must end with `/`, must not
    //! overflow the buffer.
    //!
    //! \param allowedDirectory: absolute path of the allowed directory (must end with `/`)
    //!
    void configure(const char* allowedDirectory);

    //! \brief Check if a sandbox directory has been configured
    //! \return true if configure() has been called successfully
    //!
    bool isConfigured() const;

    // -------------------------------------------------------
    // Os::File-equivalent interface
    // -------------------------------------------------------

    //! \brief Open a file, validating the path against the sandbox directory
    //!
    //! Resolves the path and checks containment before opening.
    //! Returns `NO_PERMISSION` if the sandbox is not configured or the
    //! resolved path falls outside the sandbox.
    //!
    //! \param path: c-string path to open
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return status of the open (NO_PERMISSION if path validation fails)
    //!
    Os::FileInterface::Status open(const char* path,
                                   Os::FileInterface::Mode mode,
                                   Os::FileInterface::OverwriteType overwrite = Os::FileInterface::NO_OVERWRITE);

    //! \brief Close the file
    void close();

    //! \brief Check if the file is open
    //! \return true if file is open
    bool isOpen() const;

    //! \brief Get size of currently open file
    //! \param size_result: output for file size
    //! \return OP_OK on success
    Os::FileInterface::Status size(FwSizeType& size_result);

    //! \brief Get file pointer position
    //! \param position_result: output for position
    //! \return OP_OK on success
    Os::FileInterface::Status position(FwSizeType& position_result);

    //! \brief Pre-allocate file storage
    //! \param offset: offset into file
    //! \param length: length to preallocate
    //! \return OP_OK on success
    Os::FileInterface::Status preallocate(FwSizeType offset, FwSizeType length);

    //! \brief Seek file pointer
    //! \param offset: offset to seek to
    //! \param seekType: ABSOLUTE or RELATIVE
    //! \return OP_OK on success
    Os::FileInterface::Status seek(FwSignedSizeType offset, Os::FileInterface::SeekType seekType);

    //! \brief Flush file contents to storage
    //! \return OP_OK on success
    Os::FileInterface::Status flush();

    //! \brief Read data from the file
    //! \param buffer: destination buffer
    //! \param size: bytes to read (updated with actual count)
    //! \param wait: WAIT or NO_WAIT
    //! \return OP_OK on success
    Os::FileInterface::Status read(U8* buffer, FwSizeType& size, Os::FileInterface::WaitType wait);

    //! \brief Read data from the file (blocking, default)
    //! \param buffer: destination buffer
    //! \param size: bytes to read (updated with actual count)
    //! \return OP_OK on success
    Os::FileInterface::Status read(U8* buffer, FwSizeType& size);

    //! \brief Write data to the file
    //! \param buffer: source buffer
    //! \param size: bytes to write (updated with actual count)
    //! \param wait: WAIT or NO_WAIT
    //! \return OP_OK on success
    Os::FileInterface::Status write(const U8* buffer, FwSizeType& size, Os::FileInterface::WaitType wait);

    //! \brief Write data to the file (blocking, default)
    //! \param buffer: source buffer
    //! \param size: bytes to write (updated with actual count)
    //! \return OP_OK on success
    Os::FileInterface::Status write(const U8* buffer, FwSizeType& size);

    //! \brief Calculate CRC32 of the entire file
    //! \param crc: output for CRC value
    //! \return OP_OK on success
    Os::FileInterface::Status calculateCrc(U32& crc);

    //! \brief Get the configured sandbox directory
    //! \return pointer to the sandbox directory string, or nullptr if unconfigured
    const char* getSandboxDirectory() const;

  private:
    //! The underlying OS file
    Os::File m_file;

    //! The allowed directory (absolute, with trailing '/')
    Fw::FileNameString m_allowedDirectory;

    //! Whether configure() has been called
    bool m_configured;
};

}  // namespace Os

#endif
