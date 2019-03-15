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
#if FW_OBJECT_NAMES == 1
        TestTimeImpl(const char* compName);
#else
        TestTimeImpl();
#endif
        virtual ~TestTimeImpl();
        void init(void);
        void setTime(Fw::Time time);
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
        Fw::Time m_time;
};

#endif /* TESTTIMEIMPL_HPP_ */
