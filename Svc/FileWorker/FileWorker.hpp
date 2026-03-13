// ======================================================================
// \title  FileWorker.hpp
// \author racheljt
// \brief  hpp file for FileWorker component implementation class
// ======================================================================

#ifndef Svc_FileWorker_HPP
#define Svc_FileWorker_HPP

#include "Fw/Types/BasicTypes.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Mutex.hpp"
#include "Svc/FileWorker/FileWorkerComponentAc.hpp"
#include "Svc/FileWorker/FileWorkerTypes.hpp"
#include "Svc/Ports/FilePorts/FppConstantsAc.hpp"
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

    void configure(U64 chunkSize, bool append);

  private:
    static constexpr U64 BLOCK_SIZE_BYTES = 4096;
    static constexpr U32 TIMEOUT_MS = 1000000;
    static constexpr U32 MAX_LOOP_ITERATIONS = 32;

    Svc::FileWorkerState m_state;
    Os::Mutex m_lock;
    bool m_abort;

    U64 m_chunkSize;
    bool m_append;

    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for cancelIn
    I8 cancelIn_handler(FwIndexType portNum  //!< The port number
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
                         U64 offsetBytes) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    Svc::FileWorkerStatus readBufferFromFile(Fw::Buffer& buffer, const char* const fileName);
    void readFile(Fw::Buffer& buffer, U64 length, Os::File& file, Fw::LogStringArg fileNameStr);
    Svc::FileWorkerReadStatus readFileBytes(Fw::Buffer& buffer, U64 length, Os::File& file, U64& bytesRead);

    bool getHash(const char* const hashFileName,
                 Utils::Hash& hash,
                 Utils::HashBuffer& hashBuffer,
                 const U8* const data,
                 const U64 size);
    bool writeBufferToFile(Fw::Buffer& buffer, const char* fileName, U64 offset);
    bool writeBufferHashToFile(Fw::Buffer& buffer, const char* fileName, U64 offset);
    U32 writeToFile(const U8* data, U64 length, Os::File& file, const char* fileName);
};

}  // namespace Svc

#endif
