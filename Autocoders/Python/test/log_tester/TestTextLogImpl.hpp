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
#if FW_OBJECT_NAMES == 1
        LogTextImpl(const char* compName);
#else
        LogTextImpl();
#endif
        virtual ~LogTextImpl();
        void init(void);
        void setTime(Fw::Time time);
    protected:
        void textLogRecvPort_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text);
    private:
};

#endif /* TESTTIMEIMPL_HPP_ */
