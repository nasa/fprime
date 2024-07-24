// ======================================================================
// \title  TraceLogger.cpp
// \author sreddy
// \brief  cpp file for TraceLogger component implementation class
// ======================================================================

#include "Svc/TraceLogger/TraceLogger.hpp"
#include "FpConfig.hpp"
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <limits>
#include <cstring>
#include <cstdio>
#include <Fw/Types/StringUtils.hpp>

namespace Svc {

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    TraceLogger :: TraceLogger(const char* const compName) : 
    TraceLoggerComponentBase(compName),
    m_log_file(),
    m_mode(CLOSED),
    m_fileName(),
    m_maxFileSize(0),
    m_byteCount(0),
    m_log_init(false) 
    {
    }

    TraceLogger ::~TraceLogger() {}

    void TraceLogger :: 
        init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance)
    {
            TraceLoggerComponentBase::init(queueDepth,instance);
    }

    bool TraceLogger :: set_log_file(const char* fileName, const U32 maxSize) {
        FW_ASSERT(fileName != nullptr);
        //If a file is already open then close it
        if(this->m_mode == OPEN) {
            this->m_mode = CLOSED;
            this->m_log_file.close();
        }

        //If file name is too large then return failure
        U32 fileNameSize = Fw::StringUtils::string_length(fileName, Fw::String::STRING_SIZE);
        if (fileNameSize == Fw::String::STRING_SIZE) {
            return false;
        }
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
            Os::File::Status stat = this->m_file.open(fileNameFinal, Os::File::OPEN_CREATE, Os::File::OverwriteType::NO_OVERWRITE);

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
    }
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void TraceLogger ::TraceBufferLogger_handler(FwIndexType portNum,
                                                FwTraceIdType id,
                                                Fw::Time& timeTag,
                                                const Fw::TraceType& type,
                                                Fw::TraceBuffer& args) {
        // TODO
    }

    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    void TraceLogger ::EnableTrace_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void TraceLogger ::DumpTraceDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

}  // namespace Svc
