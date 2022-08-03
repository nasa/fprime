/*
 * TelemChanImpl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>

#include <cstdio>

namespace Svc {

    void TlmChanImpl::TlmGet_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {

        // Compute index for entry

        NATIVE_UINT_TYPE index = this->doHash(id);

        // Search to see if channel has been stored
        TlmEntry *entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
        for (NATIVE_UINT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
            if (entryToUse) { // If bucket exists, check id
                if (entryToUse->id == id) {
                    break;
                } else { // otherwise go to next bucket
                    entryToUse = entryToUse->next;
                }
            } else { // no buckets left to search
                break;
            }
        }

        if (entryToUse) {
            val  = entryToUse->buffer;
            timeTag = entryToUse->lastUpdate;
        } else { // requested entry may not be written yet; empty buffer
            val.resetSer();
        }

    }
}
