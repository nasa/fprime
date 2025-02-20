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
        TestLogRecvImpl(const char* compName);
        virtual ~TestLogRecvImpl();
        void init();
    protected:
        void logRecvPort_handler(FwIndexType portNum, FwEventIdType id, Fw::Time &timeTag, Fw::LogSeverity severity, Fw::LogBuffer &args);
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
