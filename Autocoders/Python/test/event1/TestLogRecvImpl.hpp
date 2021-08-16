/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTTELEMRECVIMPL_HPP_
#define TESTTELEMRECVIMPL_HPP_

#include <Autocoders/Python/test/log_tester/TestTextLogImpl.hpp>

class TestLogRecvImpl: public LogTextImpl {
    public:
#if FW_OBJECT_NAMES == 1
        TestLogRecvImpl(const char* compName);
#else
        TestLogRecvImpl();
#endif
        virtual ~TestLogRecvImpl();
        void init();
    protected:
        void logRecvPort_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::LogSeverity severity, Fw::LogBuffer &args);
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
