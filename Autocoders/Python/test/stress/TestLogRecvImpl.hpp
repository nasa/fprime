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
        TestLogRecvImpl(const char* compName);
        virtual ~TestLogRecvImpl();
        void init();
    protected:
        void logRecvPort_handler(FwIndexType portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity& severity, Fw::LogBuffer &args);
    private:
};

#endif /* TESTLOGRECVIMPL_HPP_ */
