/*
 * Main.cpp
 *
 *  Created on: Mar 18, 2015
 *  Updated: 6/22/2022
 *      Author: tcanham
 */

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/TlmChan/TlmChan.hpp>
#include "TlmChanTester.hpp"

// ============================================================================
// Functional tests (preexisting)
// ============================================================================

TEST(TlmChanTest, InitTest) {
    Svc::TlmChanTester tester;
}

TEST(TlmChanTest, NominalChannelTest) {
    TEST_CASE(107.1.1, "Nominal channelized telemetry");
    COMMENT("Write a single channel and verify it is read back and pushed correctly.");

    Svc::TlmChanTester tester;
    // run test
    tester.runNominalChannel();
}

TEST(TlmChanTest, MultiChannelTest) {
    TEST_CASE(107.1.2, "Nominal Multi-channel channelized telemetry");
    COMMENT("Write multiple channels and verify they are read back and pushed correctly.");

    Svc::TlmChanTester tester;

    // run test
    tester.runMultiChannel();
}

TEST(TlmChanTest, OffNominal) {
    TEST_CASE(107.2.1, "Off-nominal channelized telemetry");
    COMMENT("Attempt to read a channel that hasn't been written.");

    Svc::TlmChanTester tester;

    // run test
    tester.runOffNominal();
}

TEST(TlmChanTest, ProcGuardTest) {
    TEST_CASE(107.3.1, "Off-nominal Run_handler processing limit (per epoch)");
    COMMENT("Verify Run_handler defers entries beyond TLMCHAN_MAX_ENTRIES_PER_RUN to next epoch.");

    Svc::TlmChanTester tester;

    // run test
    tester.runProcGuard();
}

// ============================================================================
// doHash() cyber-security tests  (107.4.x)
//
// The original hash  doHash(id) = (id % MOD) % SLOTS  was a seed-free linear
// transformation.  An attacker who knew only TLMCHAN_NUM_TLM_HASH_SLOTS could
// enumerate channel IDs that all map to the same slot, fill the free-bucket
// pool, and deny service to (or shadow) legitimate channels.
//
// The replacement uses a per-boot random seed fed into Murmur3 (≥32-bit IDs),
// Wang (16-bit), or XOR-then-modulo (<16-bit) so the slot mapping is
// unpredictable without the seed.
//
// Each test below targets one independent security property so that a failure
// pinpoints exactly which invariant was broken.
// ============================================================================

// ----------------------------------------------------------------------------
// 107.4.1  Hash-width identification and output-range check
//
// Purpose : Confirm the correct algorithm branch compiled in, and prove that
//           every possible return value is a valid slot index.  An out-of-range
//           slot would corrupt the hash table.
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashSizeIdentification) {
    TEST_CASE(107.4.1, "doHash width-branch identification and range check");
    COMMENT(
        "Identify the active hash algorithm (Murmur3 / Wang / XOR) and verify "
        "all outputs are in [0, TLMCHAN_NUM_TLM_HASH_SLOTS).");

    Svc::TlmChanTester tester;
    tester.runHashSizeIdentification();
}

// ----------------------------------------------------------------------------
// 107.4.2  Within-session determinism
//
// Purpose : doHash() must be a pure function of (id, seed).  Non-determinism
//           would corrupt hash-table lookups independently of any attack.
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashDeterminism) {
    TEST_CASE(107.4.2, "doHash within-session determinism");
    COMMENT(
        "Verify that repeated calls to doHash() with the same channel ID "
        "always return the same slot within one component lifetime.");

    Svc::TlmChanTester tester;
    tester.runHashDeterminism();
}

// ----------------------------------------------------------------------------
// 107.4.3  Per-boot seed diversity
//
// Purpose : Two independently constructed TlmChan instances must receive
//           different seeds with overwhelming probability.  If seeds are
//           identical (e.g., from a constant or stub), an attacker can compute
//           collision lists offline and replay them against any deployment.
//
// Acceptable false-failure rate: 1 in 2^32 (~2.3 × 10^-10).
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashSeedDiversity) {
    TEST_CASE(107.4.3, "doHash per-boot seed diversity");
    COMMENT(
        "Verify that two TlmChan instances receive statistically independent "
        "seeds so offline pre-computation of collision lists is infeasible.");

    Svc::TlmChanTester tester;
    tester.runHashSeedDiversity();
}

// ----------------------------------------------------------------------------
// 107.4.4  Avalanche effect (non-linearity)
//
// Purpose : A 1-bit change in the channel ID must cause the output slot to
//           change for a statistically significant fraction of cases.  The old
//           linear hash failed this: adjacent IDs in the attack sequence mapped
//           to the same or predictably different slots.
//
// Pass criterion: > 40 % of all tested single-bit flips change the output slot.
//                 Murmur3 / Wang typically exceed 85 % for real SLOTS values.
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashAvalanche) {
    TEST_CASE(107.4.4, "doHash avalanche effect (non-linearity)");
    COMMENT(
        "Verify that a 1-bit change in the channel ID changes the output slot "
        "for the majority of cases, confirming non-linear diffusion.");

    Svc::TlmChanTester tester;
    tester.runHashAvalanche();
}

// ----------------------------------------------------------------------------
// 107.4.5  Collision-hardening: original attack-vector regression
//
// Purpose : The exact ID sequence used to exploit the old linear hash
//           (multiples of TLMCHAN_NUM_TLM_HASH_SLOTS) must no longer
//           concentrate into a single slot.
//
// Under the old hash ALL probed IDs landed in slot 0.  Under the fixed hash
// the maximum per-slot hits from 4 × BUCKETS attack IDs must remain below
// BUCKETS — i.e., the attacker cannot fill any slot with the known sequence.
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashCollisionHardening) {
    TEST_CASE(107.4.5, "doHash collision-hardening: original DoS/shadowing attack regression");
    COMMENT(
        "Verify that the known linear-collision attack sequence (k * SLOTS) "
        "no longer fills any hash slot, improving cyber.");

    Svc::TlmChanTester tester;
    tester.runCollisionHardening();
}

// ----------------------------------------------------------------------------
// 107.4.6  Statistical uniformity of slot distribution
//
// Purpose : A hot-spot slot (where many IDs naturally cluster) reduces the
//           attacker effort to find collisions and degrades lookup performance.
//           This test verifies no slot has more than 3× the expected occupancy
//           and no slot is a dead-zone.
// ----------------------------------------------------------------------------
TEST(TlmChanHashTest, HashDistribution) {
    TEST_CASE(107.4.6, "doHash slot-distribution uniformity");
    COMMENT(
        "Hash 50 × SLOTS sequential IDs and verify no slot is a hot-spot "
        "(> 3× average) or dead-zone (0 hits).");

    Svc::TlmChanTester tester;
    tester.runHashDistribution();
}
