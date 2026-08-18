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
//! By default, the sandbox is set to `/` (root), which allows any absolute path.
//! Call `configure()` to restrict to a specific directory.
//!
//! Threat model assumption: untrusted actors cannot create symlinks inside the sandbox.
//! Path validation is purely textual (no `realpath()` calls) — it does not follow symlinks.
//! On embedded targets without symlink support this is a non-issue. On POSIX targets with
//! untrusted local write access to the sandbox directory, symlink-based escapes are possible;
//! additional measures (e.g., `O_NOFOLLOW`, `realpath()` integration) are needed for those
//! deployments.
//!
//! Usage:
//! ```cpp
//! Os::SandboxedFile file;
//! file.configure("/data/uplink/");
//! Os::File::Status status = file.open("/data/uplink/mission/seq.bin", Os::File::OPEN_WRITE);
//! // -> OP_OK
//! status = file.open("/data/uplink/../../etc/shadow", Os::File::OPEN_WRITE);
//! // -> OUTSIDE_SANDBOX
//! ```
//!
class SandboxedFile {
  public:
    SandboxedFile(const SandboxedFile&) = delete;             //!< Non-copyable (owns file handle)
    SandboxedFile& operator=(const SandboxedFile&) = delete;  //!< Non-copy-assignable

    //! \brief Construct a SandboxedFile with default sandbox of `/`
    //!
    //! The default allows any absolute path. Call `configure()` to restrict.
    //!
    SandboxedFile();

    //! \brief Destroy the SandboxedFile (closes the underlying file if open)
    ~SandboxedFile();

    //! \brief Configure the allowed sandbox directory
    //!
    //! Sets the directory that all file operations are restricted to.
    //! May be absolute or relative (relative paths are resolved against CWD).
    //! A trailing `/` is appended automatically if not present.
    //!
    //! Intended to be called once at startup. Preconditions are enforced
    //! with FW_ASSERT: must not be called while a file is open, directory
    //! must resolve successfully, must not overflow the buffer.
    //!
    //! \param allowedDirectory: path of the allowed directory (absolute or relative to CWD)
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
    //! Resolves the path against CWD and checks containment before opening.
    //! Returns `OUTSIDE_SANDBOX` if the resolved path falls outside the sandbox.
    //!
    //! \param path: c-string path to open
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return status of the open (OUTSIDE_SANDBOX if path validation fails)
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
