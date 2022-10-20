/*
 * PolyDbImpl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Timothy Canham
 */

#include <Svc/PolyDb/PolyDbImpl.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>

namespace Svc {
    PolyDbImpl::PolyDbImpl(const char* name) : PolyDbComponentBase(name) {
        // initialize all entries to stale
        for (NATIVE_INT_TYPE entry = 0; entry < POLYDB_NUM_DB_ENTRIES; entry++) {
            this->m_db[entry].status = MeasurementStatus::STALE;
        }
    }

    void PolyDbImpl::init(NATIVE_INT_TYPE instance) {
        PolyDbComponentBase::init(instance);
    }

    // If ports are no longer guarded, these accesses need to be protected from each other
    // If there are a lot of accesses, perhaps an interrupt lock could be used instead of guarded ports

    void PolyDbImpl::getValue_handler(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {
        FW_ASSERT(entry < POLYDB_NUM_DB_ENTRIES,entry);
        status = this->m_db[entry].status;
        time = this->m_db[entry].time;
        val = this->m_db[entry].val;
    }

    void PolyDbImpl::setValue_handler(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val) {
        FW_ASSERT(entry < POLYDB_NUM_DB_ENTRIES,entry);
        this->m_db[entry].status = status;
        this->m_db[entry].time = time;
        this->m_db[entry].val = val;
    }

    PolyDbImpl::~PolyDbImpl() {
    }

}
