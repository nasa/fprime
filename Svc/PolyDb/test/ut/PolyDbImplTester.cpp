/*
 * PolyDbImplTester.cpp
 *
 *  Created on: Sept 14, 2015
 *      Author: tcanham
 */

#include <Svc/PolyDb/test/ut/PolyDbImplTester.hpp>
#include <cstdio>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

namespace Svc {

    void PolyDbImplTester::init(NATIVE_INT_TYPE instance) {
        Svc::PolyDbTesterComponentBase::init();
    }

    PolyDbImplTester::PolyDbImplTester(Svc::PolyDbImpl& inst) :
        Svc::PolyDbTesterComponentBase("testerbase") {
    }

    PolyDbImplTester::~PolyDbImplTester() {
    }

    void PolyDbImplTester::runNominalReadWrite() {

        enum {
            NUM_TEST_VALS = 12 // one for each type supported
        };

        Fw::PolyType vals[NUM_TEST_VALS];

        // set test values
        vals[0] = static_cast<U8>(1);
        vals[1] = static_cast<I8>(2);
        vals[2] = static_cast<U16>(3);
        vals[3] = static_cast<I16>(4);
        vals[4] = static_cast<U32>(5);
        vals[5] = static_cast<I32>(6);
        vals[6] = static_cast<U64>(7);
        vals[7] = static_cast<I64>(8);
        vals[8] = static_cast<F32>(9.0);
        vals[9] = static_cast<F64>(10.0);
        vals[10] = true;
        vals[11] = reinterpret_cast<void*>(0x100);

        Fw::Time ts(TB_NONE,6,7);

        bool r001Emitted = false;
        bool r002Emitted = false;
        bool r003Emitted = false;
        bool r004Emitted = false;

        // read and write normal values for each entry in the database with each status type
        for (MeasurementStatus::t mstatValue = MeasurementStatus::OK; mstatValue <= MeasurementStatus::STALE;) {
            MeasurementStatus mstat(mstatValue);
            for (U32 entry = 0; entry < POLYDB_NUM_DB_ENTRIES; entry++) {
                if (not r001Emitted) {
                    REQUIREMENT("PDB-001");
                    r001Emitted = true;
                }
                this->setValue_out(0,entry,mstat,ts,vals[entry%NUM_TEST_VALS]);
                Fw::PolyType check;
                if (not r003Emitted) {
                    REQUIREMENT("PDB-003");
                    r003Emitted = true;
                }
                Fw::Time checkTs;
                MeasurementStatus checkStat;
                this->getValue_out(0,entry,checkStat,checkTs,check);
                if (not r002Emitted) {
                    REQUIREMENT("PDB-002");
                    r002Emitted = true;
                }
                if (vals[entry%NUM_TEST_VALS].isF32() or vals[entry%NUM_TEST_VALS].isF64()) {
                    ASSERT_NE(check,vals[entry%NUM_TEST_VALS]);
                } else {
                    ASSERT_EQ(check,vals[entry%NUM_TEST_VALS]);
                }
                ASSERT_EQ(checkTs,ts);
                if (not r004Emitted) {
                    REQUIREMENT("PDB-004");
                    r004Emitted = true;
                }
                ASSERT_EQ(mstatValue,checkStat.e);
            }
            mstatValue = static_cast<MeasurementStatus::t>(mstatValue + 1);
        }

    }


} /* namespace Svc */
