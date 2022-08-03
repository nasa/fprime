/**
 * \file
 * \author T. Canham
 * \brief Implementation for telemetry channel component thread.
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>

#include <cstdio>

namespace Svc {

    void TlmChanImpl::Run_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
        // Only write packets if connected
        if (not this->isConnected_PktSend_OutputPort(0)) {
            return;
        }

        // lock mutex long enough to modify active telemetry buffer
        // so the data can be read without worrying about updates
        this->lock();
        this->m_activeBuffer = 1 - this->m_activeBuffer;
        // set activeBuffer to not updated
        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            this->m_tlmEntries[this->m_activeBuffer].buckets[entry].updated = false;
        }
        this->unLock();

        // go through each entry and send a packet if it has been updated

        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            TlmEntry* p_entry = &this->m_tlmEntries[1-this->m_activeBuffer].buckets[entry];
            if ((p_entry->updated) && (p_entry->used)) {
                this->m_tlmPacket.setId(p_entry->id);
                this->m_tlmPacket.setTimeTag(p_entry->lastUpdate);
                this->m_tlmPacket.setTlmBuffer(p_entry->buffer);
                this->m_comBuffer.resetSer();
                Fw::SerializeStatus stat = this->m_tlmPacket.serialize(this->m_comBuffer);
                FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
                p_entry->updated = false;
                this->PktSend_out(0,this->m_comBuffer,0);
            }
        }
    }

}
