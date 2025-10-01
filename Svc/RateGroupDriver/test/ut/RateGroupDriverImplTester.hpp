/*
 * RateGroupDriverImplTester.hpp
 *
 *  Created on: June 19, 2015
 *      Author: tcanham
 */

#ifndef RATEGROUPDRIVER_TEST_UT_RATEGROUPDRIVERIMPLTESTER_HPP_
#define RATEGROUPDRIVER_TEST_UT_RATEGROUPDRIVERIMPLTESTER_HPP_

#include <RateGroupDriverGTestBase.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>

namespace Svc {

class RateGroupDriverImplTester : public RateGroupDriverGTestBase {
  public:
    RateGroupDriverImplTester(Svc::RateGroupDriver& inst);
    virtual ~RateGroupDriverImplTester();

    void runSchedNominal(Svc::RateGroupDriver::DividerSet dividersSet, FwIndexType numDividers);

  private:
    void from_CycleOut_handler(FwIndexType portNum, Os::RawTime& cycleStart);

    Svc::RateGroupDriver& m_impl;

    void clearPortCalls();

    bool m_portCalls[Svc::RateGroupDriver::DIVIDER_SIZE];
};

} /* namespace Svc */

#endif
