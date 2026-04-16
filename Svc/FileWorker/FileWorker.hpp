// ======================================================================
// \title  FileWorker.hpp
// \author racheljt
// \brief  hpp file for FileWorker component implementation class
// ======================================================================

#ifndef Svc_FileWorker_HPP
#define Svc_FileWorker_HPP

#include <atomic>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Mutex.hpp"
#include "Svc/FileWorker/FileWorkerComponentAc.hpp"
#include "Svc/FileWorker/FileWorkerTypes.hpp"
#include "Utils/CRCChecker.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

class FileWorker : public FileWorkerComponentBase {
    friend class FileWorkerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FileWorker object
    FileWorker(const char* const compName  //!< The component name
    );

    //! Destroy FileWorker object
    ~FileWorker();

    void configure(U64 chunkSize);

  private:
    static constexpr U64 BLOCK_SIZE_BYTES = 4096;
    static constexpr U32 TIMEOUT_MS = 1000000;
    static constexpr U32 MAX_LOOP_ITERATIONS = 32;

    Svc::FileWorkerState m_state;
    std::atomic<bool> m_abort;
    U64 m_chunkSize;

    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for cancelIn
    void cancelIn_handler(FwIndexType portNum  //!< The port number
                          ) override;

    //! Handler implementation for readIn
    void readIn_handler(FwIndexType portNum,  //!< The port number
                        const Fw::StringBase& path,
                        Fw::Buffer& buffer) override;

    //! Handler implementation for verifyIn
    void verifyIn_handler(FwIndexType portNum,  //!< The port number
                          const Fw::StringBase& path,
                          U32 crc) override;

    //! Handler implementation for writeIn
    void writeIn_handler(FwIndexType portNum,  //!< The port number
                         const Fw::StringBase& path,
                         Fw::Buffer& buffer,
                         FwSizeType offsetBytes,
                         bool append) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    Svc::FileWorkerStatus readBufferFromFile(Fw::Buffer& buffer, const char* const fileName);
    void readFile(Fw::Buffer& buffer, FwSizeType size, Os::File& file, const Fw::LogStringArg& fileNameStr);
    Svc::FileWorkerReadStatus readFileBytes(Fw::Buffer& buffer, FwSizeType size, Os::File& file, FwSizeType& bytesRead);

    bool getHash(const char* const hashFileName,
                 Utils::Hash& hash,
                 Utils::HashBuffer& hashBuffer,
                 const U8* const data,
                 const FwSizeType size);
    bool writeBufferToFile(Fw::Buffer& buffer, const char* fileName, FwSizeType offset, bool append);
    void writeBufferHashToFile(Fw::Buffer& buffer, const char* fileName, FwSizeType offset, bool append);
    FwSizeType writeToFile(const U8* data, FwSizeType size, Os::File& file, const char* fileName);
};

}  // namespace Svc

#endif
