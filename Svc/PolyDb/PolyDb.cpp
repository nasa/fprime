/*
 * PolyDbImpl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Timothy Canham
 */

#include <Svc/PolyDb/PolyDb.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>

namespace Svc {
    PolyDb::PolyDb(const char* name) : PolyDbComponentBase(name) {
        // initialize all entries to stale
        for (NATIVE_INT_TYPE entry = 0; entry < Svc::PolyDbCfg::PolyDbEntry::NUM_CONSTANTS; entry++) {
            this->m_db[entry].status = MeasurementStatus::STALE;
        }
    }

    void PolyDb::init(NATIVE_INT_TYPE instance) {
        PolyDbComponentBase::init(instance);
    }

    void PolyDb ::
        getValue_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::PolyDbCfg::PolyDbEntry& entry,
            Svc::MeasurementStatus& status,
            Fw::Time& time,
            Fw::PolyType& val)
    {
        FW_ASSERT(entry.isValid(),entry.e);
        status = this->m_db[entry.e].status;
        time = this->m_db[entry.e].time;
        val = this->m_db[entry.e].val;
    }

    void PolyDb ::
        setValue_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::PolyDbCfg::PolyDbEntry& entry,
            Svc::MeasurementStatus& status,
            Fw::Time& time,
            Fw::PolyType& val)
    {
        FW_ASSERT(entry.isValid(),entry.e);
        this->m_db[entry.e].status = status;
        this->m_db[entry.e].time = time;
        this->m_db[entry.e].val = val;
    }

    PolyDb::~PolyDb() {
    }

}
