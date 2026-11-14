/*
 * PrmDbTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef PRMDB_TEST_UT_PRMDBTESTER_HPP_
#define PRMDB_TEST_UT_PRMDBTESTER_HPP_

#include <Os/Stub/test/File.hpp>
#include <PrmDbGTestBase.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <config/PrmDbImplTesterCfg.hpp>

namespace Svc {

class PrmDbTester : public PrmDbGTestBase {
  public:
    PrmDbTester(Svc::PrmDbImpl& inst);
    virtual ~PrmDbTester();

    void runNominalPopulate();
    void runNominalSaveFile();
    void runNominalLoadFile();
    void runMissingExtraParams();
    void runFileReadError();
    void runFileWriteError();
    void runDbEqualTest();
    void runDbCopyTest();
    void runDbCommitTest();
    void runPrmFileLoadNominal();
    void runPrmFileLoadWithErrors();
    void runPrmFileLoadIllegal();
    void runPrmFileLoadEmptyFileName();
    void runPrmFileLoadSandboxViolation();
    void runShorterSaveDoesNotCorrupt();

    void runRefPrmFile();

    void runPrmRequestDpNominal();
    void runPrmRequestDpInvalidRange();
    void runPrmRequestDpEmptyRange();
    void runPrmRequestDpBufferFailure();
    void runPrmRequestDpChunking();
    void runPrmRequestDpRecordTooLarge();
    void runPrmRequestDpMidStreamBufferFailure();
    void runPrmRequestDpNotConfigured();

  private:
    bool dbEqual();

    //! Override productGet_handler to control allocation behavior and buffer backing
    Fw::Success::T productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer) override;

    //! Override productSend_handler to decode each sent container's records immediately,
    //! since the backing buffer memory is reused by subsequent dpGet calls
    void productSend_handler(FwDpIdType id, const Fw::Buffer& buffer) override;

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
    );
    Svc::PrmDbImpl& m_impl;
    void resetEvents();

    // enumeration to tell what kind of error to inject
    enum ErrorType {
        FILE_STATUS_ERROR,   // return a bad read status
        FILE_SIZE_ERROR,     // return a bad size
        FILE_DATA_ERROR,     // return unexpected data
        FILE_READ_NO_ERROR,  // No error
    };
    Os::File::Status m_status;
    FwSizeType m_waits = 0;
    ErrorType m_errorType = FILE_READ_NO_ERROR;

    BYTE m_io_data[PRMDB_IMPL_TESTER_MAX_READ_BUFFER];

    // write call modifiers

    Os::File::Status WriteInterceptor();
    Os::File::Status m_testWriteStatus;

    //! Backing storage for the data product buffer returned by productGet_handler. Reused for
    //! every dpGet call, so productSend_handler must decode a sent container's records before
    //! the next dpGet call overwrites this memory.
    U8 m_dpBuffer[FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwPrmIdType) + 2 * sizeof(FwDpIdType)] = {};

    //! Size of the buffer passed to configureDp() in the current test; productGet_handler
    //! validates that every requested dataSize fits within this buffer once header/hash
    //! overhead is subtracted
    FwSizeType m_dpBufferSize = sizeof(m_dpBuffer);

    //! Set by tests to force every productGet_handler call to report an allocation failure
    bool m_dpGetShouldFail = false;

    //! If nonzero, the Nth productGet_handler call (1-indexed) reports an allocation failure;
    //! all other calls succeed. Reset to 0 at the start of each run* test.
    U32 m_dpGetFailOnCall = 0;

    //! Count of productGet_handler calls made so far in the current test
    U32 m_dpGetCallCount = 0;

    //! One decoded data product record, captured by productSend_handler
    struct DecodedRecord {
        FwPrmIdType id;
        Fw::ParamBuffer value;
    };

    //! Records decoded from sent containers, in the order they were sent
    DecodedRecord m_decodedRecords[PRMDB_NUM_DB_ENTRIES];

    //! Number of valid entries in m_decodedRecords
    FwSizeType m_numDecodedRecords = 0;

  public:
    class PrmDbTestFile : public Os::Stub::File::Test::TestFile {
      public:
        Status read(U8* buffer, FwSizeType& size, WaitType wait) override;

        Status write(const U8* buffer, FwSizeType& size, WaitType wait) override;

        // Tracks the current tester
        static void setTester(PrmDbTester* tester);
        static PrmDbTester* s_tester;
    };

    //! Expose PrmDbImpl::computeCrc for hardcoded CRC value testing
};

}  // namespace Svc

#endif /* PRMDB_TEST_UT_PRMDBTESTER_HPP_ */
