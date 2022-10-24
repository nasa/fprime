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

                NATIVE_INT_TYPE writeSize = static_cast<NATIVE_INT_TYPE>(size);
                Os::File::Status stat = this->m_file.write(buf,writeSize,true);

                // Assert that we are not trying to write to a file we never opened:
                FW_ASSERT(stat != Os::File::NOT_OPENED);

                // Only return a good status if the write was valid
                status = (writeSize > 0);

                this->m_currentFileSize += writeSize;
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

        // If file name is too large, return failure:
        U32 fileNameSize = Fw::StringUtils::string_length(fileName, Fw::String::STRING_SIZE);
        if (fileNameSize == Fw::String::STRING_SIZE) {
            return false;
        }

        U32 suffix = 0;
        U64 tmp;
        char fileNameFinal[Fw::String::STRING_SIZE];
        (void) strncpy(fileNameFinal,fileName,
                       Fw::String::STRING_SIZE);
        fileNameFinal[Fw::String::STRING_SIZE-1] = 0;

        // Check if file already exists, and if it does try to tack on a suffix.
        // Quit after 10 suffix addition tries (first try is w/ the original name).
        bool failedSuffix = false;
        while (Os::FileSystem::getFileSize(fileNameFinal,tmp) == Os::FileSystem::OP_OK) {

            // If the file name was the max size, then can't append a suffix,
            // so just fail:
            if (fileNameSize == (Fw::String::STRING_SIZE-1)) {
                return false;
            }

            // Not able to create a new non-existing file in maxBackups tries, then mark that it failed:
            if (suffix >= maxBackups) {
                failedSuffix = true;
                break;
            }

            NATIVE_INT_TYPE stat = snprintf(fileNameFinal,Fw::String::STRING_SIZE,
                                            "%s%" PRIu32,fileName,suffix);

            // If there was error, then just fail:
            if (stat <= 0) {
                return false;
            }

            // There should never be truncation:
            FW_ASSERT(stat < Fw::String::STRING_SIZE);

            ++suffix;
        }

        // If failed trying to make a new file, just use the original file
        if (failedSuffix) {
            (void) strncpy(fileNameFinal,fileName,
                           Fw::String::STRING_SIZE);
            fileNameFinal[Fw::String::STRING_SIZE-1] = 0;
        }

        // Open the file (using CREATE so that it truncates an already existing file):
        Os::File::Status stat = this->m_file.open(fileNameFinal, Os::File::OPEN_CREATE, false);

        // Bad status when trying to open the file:
        if (stat != Os::File::OP_OK) {
            return false;
        }

        this->m_currentFileSize = 0;
        this->m_maxFileSize = maxSize;
        this->m_fileName = fileNameFinal;
        this->m_openFile = true;

        return true;
    }


} // namespace Svc
