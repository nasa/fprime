/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTLOGRECVIMPL_HPP_
#define TESTLOGRECVIMPL_HPP_

#include <Autocoders/Python/test/log_tester/TestTextLogImpl.hpp>

class TestLogRecvImpl: public LogTextImpl {
    public:
#if FW_OBJECT_NAMES == 1
        TestLogRecvImpl(const char* compName);
#else
        TestLogRecvImpl();
#endif
        virtual ~TestLogRecvImpl();
        void init(void);
    protected:
        void logRecvPort_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::LogSeverity severity, Fw::LogBuffer &args);
    private:
};

#endif /* TESTLOGRECVIMPL_HPP_ */
