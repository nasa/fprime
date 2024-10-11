/**
 * \file
 * \author T. Canham
 * \brief Component that stores telemetry channel values
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef TELEMCHANIMPL_HPP_
#define TELEMCHANIMPL_HPP_

#include <Fw/Tlm/TlmPacket.hpp>
#include <Svc/TlmChan/TlmChanComponentAc.hpp>
#include <TlmChanImplCfg.hpp>

namespace Svc {

class TlmChan : public TlmChanComponentBase {
  public:
    TlmChan(const char* compName);
    virtual ~TlmChan();

  PROTECTED:
    // can be overridden for alternate algorithms
    virtual NATIVE_UINT_TYPE doHash(FwChanIdType id);

  PRIVATE:
    // Port functions
    void TlmRecv_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val);
    void TlmGet_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val);
    void Run_handler(NATIVE_INT_TYPE portNum, U32 context);
    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                        U32 key                        /*!< Value to return to pinger*/
    );

    typedef struct tlmEntry {
        FwChanIdType id;  //!< telemetry id stored in slot
        bool updated;     //!< set whenever a value has been written. Used to skip if writing out values for downlinking
        Fw::Time lastUpdate;        //!< last updated time
        Fw::TlmBuffer buffer;       //!< buffer to store serialized telemetry
        tlmEntry* next;             //!< pointer to next bucket in table
        bool used;                  //!< if entry has been used
        NATIVE_UINT_TYPE bucketNo;  //!< for testing
    } TlmEntry;

    struct TlmSet {
        TlmEntry* slots[TLMCHAN_NUM_TLM_HASH_SLOTS];  //!< set of hash slots in hash table
        TlmEntry buckets[TLMCHAN_HASH_BUCKETS];       //!< set of buckets used in hash table
        NATIVE_INT_TYPE free;                         //!< next free bucket
    } m_tlmEntries[2];

    U32 m_activeBuffer;  // !< which buffer is active for storing telemetry
};

}  // namespace Svc

#endif /* TELEMCHANIMPL_HPP_ */
