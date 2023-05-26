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
*   acknowledged. Any commercial use must be negotiated with the Office
*   of Technology Transfer at the California Institute of Technology.
*
*   Information included herein is controlled under the International
*   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
*   Export or transfer of this information to a Foreign Person or foreign
*   entity requires an export license issued by the U.S. State Department
*   or an ITAR exemption prior to the export or transfer.
*/

#ifndef PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_
#define PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_

#include <Svc/PassiveRateGroup/test/ut/GTestBase.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroupImpl.hpp>

namespace Svc {

    class PassiveRateGroupImplTester: public PassiveRateGroupGTestBase {
        public:
            PassiveRateGroupImplTester(Svc::PassiveRateGroupImpl& inst);
            virtual ~PassiveRateGroupImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void runNominal(NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts, NATIVE_INT_TYPE instance);

        private:

            void from_RateGroupMemberOut_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context);

            Svc::PassiveRateGroupImpl& m_impl;

            void clearPortCalls(void);

            struct {
                bool portCalled;
                NATIVE_UINT_TYPE contextVal;
                NATIVE_UINT_TYPE order;
            } m_callLog[Svc::PassiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];

            bool m_causeOverrun; //!< flag to cause an overrun during a rate group member port call
            NATIVE_UINT_TYPE m_callOrder; //!< tracks order of port call.

    };

} /* namespace Svc */

#endif /* PASSIVERATEGROUP_TEST_UT_PASSIVERATEGROUPIMPLTESTER_HPP_ */
