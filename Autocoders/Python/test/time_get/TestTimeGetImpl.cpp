/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <stdio.h>
#include "TestTimeGetImpl.hpp"

TimeGetTesterImpl::TimeGetTesterImpl(const char* name) : TimeGet::TimeGetTesterComponentBase(name)
{
}

TimeGetTesterImpl::~TimeGetTesterImpl() {
}

void TimeGetTesterImpl::init(void) {
    TimeGet::TimeGetTesterComponentBase::init();
}

void TimeGetTesterImpl::test_time_get_handler() {
    this->getTime();  // Tests independent getTime() function
}


