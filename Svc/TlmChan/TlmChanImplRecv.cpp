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

    void TlmChanImpl::TlmRecv_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {

        // Compute index for entry

        NATIVE_UINT_TYPE index = this->doHash(id);
        TlmEntry* entryToUse = nullptr;
        TlmEntry* prevEntry = nullptr;

        // Search to see if channel has already been stored or a bucket needs to be added
        if (this->m_tlmEntries[this->m_activeBuffer].slots[index]) {
            entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
            for (NATIVE_UINT_TYPE bucket = 0; bucket < TLMCHAN_HASH_BUCKETS; bucket++) {
                if (entryToUse) {
                    if (entryToUse->id == id) { // found the matching entry
                        break;
                    } else { // try next entry
                        prevEntry = entryToUse;
                        entryToUse = entryToUse->next;
                    }
                } else {
                    // Make sure that we haven't run out of buckets
                    FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
                    // add new bucket from free list
                    entryToUse = &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
                    FW_ASSERT(prevEntry);
                    prevEntry->next = entryToUse;
                    // clear next pointer
                    entryToUse->next = nullptr;
                    break;
                }
            }
        } else {
            // Make sure that we haven't run out of buckets
            FW_ASSERT(this->m_tlmEntries[this->m_activeBuffer].free < TLMCHAN_HASH_BUCKETS);
            // create new entry at slot head
            this->m_tlmEntries[this->m_activeBuffer].slots[index] = &this->m_tlmEntries[this->m_activeBuffer].buckets[this->m_tlmEntries[this->m_activeBuffer].free++];
            entryToUse = this->m_tlmEntries[this->m_activeBuffer].slots[index];
            entryToUse->next = nullptr;
        }

        // copy into entry
        FW_ASSERT(entryToUse);
        entryToUse->used = true;
        entryToUse->id = id;
        entryToUse->updated = true;
        entryToUse->lastUpdate = timeTag;
        entryToUse->buffer = val;

    }
}
