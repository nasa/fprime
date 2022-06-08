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
        Fw::TlmPacket pkt;

        for (U32 entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            TlmEntry* p_entry = &this->m_tlmEntries[1-this->m_activeBuffer].buckets[entry];
            if ((p_entry->updated) && (p_entry->used)) {
                Fw::SerializeStatus stat = pkt.addValue(
                    p_entry->id,
                    p_entry->lastUpdate,
                    p_entry->buffer
                );

                // check to see if this packet is full, if so, send it
                if (Fw::FW_SERIALIZE_NO_ROOM_LEFT == stat) {
                    this->PktSend_out(0,pkt.getBuffer(),0);
                    // reset packet for more entries
                    pkt.resetPktSer();
                    // add entry to new packet
                    Fw::SerializeStatus stat = pkt.addValue(
                        p_entry->id,
                        p_entry->lastUpdate,
                        p_entry->buffer
                    );
                    // if this doesn't work, that means packet isn't big enough for
                    // even one channel, so assert
                    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

                // if successful, move on to the next packet
                } else if (Fw::FW_SERIALIZE_OK == stat) {
                    p_entry->updated = false;
                // any other status is an assert, since it shouldn't happen
                } else {
                    FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(stat));
                }
            } // end if entry was updated
        } // end for each entry
    } // end run handler

}
