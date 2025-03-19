/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTTEXTLOGIMPL_HPP_
#define TESTTEXTLOGIMPL_HPP_

#include <Autocoders/Python/test/log_tester/LogTestComponentAc.hpp>

class LogTextImpl: public Log::LogTesterComponentBase {
    public:
        LogTextImpl(const char* compName);
        virtual ~LogTextImpl();
        void init();
        void setTime(Fw::Time time);
    protected:
        void textLogRecvPort_handler(FwIndexType portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity &severity, Fw::TextLogString &text);
    private:
};

#endif /* TESTTIMEIMPL_HPP_ */
