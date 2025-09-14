/*
 * \author Tim Canham
 * \file
 * \brief
 *
 * This file is the test component header for the active rate group unit test.
 *
 * Code Generated Source Code Header
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 *
 */

#ifndef ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPTESTER_HPP_
#define ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPTESTER_HPP_

#include <ActiveRateGroupGTestBase.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroup.hpp>

namespace Svc {

class ActiveRateGroupTester : public ActiveRateGroupGTestBase {
  public:
    ActiveRateGroupTester(Svc::ActiveRateGroup& inst);
    virtual ~ActiveRateGroupTester();

    void runNominal(U32 contexts[], FwIndexType numContexts, FwEnumStoreType instance);
    void runCycleOverrun(U32 contexts[], FwIndexType numContexts, FwEnumStoreType instance);
    void runPingTest();

  private:
    void from_RateGroupMemberOut_handler(FwIndexType portNum, U32 context);

    //! Handler for from_PingOut
    //!
    void from_PingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
    );

    Svc::ActiveRateGroup& m_impl;

    void clearPortCalls();

    struct {
        bool portCalled;
        U32 contextVal;
        FwIndexType order;
    } m_callLog[Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];

    bool m_causeOverrun;      //!< flag to cause an overrun during a rate group member port call
    FwIndexType m_callOrder;  //!< tracks order of port call.
};

} /* namespace Svc */

#endif /* ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPTESTER_HPP_ */
