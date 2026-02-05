// ======================================================================
// \title  CfdpChunk.hpp
// \brief  CFDP chunks (spare gap tracking) header file
//
// This file is a port of CFDP chunk/gap tracking from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_chunks.h (CFDP chunk and gap tracking definitions)
//
// ======================================================================
//
// NASA Docket No. GSC-18,447-1
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ======================================================================

#ifndef CFDP_CHUNK_HPP
#define CFDP_CHUNK_HPP

#include <functional>

#include <Fw/FPrimeBasicTypes.hpp>

#include <config/CfdpFileSizeAliasAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/StatusEnumAc.hpp>

namespace Svc {
namespace Ccsds {

using CfdpChunkIdx = U16;

/**
 * @brief Pairs an offset with a size to identify a specific piece of a file
 */
struct CfdpChunk
{
    CfdpFileSize offset; /**< \brief The start offset of the chunk within the file */
    CfdpFileSize size;   /**< \brief The size of the chunk */
};

/**
 * @brief Selects the larger of the two passed-in offsets
 *
 * @param a First chunk offset
 * @param b Second chunk offset
 * @return the larger CfdpFileSize value
 */
static inline CfdpFileSize CfdpChunkMax(CfdpFileSize a, CfdpFileSize b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

/**
 * @brief Callback type for gap computation
 *
 * std::function-based callback used by CfdpChunkList::computeGaps().
 * The callback receives the gap chunk and an opaque context pointer.
 */
using CfdpGapComputeCallback = std::function<void(const CfdpChunk* chunk, void* opaque)>;

/**
 * @brief C++ class encapsulation of CFDP chunk list operations
 *
 * This class provides modern C++ encapsulation around the gap tracking functionality
 * previously implemented as C-style free functions. The class does not own the backing
 * memory for chunks; it takes a pointer to pre-allocated memory in the constructor,
 * preserving the existing memory pooling system managed by CfdpChannel.
 *
 * The chunk list maintains file segments in offset-sorted order and provides operations
 * for adding segments, computing gaps, and managing the list. This is primarily used for:
 * - RX transactions: Track received file data segments to identify gaps for NAK packets
 * - TX transactions: Track NAK segment requests for retransmission
 */
class CfdpChunkList {
  public:
    // ----------------------------------------------------------------------
    // Construction and Destruction
    // ----------------------------------------------------------------------

    /**
     * @brief Constructor - initializes chunk list with pre-allocated memory
     *
     * @param maxChunks  Maximum number of chunks this list can hold
     * @param chunkMem   Pointer to pre-allocated array of CfdpChunk objects
     *
     * @note The class does NOT take ownership of chunkMem; memory is externally managed
     */
    CfdpChunkList(CfdpChunkIdx maxChunks, CfdpChunk* chunkMem);

    // ----------------------------------------------------------------------
    // Public Interface
    // ----------------------------------------------------------------------

    /**
     * @brief Add a chunk (file segment) to the list
     *
     * Adds a new chunk representing a file segment at the given offset and size.
     * The chunk may be combined with adjacent chunks if they are contiguous.
     * If the list is full, the smallest chunk may be evicted.
     *
     * @param offset  Starting offset of the chunk within the file
     * @param size    Size of the chunk in bytes
     */
    void add(CfdpFileSize offset, CfdpFileSize size);

    /**
     * @brief Reset the chunk list to empty state
     *
     * Removes all chunks from the list while preserving the max_chunks and
     * memory pointer configuration. After reset, the list is in the same state
     * as immediately after construction.
     */
    void reset();

    /**
     * @brief Get the first chunk in the list
     *
     * Returns a pointer to the first chunk (lowest offset) in the list without
     * removing it from the list.
     *
     * @returns Pointer to the first chunk
     * @retval  nullptr if the list is empty
     */
    const CfdpChunk* getFirstChunk() const;

    /**
     * @brief Remove a specified size from the first chunk
     *
     * Reduces the size of the first chunk by the specified amount. If the
     * size exactly matches the chunk size, the entire chunk is removed.
     * This is used for consuming data in-order during processing.
     *
     * @param size  Number of bytes to remove from the first chunk
     *
     * @note The list must not be empty when calling this function
     */
    void removeFromFirst(CfdpFileSize size);

    /**
     * @brief Compute gaps between chunks and invoke callback for each
     *
     * Walks the chunk list and computes gaps (missing file segments) between
     * chunks. For each gap found, invokes the provided callback function.
     * This is used to generate NAK segment requests.
     *
     * @param maxGaps    Maximum number of gaps to compute
     * @param total      Total size of the file
     * @param start      Starting offset for gap computation
     * @param callback   Callback function to invoke for each gap
     * @param opaque     Opaque pointer passed through to callback
     *
     * @returns Number of gaps computed (may be less than maxGaps if fewer gaps exist)
     */
    U32 computeGaps(CfdpChunkIdx maxGaps,
                    CfdpFileSize total,
                    CfdpFileSize start,
                    const CfdpGapComputeCallback& callback,
                    void* opaque) const;

    /**
     * @brief Get the current number of chunks in the list
     * @returns Current chunk count
     */
    CfdpChunkIdx getCount() const { return m_count; }

    /**
     * @brief Get the maximum number of chunks this list can hold
     * @returns Maximum chunk capacity
     */
    CfdpChunkIdx getMaxChunks() const { return m_maxChunks; }

  private:
    // ----------------------------------------------------------------------
    // Private Implementation
    // ----------------------------------------------------------------------

    /**
     * @brief Insert a chunk at a specific index
     *
     * Inserts the chunk at the specified index position, shifting existing
     * chunks as needed. May combine with adjacent chunks if contiguous.
     *
     * @param index  Index position to insert at
     * @param chunk  Chunk data to insert
     */
    void insertChunk(CfdpChunkIdx index, const CfdpChunk* chunk);

    /**
     * @brief Erase a single chunk at the given index
     *
     * Removes the chunk and shifts subsequent chunks to close the gap.
     *
     * @param index  Index of chunk to erase
     */
    void eraseChunk(CfdpChunkIdx index);

    /**
     * @brief Erase a range of chunks
     *
     * Removes chunks from start (inclusive) to end (exclusive) and shifts
     * remaining chunks to close the gap.
     *
     * @param start  Starting index (inclusive)
     * @param end    Ending index (exclusive)
     */
    void eraseRange(CfdpChunkIdx start, CfdpChunkIdx end);

    /**
     * @brief Find where a chunk should be inserted to maintain sorted order
     *
     * Uses binary search to find the insertion point based on chunk offset.
     *
     * @param chunk  Chunk data to find insertion point for
     * @returns Index where chunk should be inserted
     */
    CfdpChunkIdx findInsertPosition(const CfdpChunk* chunk);

    /**
     * @brief Attempt to combine chunk with the next chunk
     *
     * If the chunk is contiguous with the next chunk in the list, combines them.
     *
     * @param i      Index of the current chunk
     * @param chunk  Chunk data to attempt combining
     * @returns true if chunks were combined, false otherwise
     */
    bool combineNext(CfdpChunkIdx i, const CfdpChunk* chunk);

    /**
     * @brief Attempt to combine chunk with the previous chunk
     *
     * If the chunk is contiguous with the previous chunk in the list, combines them.
     *
     * @param i      Index of the current chunk
     * @param chunk  Chunk data to attempt combining
     * @returns true if chunks were combined, false otherwise
     */
    bool combinePrevious(CfdpChunkIdx i, const CfdpChunk* chunk);

    /**
     * @brief Insert a chunk, potentially combining with neighbors
     *
     * Inserts the chunk at position i and attempts to combine with adjacent chunks.
     *
     * @param i      Position to insert at
     * @param chunk  Chunk data to insert
     */
    void insert(CfdpChunkIdx i, const CfdpChunk* chunk);

    /**
     * @brief Find the index of the chunk with the smallest size
     *
     * Used when the list is full and a chunk needs to be evicted.
     *
     * @returns Index of the smallest chunk, or 0 if list is empty
     */
    CfdpChunkIdx findSmallestSize() const;

  private:
    // ----------------------------------------------------------------------
    // Private Member Variables
    // ----------------------------------------------------------------------

    CfdpChunkIdx m_count;       //!< Current number of chunks in the list
    CfdpChunkIdx m_maxChunks;   //!< Maximum number of chunks allowed
    CfdpChunk* m_chunks;        //!< Pointer to pre-allocated chunk array (not owned)
};

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_CHUNK_HPP */
