/*
 * PolyDbImplTester.hpp
 *
 *  Created on: Sept 14, 2015
 *      Author: tcanham
 */

#ifndef POLYDB_TEST_UT_TLMCHANIMPLTESTER_HPP_
#define POLYDB_TEST_UT_TLMCHANIMPLTESTER_HPP_

#include <Svc/PolyDb/test/ut/PolyDbComponentTestAc.hpp>
#include <Svc/PolyDb/PolyDbImpl.hpp>

namespace Svc {

    class PolyDbImplTester: public Svc::PolyDbTesterComponentBase {
        public:
            PolyDbImplTester(Svc::PolyDbImpl& inst);
            virtual ~PolyDbImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void runNominalReadWrite();
    };

} /* namespace Svc */

#endif /* POLYDB_TEST_UT_TLMCHANIMPLTESTER_HPP_ */
