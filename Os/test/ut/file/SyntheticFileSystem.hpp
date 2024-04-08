// ======================================================================
// \title Os/test/ut/file/SyntheticFileSystem.hpp
// \brief standard template library driven synthetic file system definitions
// ======================================================================
#include <FpConfig.h>
#include "Os/File.hpp"
#include <map>
#include <memory>
#include <vector>
#include <string>

#ifndef OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM
#define OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM

namespace Os {
namespace Test {
// Forward declaration
class SyntheticFileSystem;

struct SyntheticFileData : public FileHandle {
    //! Path of this file
    std::string m_path;
    //! Data stored in the file
    std::vector<U8> m_data;
    //! Pointer of the file
    FwSignedSizeType m_pointer = -1;
    //! Separate mode tracking
    File::Mode m_mode = File::OPEN_NO_MODE;
};

//! \brief Synthetic file data implementation
//!
//! File implementation for a synthetic standard template library based file.
//!
class SyntheticFile : public FileInterface {
  public:
    friend class SyntheticFileSystem;

    //! \brief check if file exists
    static bool exists(const CHAR* path);

    //! \brief set the file system
    static void setFileSystem(std::unique_ptr<SyntheticFileSystem> new_file_system);

    //! \brief remove a file by path
    static void remove(const CHAR* path);

    //! \brief open a given path with mode and overwrite
    //!
    //! This opens a file at the given path. Mode drives the mode of the file and overwrite will overwrite files if it
    //! exists and was created. Returns a pair of status and synthetic file data.
    //!
    //! \param path: path to open
    //! \param mode: mode to open with
    //! \param overwrite: overwrite if exists
    //! \return (status, synthetic file object)
    //!
    Status open(const CHAR *path, const Os::File::Mode mode, const OverwriteType overwrite) override;

    //! \brief close the file
    //!
    void close() override;

    //! \brief read data from the file
    //!
    //! Read from the synthetic file and fill the buffer up-to size. Fill size with the data that was read.
    //!
    //! \param buffer: buffer to fill
    //! \param size: size of data to read
    //! \param wait: wait, unused
    //! \return status of the read
    //!
    Os::File::Status read(U8 *buffer, FwSignedSizeType &size, File::WaitType wait) override;

    //! \brief write data to the file
    //!
    //! Write to the synthetic file from the buffer of given size. Fill size with the data that was written.
    //!
    //! \param buffer: buffer to fill
    //! \param size: size of data to read
    //! \param bool: wait, unused
    //! \return status of the write
    //!
    Os::File::Status write(const U8 *buffer, FwSignedSizeType &size, File::WaitType wait) override;

    //! \brief seek pointer within file
    //!
    //! Seek the pointer within the file.
    //!
    //! \param offset: offset to seek to
    //! \param bool: absolute
    //! \return status of the seek
    //!
    Os::File::Status seek(const FwSignedSizeType offset, const File::SeekType absolute) override;

    //! \brief preallocate data within file
    //!
    //! Preallocate data within the file.
    //!
    //! \param offset: offset to start pre-allocation
    //! \param length: length of the pre-allocation
    //! \return status of the preallocate
    //!
    Os::File::Status preallocate(const FwSignedSizeType offset, const FwSignedSizeType length) override;

    //! \brief flush is no-op
    //!
    Os::File::Status flush() override;

    //! \brief pointer getter
    Os::File::Status position(FwSignedSizeType& position) override;

    //! \brief size getter
    Os::File::Status size(FwSignedSizeType& size) override;

    //! \brief silt data handle
    FileHandle* getHandle() override;

    std::shared_ptr<SyntheticFileData> m_data;

    static std::unique_ptr<SyntheticFileSystem> s_file_system;
};


//! \brief Synthetic file system implementation
//!
//! A synthetic standard template library based in-memory file system for use with testing. It is composed of a map of string paths to a
//! synthetic file data packet that tracks data and file pointer
//!
class SyntheticFileSystem {
  public:
    friend class SyntheticFile;
    //! \brief data returned by the open call
    //!
    struct OpenData {
        std::shared_ptr<SyntheticFileData> file;
        Os::File::Status status = Os::File::Status::OTHER_ERROR;
    };

    //! Constructor
    SyntheticFileSystem() = default;

    //! Destructor
    virtual ~SyntheticFileSystem() = default;

    //! \brief check a file exists
    //!
    //! Check if a file exists.
    //!
    //! \return: true if exists, false otherwise
    //!
    bool exists(const CHAR *path);

    //! \brief remove file
    void remove(const CHAR* path);

  private:
    //! \brief open a given path with mode and overwrite
    //!
    //! This opens a file at the given path. Mode drives the mode of the file and overwrite will overwrite files if it
    //! exists and was created. Returns a pair of status and synthetic file data.
    //!
    //! \param path: path to open
    //! \param mode: mode to open with
    //! \param overwrite: overwrite if exists
    //! \return (status, synthetic file object)
    //!
    OpenData open(const CHAR *path, const Os::File::Mode mode, const File::OverwriteType overwrite);
    //! Shadow file state: file system
    std::map<std::string, std::shared_ptr<SyntheticFileData>> m_filesystem;
};

} // namespace Test
} // namespace Os

#endif // OS_TEST_UT_FILE_SYNTHETIC_FILE_SYSTEM
