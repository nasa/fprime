// ======================================================================
// \title  TlmChan/test/ut/TlmChanHashTester.cpp
// \brief  Unit tests for the cyber-hardened doHash() implementation.
//
// Background
// ----------
// The original hash  doHash(id) = (id % MOD) % SLOTS  is a linear, seed-free
// transformation.  An attacker who knows TLMCHAN_NUM_TLM_HASH_SLOTS (a public
// config constant) can trivially enumerate channel IDs that all map to the same
// slot, exhaust the free-bucket pool, and either deny service to legitimate
// channels or shadow them with attacker-controlled values.
//
// The replacement hash mixes in a per-boot random seed:
//   ≥32-bit IDs : Murmur3 finalizer  (strong avalanche, near-uniform output)
//   16-bit IDs  : Wang hash          (comparable properties for narrower type)
//   <16-bit IDs : XOR-seed + modulo  (lightweight; only 256 distinct IDs exist)
//
// These tests verify the five security properties that the fix must provide:
//
//   1. Range          — output is always in [0, TLMCHAN_NUM_TLM_HASH_SLOTS)
//   2. Determinism    — repeated calls return the same slot within a session
//   3. Seed diversity — two component instances get independent seeds
//   4. Avalanche      — a 1-bit input change scrambles the output slot
//   5. Collision-hardening — the known linear-attack ID sequence no longer
//                            concentrates into a single slot
//   6. Distribution   — slot occupancy is roughly uniform over a large sample
//
// ======================================================================

#include <Fw/Test/UnitTest.hpp>
#include <config/TlmChanImplCfg.hpp>
#include "TlmChanTester.hpp"

// ---- compile-time size of FwChanIdType (used to label the active branch) ----
static constexpr size_t FW_CHAN_ID_BYTES = sizeof(FwChanIdType);
static constexpr U32 FW_CHAN_ID_BITS = static_cast<U32>(FW_CHAN_ID_BYTES * 8u);

namespace Svc {

// ============================================================================
// Test 1 — Hash-width identification and output-range check
// ============================================================================

void TlmChanTester::runHashSizeIdentification() {
    // Print which algorithm branch is active so the test log is self-documenting.
    if (FW_CHAN_ID_BYTES >= 4) {
        printf("[HashSizeId] sizeof(FwChanIdType)=%u  => Murmur3 32-bit path active\n",
               static_cast<unsigned>(FW_CHAN_ID_BYTES));
    } else if (FW_CHAN_ID_BYTES == 2) {
        printf("[HashSizeId] sizeof(FwChanIdType)=%u  => Wang 16-bit path active\n",
               static_cast<unsigned>(FW_CHAN_ID_BYTES));
    } else {
        printf("[HashSizeId] sizeof(FwChanIdType)=%u  => XOR+modulo <16-bit path active\n",
               static_cast<unsigned>(FW_CHAN_ID_BYTES));
    }

    // ------------------------------------------------------------------
    // Range invariant: every possible return value must be a valid slot
    // index.  Test the first 1024 IDs exhaustively (or all of them for
    // narrow types where 256 is the complete universe).
    // ------------------------------------------------------------------
    const U32 probeLimit = (FW_CHAN_ID_BYTES == 1) ? 256u : 1024u;

    for (U32 id = 0; id < probeLimit; id++) {
        const FwChanIdType slot = this->component.doHash(static_cast<FwChanIdType>(id));
        ASSERT_LT(slot, static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS))
            << "Out-of-range slot " << static_cast<unsigned>(slot) << " returned for id=" << id
            << "  (TLMCHAN_NUM_TLM_HASH_SLOTS=" << TLMCHAN_NUM_TLM_HASH_SLOTS << ")";
    }

    // Additionally probe a handful of large IDs that might exercise wrap-around
    // in the modulo reduction.
    static const FwChanIdType largeIds[] = {
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS),
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS - 1u),
        static_cast<FwChanIdType>(TLMCHAN_HASH_BUCKETS),
        static_cast<FwChanIdType>(TLMCHAN_HASH_BUCKETS - 1u),
        static_cast<FwChanIdType>(0xFFFFu),
        static_cast<FwChanIdType>(0xFFFFFFFFu),
    };
    for (FwChanIdType id : largeIds) {
        const FwChanIdType slot = this->component.doHash(id);
        ASSERT_LT(slot, static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS))
            << "Out-of-range slot for large id=0x" << std::hex << static_cast<unsigned>(id);
    }
}

// ============================================================================
// Test 2 — Within-session determinism
// ============================================================================

void TlmChanTester::runHashDeterminism() {
    // doHash() must be a pure function of (id, m_hashSeed).  Because the seed
    // is fixed for the lifetime of the component, the same id must always yield
    // the same slot.  Non-determinism here would corrupt the hash-table lookup.

    static const FwChanIdType testIds[] = {
        0x0000u,
        0x0001u,
        0x1000u,
        0x5678u,
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS - 1u),
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS),
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS + 1u),
        static_cast<FwChanIdType>(TLMCHAN_HASH_BUCKETS - 1u),
        0xFFFFu,
    };

    for (FwChanIdType id : testIds) {
        const FwChanIdType reference = this->component.doHash(id);

        // 200 repetitions is more than enough to catch a spurious PRNG or
        // uninitialized-memory bug.
        for (U32 rep = 0; rep < 200u; rep++) {
            EXPECT_EQ(reference, this->component.doHash(id))
                << "doHash() returned a different value on repetition " << rep << " for id=0x" << std::hex
                << static_cast<unsigned>(id);
        }

        // Confirm the result is still in range.
        EXPECT_LT(reference, static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS));
    }
}

// ============================================================================
// Test 3 — Per-boot seed diversity
// ============================================================================

void TlmChanTester::runHashSeedDiversity() {
    // Each TlmChan instance draws its seed from an OSAL high-resolution
    // timestamp (Os::RawTime) and a stack-address nonce at construction time.
    // Two independently constructed instances should (where the platform
    // provides boot-time entropy) get different seeds, denying an attacker the
    // ability to pre-compute collision lists offline. NOTE: this property
    // depends on the platform — on bare-metal targets with a fixed boot clock
    // and deterministic memory layout the seeds may coincide; that is a
    // platform-entropy limitation, not a hash-logic defect (see sdd.md 3.5.2).
    //
    // False-failure probability: 1 / 2^32 ≈ 2.3 × 10^-10.  If this test ever
    // fails on a CI run without a deliberate code change, investigate the
    // entropy source rather than dismissing it as a flake.

    TlmChan second("TlmChanSecondary");

    printf("[SeedDiversity] component.m_hashSeed = 0x%08X   second.m_hashSeed = 0x%08X\n",
           static_cast<unsigned>(this->component.m_hashSeed), static_cast<unsigned>(second.m_hashSeed));

    EXPECT_NE(this->component.m_hashSeed, second.m_hashSeed) << "Both TlmChan instances received the same seed — "
                                                                "entropy source may be broken or stubbed out.";

    // With different seeds the two instances must produce different slot mappings
    // for most inputs.  Sample a diverse set of IDs and count disagreements.
    U32 differences = 0;
    const U32 sampleSize = 256u;

    for (U32 i = 0; i < sampleSize; i++) {
        // Stride by a prime to avoid accidentally sampling a harmonic of
        // TLMCHAN_NUM_TLM_HASH_SLOTS.
        FwChanIdType id = static_cast<FwChanIdType>(i * 13u + 7u);
        if (this->component.doHash(id) != second.doHash(id)) {
            differences++;
        }
    }

    printf("[SeedDiversity] %u/%u sampled IDs hashed to different slots\n", differences, sampleSize);

    // Even a weak hash with independent seeds should disagree on at least half
    // the sample.  Murmur3 / Wang in practice disagrees on ~(1 - 1/SLOTS) of
    // inputs, which for typical SLOTS values is well above 90 %.
    EXPECT_GT(differences, sampleSize / 2u) << "Instances with different seeds produced too many identical slot "
                                               "mappings ("
                                            << differences << "/" << sampleSize
                                            << " differed).  "
                                               "The seed may not be influencing the hash output.";
}

// ============================================================================
// Test 4 — Avalanche effect (non-linearity)
// ============================================================================

void TlmChanTester::runHashAvalanche() {
    // A 1-bit change in the channel ID must propagate through the hash and
    // change the output slot for a statistically significant fraction of cases.
    //
    // This is the defining difference between the new hash and the old linear
    // modulo: under  (id % MOD) % SLOTS,  adjacent IDs often land in the same
    // slot, making incremental probing trivially effective.
    //
    // Test methodology
    // ----------------
    // For each of several diverse base IDs, flip every bit in the ID one at a
    // time and check whether the output slot changes.  Aggregate across all
    // base IDs and bit positions and assert that at least 40 % of flips
    // produced a different slot.
    //
    // 40 % is a deliberately generous lower bound that avoids flakiness while
    // still catching any obviously broken (near-linear) implementation.
    // Murmur3 and Wang both typically exceed 85 % for typical SLOTS values.

    static const FwChanIdType bases[] = {
        0x00000001u,                                                      // low bit set
        0x12345678u,                                                      // arbitrary mixed pattern
        0xABCD1234u,                                                      // high bits active
        0xFFFF0000u,                                                      // upper half only
        0x00010001u,                                                      // sparse bits
        static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS * 3u + 7u),  // near a slot boundary
    };

    U32 totalFlips = 0u;
    U32 outputChanged = 0u;

    for (FwChanIdType base : bases) {
        const FwChanIdType baseSlot = this->component.doHash(base);

        for (U32 bit = 0u; bit < FW_CHAN_ID_BITS; bit++) {
            const FwChanIdType flipped = base ^ (static_cast<FwChanIdType>(1u) << bit);
            const FwChanIdType flippedSlot = this->component.doHash(flipped);

            if (flippedSlot != baseSlot) {
                outputChanged++;
            }
            totalFlips++;
        }
    }

    const U32 threshold = totalFlips * 40u / 100u;  // 40 % lower bound

    printf(
        "[Avalanche] %u/%u single-bit flips changed the output slot  "
        "(threshold >= %u)\n",
        outputChanged, totalFlips, threshold);

    EXPECT_GT(outputChanged, threshold) << "Poor avalanche: only " << outputChanged << "/" << totalFlips
                                        << " 1-bit flips changed the hash slot.  "
                                           "The hash may still be near-linear.";
}

// ============================================================================
// Test 5 — Collision-hardening: original attack vector regression
// ============================================================================

void TlmChanTester::runCollisionHardening() {
    // This is the direct security regression test.
    //
    // Original vulnerability
    // ~~~~~~~~~~~~~~~~~~~~~~
    // doHash(id) = (id % TLMCHAN_HASH_MOD_VALUE) % TLMCHAN_NUM_TLM_HASH_SLOTS
    //
    // Attack recipe (requires only the public constant TLMCHAN_NUM_TLM_HASH_SLOTS):
    //   IDs = { 0, SLOTS, 2*SLOTS, 3*SLOTS, ... }
    //   → all reduce to 0 mod SLOTS → all land in slot 0.
    //   Send TLMCHAN_HASH_BUCKETS such IDs → slot 0's linked list fills the
    //   entire free-bucket pool → every subsequent TlmRecv_handler call for
    //   a legitimate channel hits FW_ASSERT (DoS) or overwrites another
    //   channel's entry (shadowing).
    //
    // Expected hardened behavior
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~
    // The seeded hash scatters these IDs uniformly, so the attacker must probe
    // O(SLOTS × BUCKETS) IDs before any one slot accumulates BUCKETS collisions.
    // Without the seed, even SLOTS × BUCKETS random probes fill at most ~1 slot
    // with high probability (birthday paradox).
    //
    // What this test checks
    // ~~~~~~~~~~~~~~~~~~~~~
    // 1. Of 4 × BUCKETS attack IDs, the most-populated slot receives fewer
    //    than BUCKETS hits (the attack can no longer fill the pool).
    // 2. Slot 0 in particular receives no more than 3 × expected hits
    //    (expected ≈ 4 × BUCKETS / SLOTS), confirming it is not a hot-spot.

    // Use 4× BUCKETS probes — under the old hash ALL of them mapped to slot 0.
    const U32 probeCount = static_cast<U32>(TLMCHAN_HASH_BUCKETS) * 4u;

    // Per-slot collision counter.  Stack allocation is safe: SLOTS is a small
    // compile-time constant (typically 16–128).
    FwChanIdType slotHits[TLMCHAN_NUM_TLM_HASH_SLOTS] = {};

    for (U32 k = 0u; k < probeCount; k++) {
        // Reproduce the exact attack sequence: multiples of SLOTS.
        // Cast through U32 to suppress potential overflow warnings; wrapping
        // is harmless here because we are testing the hash of those IDs.
        const U32 rawId = k * static_cast<U32>(TLMCHAN_NUM_TLM_HASH_SLOTS);
        const FwChanIdType attackId = static_cast<FwChanIdType>(rawId);
        const FwChanIdType slot = this->component.doHash(attackId);

        ASSERT_LT(slot, static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS))
            << "Hash returned out-of-range slot during collision-hardening probe";

        slotHits[slot]++;
    }

    // Find the most-hit slot.
    FwChanIdType maxHits = 0u;
    FwChanIdType maxSlot = 0u;
    for (FwChanIdType s = 0u; s < static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS); s++) {
        if (slotHits[s] > maxHits) {
            maxHits = slotHits[s];
            maxSlot = s;
        }
    }

    // Expected hits per slot under ideal uniform distribution.
    const FwChanIdType expectedPerSlot = static_cast<FwChanIdType>(probeCount / TLMCHAN_NUM_TLM_HASH_SLOTS);

    printf(
        "[CollisionHardening] probed %u IDs (old attack pattern: k*%u)\n"
        "  TLMCHAN_HASH_BUCKETS=%u  TLMCHAN_NUM_TLM_HASH_SLOTS=%u\n"
        "  expected per slot ~%u  max hits=%u (slot %u)  slot-0 hits=%u\n",
        probeCount, static_cast<unsigned>(TLMCHAN_NUM_TLM_HASH_SLOTS), static_cast<unsigned>(TLMCHAN_HASH_BUCKETS),
        static_cast<unsigned>(TLMCHAN_NUM_TLM_HASH_SLOTS), static_cast<unsigned>(expectedPerSlot),
        static_cast<unsigned>(maxHits), static_cast<unsigned>(maxSlot), static_cast<unsigned>(slotHits[0]));

    // ---- Assertion 1: attack cannot fill any single slot ----
    // Under the old hash, slot 0 received ALL probeCount hits (probeCount >> BUCKETS).
    // With the new hash the most-hit slot must stay below BUCKETS.
    EXPECT_LT(maxHits, static_cast<FwChanIdType>(TLMCHAN_HASH_BUCKETS))
        << "The old-attack ID sequence still fills slot " << static_cast<unsigned>(maxSlot) << " ("
        << static_cast<unsigned>(maxHits) << " hits >= TLMCHAN_HASH_BUCKETS=" << TLMCHAN_HASH_BUCKETS
        << ").  "
           "The seeded hash did not break the linear collision pattern.";

    // ---- Assertion 2: slot 0 is not a hot-spot ----
    // Allow up to 3× the expected average — well above random variance but
    // far below the old value of probeCount (4 × BUCKETS).
    const FwChanIdType slot0Tolerance = expectedPerSlot * 3u;
    EXPECT_LE(slotHits[0], slot0Tolerance)
        << "Slot 0 received " << static_cast<unsigned>(slotHits[0])
        << " hits from the attack sequence but no more than " << static_cast<unsigned>(slot0Tolerance)
        << " (3 × expected) should land there after hardening.";
}

// ============================================================================
// Test 6 — Statistical uniformity of hash distribution
// ============================================================================

void TlmChanTester::runHashDistribution() {
    // A good hash spreads channel IDs uniformly across slots.  Non-uniform
    // distribution creates hot-spot slots that:
    //   (a) degrade lookup performance for legitimate channels, and
    //   (b) reduce the attacker effort to fill a hot-spot by probing
    //       a smaller search space than the theoretical O(SLOTS × BUCKETS).
    //
    // Methodology
    // -----------
    // Hash 50 sequential IDs per slot (sampleSize = 50 × SLOTS).  Count the
    // number of IDs falling into each slot and check:
    //   • max count ≤ 3 × expected   (no hot-spots)
    //   • min count ≥ 1              (no dead-zone slots)
    //
    // These are very generous bounds; a broken hash (pure modulo) would have
    // every ID fall into just one slot, a ratio of SLOTS:1 over expected.

    const U32 multiplier = 50u;
    const U32 sampleSize = multiplier * static_cast<U32>(TLMCHAN_NUM_TLM_HASH_SLOTS);
    const FwChanIdType expectedPerSlot =
        static_cast<FwChanIdType>(sampleSize / TLMCHAN_NUM_TLM_HASH_SLOTS);  // = multiplier

    FwChanIdType counts[TLMCHAN_NUM_TLM_HASH_SLOTS] = {};

    for (U32 i = 0u; i < sampleSize; i++) {
        const FwChanIdType id = static_cast<FwChanIdType>(i);
        const FwChanIdType slot = this->component.doHash(id);

        ASSERT_LT(slot, static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS))
            << "Hash returned out-of-range slot for sequential id=" << i;

        counts[slot]++;
    }

    // Find min / max occupancy.
    FwChanIdType minCount = counts[0];
    FwChanIdType maxCount = counts[0];
    FwChanIdType maxSlot = 0u;
    for (FwChanIdType s = 1u; s < static_cast<FwChanIdType>(TLMCHAN_NUM_TLM_HASH_SLOTS); s++) {
        if (counts[s] < minCount) {
            minCount = counts[s];
        }
        if (counts[s] > maxCount) {
            maxCount = counts[s];
            maxSlot = s;
        }
    }

    printf(
        "[HashDistribution] %u IDs across %u slots  "
        "expected=%u/slot  min=%u  max=%u (slot %u)\n",
        sampleSize, static_cast<unsigned>(TLMCHAN_NUM_TLM_HASH_SLOTS), static_cast<unsigned>(expectedPerSlot),
        static_cast<unsigned>(minCount), static_cast<unsigned>(maxCount), static_cast<unsigned>(maxSlot));

    // ---- Assertion 1: no hot-spot slots ----
    const FwChanIdType hotSpotThreshold = expectedPerSlot * 3u;
    EXPECT_LE(maxCount, hotSpotThreshold)
        << "Slot " << static_cast<unsigned>(maxSlot) << " is a hot-spot: " << static_cast<unsigned>(maxCount)
        << " hits vs expected " << static_cast<unsigned>(expectedPerSlot)
        << " (threshold=" << static_cast<unsigned>(hotSpotThreshold) << ").";

    // ---- Assertion 2: no dead-zone (empty) slots ----
    // With 50× over sampling, every slot must receive at least one hit.
    EXPECT_GT(minCount, 0u) << "At least one slot received zero hits from " << sampleSize
                            << " sequential IDs.  The hash has a dead-zone.";
}

}  // namespace Svc
