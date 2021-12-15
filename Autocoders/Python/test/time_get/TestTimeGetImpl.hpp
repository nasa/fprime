/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTTEXTLOGIMPL_HPP_
#define TESTTEXTLOGIMPL_HPP_

#include <Autocoders/Python/test/time_get/TimeGetComponentAc.hpp>

class TimeGetTesterImpl: public TimeGet::TimeGetTesterComponentBase {
    public:
        TimeGetTesterImpl(const char* compName);
        virtual ~TimeGetTesterImpl();
        void init();
    protected:
        void test_time_get_handler();
};

#endif /* TESTTIMEIMPL_HPP_ */
