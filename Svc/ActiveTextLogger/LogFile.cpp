// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Svc/ActiveTextLogger/LogFile.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <limits>
#include <cstring>
#include <cstdio>
#include <Fw/Types/StringUtils.hpp>


namespace Svc {

    // ----------------------------------------------------------------------
    // Initialization/Exiting
    // ----------------------------------------------------------------------

    LogFile::LogFile() :
        m_fileName(), m_file(), m_maxFileSize(0), m_openFile(false), m_currentFileSize(0)
    {

    }

    LogFile::~LogFile()
    {
        // Close the file if needed:
        if (this->m_openFile) {
            this->m_file.close();
        }
    }

    // ----------------------------------------------------------------------
    // Member Functions
    // ----------------------------------------------------------------------

    bool LogFile::write_to_log(const char *const buf, const U32 size)
    {

        FW_ASSERT(buf != nullptr);

        bool status = true;

        // Print to file if there is one, and given a valid size:
        if (this->m_openFile && size > 0) {

            // Make sure we won't exceed the maximum size:
            // Note: second condition in if statement is true if there is overflow
            // in the addition below
            U32 projectedSize = this->m_currentFileSize + size;
            if ( projectedSize > this->m_maxFileSize ||
                (this->m_currentFileSize > (std::numeric_limits<U32>::max() - size)) ) {

                status = false;
                this->m_openFile = false;
                this->m_file.close();
            }
            // Won't exceed max size, so write to file:
            else {

                FwSignedSizeType writeSize = size;
                Os::File::Status stat = this->m_file.write(reinterpret_cast<const U8*>(buf),writeSize,Os::File::WAIT);

                // Assert that we are not trying to write to a file we never opened:
                FW_ASSERT(stat != Os::File::NOT_OPENED);

                // Only return a good status if the write was valid
                status = (writeSize > 0);

                this->m_currentFileSize += static_cast<U32>(writeSize);
            }
        }

        return status;
    }


    bool LogFile::set_log_file(const char* fileName, const U32 maxSize, const U32 maxBackups)
    {
        FW_ASSERT(fileName != nullptr);

        // If there is already a previously open file then close it:
        if (this->m_openFile) {
            this->m_openFile = false;
            this->m_file.close();
        }
        Fw::FileNameString searchFilename;
        Fw::FormatStatus formatStatus = searchFilename.format("%s", fileName);

        // If file name is too large, return failure:
        if (formatStatus != Fw::FormatStatus::SUCCESS) {
            return false;
        }

        // Check if file already exists, and if it does try to tack on a suffix.
        // Quit after maxBackups suffix addition tries (first try is w/ the original name).
        U32 suffix = 0;
        bool failedSuffix = false;
        FwSignedSizeType fileSize = 0;
        while (Os::FileSystem::getFileSize(searchFilename.toChar(), fileSize) == Os::FileSystem::OP_OK) {
            // Not able to create a new non-existing file in maxBackups tries, then mark that it failed:
            if (suffix >= maxBackups) {
                failedSuffix = true;
                break;
            }
            // Format and check for error and overflows
            formatStatus = searchFilename.format("%s%" PRIu32, fileName, suffix);
            if (formatStatus != Fw::FormatStatus::SUCCESS) {
                return false;
            }
            ++suffix;
        }

        // If failed trying to make a new file, just use the original file
        if (failedSuffix) {
            searchFilename = fileName;
        }

        // Open the file (using CREATE so that it truncates an already existing file):
        Os::File::Status stat = this->m_file.open(searchFilename.toChar(), Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE);

        // Bad status when trying to open the file:
        if (stat != Os::File::OP_OK) {
            return false;
        }

        this->m_currentFileSize = 0;
        this->m_maxFileSize = maxSize;
        this->m_fileName = searchFilename;
        this->m_openFile = true;

        return true;
    }


} // namespace Svc
