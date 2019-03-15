/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTTELEMRECVIMPL_HPP_
#define TESTTELEMRECVIMPL_HPP_

#include <Autocoders/Python/test/telem_tester/TelemTestComponentAc.hpp>

class TestTelemRecvImpl: public Tlm::TelemTesterComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        TestTelemRecvImpl(const char* compName);
#else
        TestTelemRecvImpl();
#endif
        virtual ~TestTelemRecvImpl();
        void init(void);
    protected:
        void tlmRecvPort_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val);
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
