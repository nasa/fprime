/*
 * TelemStoreImplTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_
#define TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_

#include <Svc/TlmChan/test/ut/TelemStoreComponentTestAc.hpp>
#include <Svc/TlmChan/TelemChanImpl.hpp>

namespace Svc {

    // access to base class data
    class TelemStoreComponentBaseFriend {
        public:
            TelemStoreComponentBaseFriend(Svc::TelemStoreComponentBase& inst);
            Fw::QueuedComponentBase::MsgDispatchStatus doDispatch();
        private:
            Svc::TelemStoreComponentBase& m_baseInst;
    };

    class TelemChanImplTester: public Svc::TelemStoreComponentTesterBase {
        public:
            TelemChanImplTester(Svc::TelemChanImpl& inst);
            virtual ~TelemChanImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void doPerfTest(U32 iterations);

        private:
            Svc::TelemChanImpl& m_impl;
            TelemStoreComponentBaseFriend m_baseFriend;

            void pktSend_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data);


    };

} /* namespace Svc */

#endif /* TLMCHAN_TEST_UT_TLMCHANIMPLTESTER_HPP_ */
