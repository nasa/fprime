// ======================================================================
// \title Os/test/ut/file/SyntheticFileSystem.hpp
// \brief stl-driven synthetic file system definitions
// ======================================================================
#include "config/FpConfig.h"
#include "Os/File.hpp"
#include <map>
#include <vector>
#include <string>

#ifndef OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM
#define OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM

namespace Os {
namespace Test {
// Forward declaration
    class SyntheticFileSystem;

/**
 * \brief Synthetic file data implementation
 *
 * File implementation for a synthetic stl-based file.
 */
class SyntheticFile {
  public:
    friend class SyntheticFileSystem;

    /**
     * \brief synthetic file constructor
     */
    SyntheticFile() = default;

    /**
     * \brief synthetic file destructor
     */
    ~SyntheticFile() = default;

    /**
     * \brief close the file
     */
    void close();

    /**
     * \brief read data from the file
     *
     * Read from the synthetic file and fill the buffer up-to size. Fill size with the data that was read.
     *
     * \param buffer: buffer to fill
     * \param size: size of data to read
     * \param bool: wait, unused
     * \return status of the read
     */
    Os::File::Status read(U8 *buffer, FwSignedSizeType &size, bool wait);

    /**
     * \brief write data to the file
     *
     * Write to the synthetic file from the buffer of given size. Fill size with the data that was written.
     *
     * \param buffer: buffer to fill
     * \param size: size of data to read
     * \param bool: wait, unused
     * \return status of the write
     */
    Os::File::Status write(const void *buffer, FwSignedSizeType &size, bool wait);

    /**
     * \brief seek pointer within file
     *
     * Seek the pointer within the file.
     *
     * \param offset: offset to seek to
     * \param bool: absolute
     * \return status of the seek
     */
    Os::File::Status seek(const FwSignedSizeType offset, const bool absolute);

    /**
     * \brief preallocate data within file
     *
     * Preallocate data within the file.
     *
     * \param offset: offset to start pre-allocation
     * \param length: length of the preallocation
     * \return status of the preallocate
     */
    Os::File::Status preallocate(const FwSignedSizeType offset, const FwSignedSizeType length);

    /**
     * \brief flush is no-op
     */
    Os::File::Status flush();

    //! \brief pointer getter
    FwSignedSizeType getPointer();

    //! \brief size getter
    FwSignedSizeType getSize();

    //! \brief size getter
    Os::File::Mode getMode();
  private:
    //! Path of this file
    std::string m_path;
    //! Data stored in the file
    std::vector<U8> m_data;
    //! Pointer of the file
    FwSignedSizeType m_pointer = -1;
    //! Mode the file is in
    Os::File::Mode m_mode;
};


/**
 * \brief Synthetic file system implementation
 *
 * A synthetic stl-based in-memory file system for use with testing. It is composed of a map of string paths to a
 * synthetic file data packet that tracks data and file pointer
 */
class SyntheticFileSystem {
  public:
    /**
     * \brief data returned by the open call
     */
    struct OpenData {
        std::shared_ptr<SyntheticFile> file = nullptr;
        Os::File::Status status = Os::File::Status::OTHER_ERROR;
    };

    //! Constructor
    SyntheticFileSystem() = default;

    //! Destructor
    virtual ~SyntheticFileSystem() = default;

    /**
     * \breif open a given path with mode and overwrite
     *
     * This opens a file at the given path. Mode drives the mode of the file and overwrite will overwrite files if it
     * exists and was created. Returns a pair of status and synthetic file data.
     *
     * \param path: path to open
     * \param mode: mode to open with
     * \param overwrite: overwrite if exists
     * \return (status, synthetic file object)
     */
    OpenData open(const CHAR *path, const Os::File::Mode mode, const bool overwrite = false);

    /**
     * \brief check a file exists
     *
     * Check if a file exists.
     *
     * \return: true if exists, false otherwise
     */
    bool exists(const CHAR *path);

    /**
     * \brief get file at path
     *
     * This will get the file associated with the given path. File *must* exist (via open call).
     *
     * \param path: path to solicit
     * \return: synthetic file data
     */
    std::shared_ptr<SyntheticFile> getFile(const CHAR *path);

  private:

    //! Shadow file state: created but unopened files
    std::vector<std::shared_ptr<SyntheticFile>> m_unopened;
    //! Shadow file state: file system
    std::map<std::string, std::shared_ptr<SyntheticFile>> m_filesystem;
};

} // namespace Test
} // namespace Os

#endif // OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM