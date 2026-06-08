/**
 * \file
 * \author T. Canham
 * \brief Configuration file for Telemetry Channel component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef TLMCHANIMPLCFG_HPP_
#define TLMCHANIMPLCFG_HPP_

// Anonymous namespace for configuration parameters

// The parameters below tune the seeded hash function used to write and read
// entries in the database. The channel ID is XORed with a per-boot seed and
// then passed through a Murmur3 finalizer (>=32-bit FwChanIdType) or a Wang
// hash (16-bit FwChanIdType), and finally reduced modulo
// TLMCHAN_NUM_TLM_HASH_SLOTS. Only the 8-bit FwChanIdType path uses
// TLMCHAN_HASH_MOD_VALUE (modulo MOD_VALUE, then modulo the slot count);
// TLMCHAN_HASH_MOD_VALUE has no effect for wider channel-ID types.
// The values can be experimented with to try and balance the number
// of slots versus the number of buckets.
// To test the set of telemetry ID in the system to see if the hash is
// balanced, do the following:
//  1) From the deployment directory (e.g Ref), do a full build then type:
//      "make comp_report_gen"
//     This will generate a list in "<deployment dir>/ComponentReport.txt"
//     with all the telemetry IDs in the deployment.
//  2) Cut and paste the ID list to the array declared in the TlmChan unit test
//     file TelemChanImplTester.cpp in the runMultiChannel() method.
//  3) Run the unit test ("make ut run_ut")
//  4) After writing all the telemetry IDs to the component, the
//     unit test will dump the hash table. The output looks like the following:
//        Slot: <n> - slot number
//        Entry - a bucket assigned to the slot
//        ... (Other buckets in the slot)
//     The number of buckets assigned to each slot can be checked for balance.

namespace {

enum {
    TLMCHAN_NUM_TLM_HASH_SLOTS = 15,  // !< Number of slots in the hash table.
                                      // Works best when set to about twice the number of components producing telemetry
    TLMCHAN_HASH_MOD_VALUE = 99,      // !< The modulo value of the hashing function.
                                      // Should be set to a little below the ID gaps to spread the entries around

    TLMCHAN_HASH_BUCKETS = 500,  // !< Buckets assignable to a hash slot.
                                 // Buckets must be >= number of telemetry channels in system

    // Maximum number of updated telemetry entries Run_handler will serialize
    // per invocation. The default of TLMCHAN_HASH_BUCKETS makes the cap a
    // no-op and preserves the original uncapped behavior (all updated entries
    // are processed each cycle). Lowering this value bounds Run_handler's
    // worst-case execution time, but it is an observable behavioral change:
    // updated entries beyond the cap are dropped for the cycle and a
    // WARNING_HI TlmChanEpochProcessingCapReached event is emitted. Reducing
    // it therefore requires CCB approval for the affected deployment.
    TLMCHAN_MAX_ENTRIES_PER_RUN = TLMCHAN_HASH_BUCKETS,
};

}

#endif /* TLMCHANIMPLCFG_HPP_ */
