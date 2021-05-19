/*
 * TlmStoreImplTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_
#define TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_

#include <GTestBase.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>

namespace Svc {

    class TlmChanImplTester: public TlmChanGTestBase {
        public:
            TlmChanImplTester(Svc::TlmChanImpl& inst);
            virtual ~TlmChanImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void runNominalChannel(void);
            void runMultiChannel(void);
            void runOffNominal(void);
            void runTooManyChannels(void);

        private:
            Svc::TlmChanImpl& m_impl;

            void from_PktSend_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context);

            // helper
            void sendBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance);
            bool doRun(bool check);
            void checkBuff(FwChanIdType id, U32 val, NATIVE_INT_TYPE instance);

            // Keep a history
            NATIVE_UINT_TYPE m_numBuffs;
            Fw::ComBuffer m_rcvdBuffer[TLMCHAN_HASH_BUCKETS];
            bool m_bufferRecv;
            void clearBuffs(void);

            // dump functions
            void dumpHash(void);
            static void dumpTlmEntry(TlmChanImpl::TlmEntry* entry);
            //! Handler for from_pingOut
            //!
            void from_pingOut_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                U32 key /*!< Value to return to pinger*/
            );

    };

} /* namespace Svc */

#endif /* TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_ */
