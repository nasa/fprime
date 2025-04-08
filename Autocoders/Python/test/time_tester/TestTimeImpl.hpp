/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTTIMEIMPL_HPP_
#define TESTTIMEIMPL_HPP_

#include <Autocoders/Python/test/time_tester/TimeTestComponentAc.hpp>

class TestTimeImpl: public Time::TimeTesterComponentBase {
    public:
        TestTimeImpl(const char* compName);
        virtual ~TestTimeImpl();
        void init();
        void setTime(Fw::Time time);
    protected:
        void timeGetPort_handler(
                FwIndexType portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
        Fw::Time m_time;
};

#endif /* TESTTIMEIMPL_HPP_ */
