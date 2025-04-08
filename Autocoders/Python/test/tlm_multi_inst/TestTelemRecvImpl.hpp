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
        TestTelemRecvImpl(const char* compName);
        virtual ~TestTelemRecvImpl();
        void init();
    protected:
        void tlmRecvPort_handler(FwIndexType portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val);
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
