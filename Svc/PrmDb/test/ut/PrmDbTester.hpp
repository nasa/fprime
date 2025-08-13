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

    void runRefPrmFile();

  private:
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

  public:
    class PrmDbTestFile : public Os::Stub::File::Test::TestFile {
      public:
        Status read(U8* buffer, FwSizeType& size, WaitType wait) override;

        Status write(const U8* buffer, FwSizeType& size, WaitType wait) override;

        // Tracks the current tester
        static void setTester(PrmDbTester* tester);
        static PrmDbTester* s_tester;
    };
};

}  // namespace Svc

#endif /* PRMDB_TEST_UT_PRMDBTESTER_HPP_ */
