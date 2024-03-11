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
        for (NATIVE_INT_TYPE entry = 0; entry < Svc::PolyDbCfg::PolyDbEntry::NUM_CONSTANTS; entry++) {
            this->m_db[entry].status = MeasurementStatus::STALE;
        }
    }

    void PolyDbImpl::init(NATIVE_INT_TYPE instance) {
        PolyDbComponentBase::init(instance);
    }

    // If ports are no longer guarded, these accesses need to be protected from each other
    // If there are a lot of accesses, perhaps an interrupt lock could be used instead of guarded ports

    void PolyDbImpl ::
        getValue_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::PolyDbCfg::PolyDbEntry& entry,
            Svc::MeasurementStatus& status,
            Fw::Time& time,
            Fw::PolyType& val)
    {
        FW_ASSERT(entry < Svc::PolyDbCfg::PolyDbEntry::NUM_CONSTANTS, entry);
        status = this->m_db[entry.e].status;
        time = this->m_db[entry.e].time;
        val = this->m_db[entry.e].val;
    }

    void PolyDbImpl ::
        setValue_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::PolyDbCfg::PolyDbEntry& entry,
            Svc::MeasurementStatus& status,
            Fw::Time& time,
            Fw::PolyType& val)
    {
        FW_ASSERT(entry < Svc::PolyDbCfg::PolyDbEntry::NUM_CONSTANTS, entry);
        this->m_db[entry.e].status = status;
        this->m_db[entry.e].time = time;
        this->m_db[entry.e].val = val;
    }

    PolyDbImpl::~PolyDbImpl() {
    }

}
