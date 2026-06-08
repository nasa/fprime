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
#include <config/TlmChanImplCfg.hpp>

namespace Svc {

class TlmChan final : public TlmChanComponentBase {
    friend class TlmChanTester;

  public:
    TlmChan(const char* compName);
    virtual ~TlmChan();

  protected:
    FwChanIdType doHash(FwChanIdType id) const;

  private:
    // Port functions
    void TlmRecv_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val);
    Fw::TlmValid TlmGet_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val);
    void Run_handler(FwIndexType portNum, U32 context);
    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
    );
    enum class ActiveBuffer : U8 { Buffer_0 = 0, Buffer_1 = 1 };
    struct TlmEntry {
        FwChanIdType id;  //!< telemetry id stored in slot
        bool updated;     //!< set whenever a value has been written. Used to skip if writing out values for downlinking
        Fw::Time lastUpdate;    //!< last updated time
        Fw::TlmBuffer buffer;   //!< buffer to store serialized telemetry
        TlmEntry* next;         //!< pointer to next bucket in table
        bool used;              //!< if entry has been used
        FwChanIdType bucketNo;  //!< for testing
    };

    struct TlmSet {
        TlmEntry* slots[TLMCHAN_NUM_TLM_HASH_SLOTS];  //!< set of hash slots in hash table
        TlmEntry buckets[TLMCHAN_HASH_BUCKETS];       //!< set of buckets used in hash table
        FwChanIdType free;                            //!< next free bucket
    } m_tlmEntries[2];

    U32 m_hashSeed;  // !< per-boot random seed for telemetry hash

    U32 m_procCapCount;  // !< per-epoch processing guard for Run_handler

    U32 m_chanIdSize;  // !< the deployed channel size

    static const U32 MURMUR3_C1 = 0x85EBCA6BU;  // Murmur3 32-bit finalizer multiplier 1
    static const U32 MURMUR3_C2 = 0xC2B2AE35U;  // Murmur3 32-bit finalizer multiplier 2
    static const U16 WANG16_C1 = 0x2993U;       // Wang 16-bit hash multiplier 1
    static const U16 WANG16_C2 = 0xE877U;       // Wang 16-bit hash multiplier 2
    ActiveBuffer m_activeBuffer;                // !< which buffer is active for storing telemetry
};

}  // namespace Svc

#endif /* TELEMCHANIMPL_HPP_ */
