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

#ifndef ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPIMPLTESTER_HPP_
#define ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPIMPLTESTER_HPP_

#include <ActiveRateGroupGTestBase.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroup.hpp>

namespace Svc {

    class ActiveRateGroupImplTester: public ActiveRateGroupGTestBase {
        public:
            ActiveRateGroupImplTester(Svc::ActiveRateGroup& inst);
            virtual ~ActiveRateGroupImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void runNominal(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts, NATIVE_INT_TYPE instance);
            void runCycleOverrun(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts, NATIVE_INT_TYPE instance);
            void runPingTest();

        private:

            void from_RateGroupMemberOut_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context);

            //! Handler for from_PingOut
            //!
            void from_PingOut_handler(
              const NATIVE_INT_TYPE portNum, /*!< The port number*/
              U32 key /*!< Value to return to pinger*/
            );

            Svc::ActiveRateGroup& m_impl;

            void clearPortCalls();

            struct {
                bool portCalled;
                NATIVE_UINT_TYPE contextVal;
                NATIVE_UINT_TYPE order;
            } m_callLog[Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];

            bool m_causeOverrun; //!< flag to cause an overrun during a rate group member port call
            NATIVE_UINT_TYPE m_callOrder; //!< tracks order of port call.

    };

} /* namespace Svc */

#endif /* ACTIVERATEGROUP_TEST_UT_ACTIVERATEGROUPIMPLTESTER_HPP_ */
