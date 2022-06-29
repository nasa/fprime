/*
 * ComponentTesterImpl.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: tcanham
 */

#include "ComponentTesterImpl.hpp"

namespace SvcTest {

    ComponentTesterImpl::ComponentTesterImpl(const char* compName) : TestTesterComponentBase(compName) {

    }

    ComponentTesterImpl::~ComponentTesterImpl() {
    }

    void ComponentTesterImpl::init() {
        TestTesterComponentBase::init();
    }

    void ComponentTesterImpl::runTest() {
        printf("Invoking aport2\n");
        this->aport2_out(0, 1, 2.3, true);
        printf("Invoking aport\n");
        this->aport_out(0,4, 5.0, 6);
        printf("Invoking aport again.\n");
        this->aport_out(0,7, 8.9, 10);

    }

} /* namespace SvcTest */
