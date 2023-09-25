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
#include <Os/File.hpp>

namespace Svc {

    class PrmDbImplTester: public PrmDbGTestBase {
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

            // open call modifiers

            static bool OpenInterceptor(Os::File::Status &stat, const char* fileName, Os::File::Mode mode, void* ptr);
            Os::File::Status m_testOpenStatus;

            // read call modifiers

            static bool ReadInterceptor(Os::File::Status &stat, void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr);
            Os::File::Status m_testReadStatus;
            // How many read calls to let pass before modifying
            NATIVE_INT_TYPE m_readsToWait;
            // enumeration to tell what kind of error to inject
            typedef enum {
                FILE_READ_READ_ERROR, // return a bad read status
                FILE_READ_SIZE_ERROR, // return a bad size
                FILE_READ_DATA_ERROR  // return unexpected data
            } FileReadTestType;
            FileReadTestType m_readTestType;
            NATIVE_INT_TYPE m_readSize;
            BYTE m_readData[PRMDB_IMPL_TESTER_MAX_READ_BUFFER];

            // write call modifiers

            static bool WriteInterceptor(Os::File::Status &status, const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone, void* ptr);
            Os::File::Status m_testWriteStatus;
            // How many read calls to let pass before modifying
            NATIVE_INT_TYPE m_writesToWait;
            // enumeration to tell what kind of error to inject
            typedef enum {
                FILE_WRITE_WRITE_ERROR, // return a bad read status
                FILE_WRITE_SIZE_ERROR, // return a bad size
            } FileWriteTestType;
            FileWriteTestType m_writeTestType;
            NATIVE_INT_TYPE m_writeSize;

    };

} /* namespace SvcTest */

#endif /* PRMDB_TEST_UT_PRMDBIMPLTESTER_HPP_ */
