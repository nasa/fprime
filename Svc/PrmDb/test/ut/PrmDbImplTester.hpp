/*
 * PrmDbImplTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef PRMDB_TEST_UT_PRMDBIMPLTESTER_HPP_
#define PRMDB_TEST_UT_PRMDBIMPLTESTER_HPP_

#include <PrmDbGTestBase.hpp>
#include <PrmDbImplTesterCfg.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Os/Stub/test/File.hpp>

namespace Svc {

    class PrmDbImplTester : public PrmDbGTestBase {
        public:
            PrmDbImplTester(Svc::PrmDbImpl& inst);
            virtual ~PrmDbImplTester();

            void runNominalPopulate();
            void runNominalSaveFile();
            void runNominalLoadFile();
            void runMissingExtraParams();
            void runFileReadError();
            void runFileWriteError();

            void runRefPrmFile();

            void init(NATIVE_INT_TYPE instance = 0);

        private:
            //! Handler for from_pingOut
            //!
            void from_pingOut_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                U32 key /*!< Value to return to pinger*/
            );
            Svc::PrmDbImpl& m_impl;
            void resetEvents();



            // enumeration to tell what kind of error to inject
            enum ErrorType {
                FILE_STATUS_ERROR, // return a bad read status
                FILE_SIZE_ERROR, // return a bad size
                FILE_DATA_ERROR,  // return unexpected data
                FILE_READ_NO_ERROR,    // No error
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
            Status read(U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

            Status write(const U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

            // Tracks the current tester
            static void setTester(PrmDbImplTester* tester);
            static PrmDbImplTester* s_tester;

        };

    };

} /* namespace SvcTest */

#endif /* PRMDB_TEST_UT_PRMDBIMPLTESTER_HPP_ */
