// ======================================================================
// \title  Chunk.cpp
// \brief  CFDP chunks (sparse gap tracking) logic file
//
// This file is a port of the cf_chunks.c file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This class handles the complexity of sparse gap tracking so that
// the CFDP engine doesn't need to worry about it. Information is given
// to the class and when needed calculations are made internally to
// help the engine build NAK packets. Received NAK segment requests
// are stored in this class as well and used for re-transmit processing.
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

#include <string.h>

#include <Fw/Types/Assert.hpp>

#include <Svc/Ccsds/CfdpManager/Chunk.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ======================================================================
// CfdpChunkList Class Implementation
// ======================================================================

CfdpChunkList::CfdpChunkList(ChunkIdx maxChunks, Chunk* chunkMem)
    : m_count(0), m_maxChunks(maxChunks), m_chunks(chunkMem)
{
    FW_ASSERT(maxChunks > 0);
    FW_ASSERT(chunkMem != nullptr);
    reset();
}

void CfdpChunkList::reset()
{
    m_count = 0;
    memset(m_chunks, 0, sizeof(*m_chunks) * m_maxChunks);
}

void CfdpChunkList::add(FileSize offset, FileSize size)
{
    const Chunk chunk = {offset, size};
    const ChunkIdx i = findInsertPosition(&chunk);

    /* PTFO: files won't be so big we need to gracefully handle overflow,
     * and in that case the user should change everything in chunks
     * to use 64-bit numbers */
    FW_ASSERT((offset + size) >= offset, offset, size);

    insert(i, &chunk);
}

const Chunk* CfdpChunkList::getFirstChunk() const
{
    return m_count ? &m_chunks[0] : nullptr;
}

void CfdpChunkList::removeFromFirst(FileSize size)
{
    Chunk* chunk = &m_chunks[0]; /* front is always 0 */

    if (size > chunk->size)
    {
        size = chunk->size;
    }
    chunk->size -= size;

    if (!chunk->size)
    {
        eraseChunk(0);
    }
    else
    {
        chunk->offset += size;
    }
}

U32 CfdpChunkList::computeGaps(ChunkIdx maxGaps,
                                FileSize total,
                                FileSize start,
                                const GapComputeCallback& callback,
                                void* opaque) const
{
    U32 ret = 0;
    ChunkIdx i = 0;
    FileSize next_off;
    FileSize gap_start;
    Chunk chunk;

    FW_ASSERT(total); /* does it make sense to have a 0 byte file? */
    FW_ASSERT(start < total, start, total);

    /* simple case: there is no chunk data, which means there is a single gap of the entire size */
    if (!m_count)
    {
        chunk.offset = 0;
        chunk.size = total;
        if (callback)
        {
            callback(&chunk, opaque);
        }
        ret = 1;
    }
    else
    {
        /* Handle initial gap if needed */
        if (start < m_chunks[0].offset)
        {
            chunk.offset = start;
            chunk.size = m_chunks[0].offset - start;
            if (callback)
            {
                callback(&chunk, opaque);
            }
            ret = 1;
        }

        while ((ret < maxGaps) && (i < m_count))
        {
            next_off = (i == (m_count - 1)) ? total : m_chunks[i + 1].offset;
            gap_start = (m_chunks[i].offset + m_chunks[i].size);

            chunk.offset = (gap_start > start) ? gap_start : start;
            chunk.size = (next_off - chunk.offset);

            if (gap_start >= total)
            {
                break;
            }
            else if (start < next_off)
            {
                /* Only report if gap finishes after start */
                if (callback)
                {
                    callback(&chunk, opaque);
                }
                ++ret;
            }
            ++i;
        }
    }

    return ret;
}

void CfdpChunkList::insertChunk(ChunkIdx index, const Chunk* chunk)
{
    FW_ASSERT(m_count < m_maxChunks, m_count, m_maxChunks);
    FW_ASSERT(index <= m_count, index, m_count);

    if (m_count && (index != m_count))
    {
        memmove(&m_chunks[index + 1], &m_chunks[index],
                sizeof(*chunk) * (m_count - index));
    }
    memcpy(&m_chunks[index], chunk, sizeof(*chunk));

    ++m_count;
}

void CfdpChunkList::eraseChunk(ChunkIdx index)
{
    FW_ASSERT(m_count > 0);
    FW_ASSERT(index < m_count, index, m_count);

    /* to erase, move memory over the old one */
    memmove(&m_chunks[index], &m_chunks[index + 1],
            sizeof(*m_chunks) * (m_count - 1 - index));
    --m_count;
}

void CfdpChunkList::eraseRange(ChunkIdx start, ChunkIdx end)
{
    /* Sanity check */
    FW_ASSERT(end <= m_count, end, m_count);

    if (start < end)
    {
        memmove(&m_chunks[start], &m_chunks[end], sizeof(*m_chunks) * (m_count - end));
        m_count -= static_cast<ChunkIdx>(end - start);
    }
}

ChunkIdx CfdpChunkList::findInsertPosition(const Chunk* chunk)
{
    ChunkIdx first = 0;
    ChunkIdx i;
    ChunkIdx count = m_count;
    ChunkIdx step;

    while (count > 0)
    {
        i = first;
        step = count / 2;
        i += step;
        if (m_chunks[i].offset < chunk->offset)
        {
            first = i + 1;
            count -= static_cast<ChunkIdx>(step + 1);
        }
        else
        {
            count = step;
        }
    }

    return first;
}

bool CfdpChunkList::combineNext(ChunkIdx i, const Chunk* chunk)
{
    ChunkIdx combined_i = i;
    bool ret = false;
    FileSize chunk_end = chunk->offset + chunk->size;

    /* Assert no rollover, only possible as a bug */
    FW_ASSERT(chunk_end > chunk->offset, chunk_end, chunk->offset);

    /* Determine how many can be combined */
    for (; combined_i < m_count; ++combined_i)
    {
        /* Advance combine index until there is a gap between end and the next offset */
        if (chunk_end < m_chunks[combined_i].offset)
        {
            break;
        }
    }

    /* If index advanced the range of chunks can be combined */
    if (i != combined_i)
    {
        /* End is the max of last combined chunk end or new chunk end */
        chunk_end =
            CfdpChunkMax(m_chunks[combined_i - 1].offset + m_chunks[combined_i - 1].size, chunk_end);

        /* Use current slot as combined entry */
        m_chunks[i].size = chunk_end - chunk->offset;
        m_chunks[i].offset = chunk->offset;

        /* Erase the rest of the combined chunks (if any) */
        eraseRange(i + 1, combined_i);
        ret = true;
    }

    return ret;
}

bool CfdpChunkList::combinePrevious(ChunkIdx i, const Chunk* chunk)
{
    Chunk* prev;
    FileSize prev_end;
    FileSize chunk_end;
    bool ret = false;

    FW_ASSERT(i <= m_maxChunks, i, m_maxChunks);

    /* Only need to check if there is a previous */
    if (i > 0)
    {
        chunk_end = chunk->offset + chunk->size;
        prev = &m_chunks[i - 1];
        prev_end = prev->offset + prev->size;

        /* Check if start of new chunk is less than end of previous (overlaps) */
        if (chunk->offset <= prev_end)
        {
            /* When combining, use the bigger of the two endings */
            if (prev_end < chunk_end)
            {
                /* Combine with previous chunk */
                prev->size = chunk_end - prev->offset;
            }
            ret = true;
        }
    }
    return ret;
}

void CfdpChunkList::insert(ChunkIdx i, const Chunk* chunk)
{
    ChunkIdx smallest_i;
    Chunk* smallest_c;
    bool next = combineNext(i, chunk);
    bool combined;

    if (next)
    {
        combined = combinePrevious(i, &m_chunks[i]);
        if (combined)
        {
            eraseChunk(i);
        }
    }
    else
    {
        combined = combinePrevious(i, chunk);
        if (!combined)
        {
            if (m_count < m_maxChunks)
            {
                insertChunk(i, chunk);
            }
            else
            {
                smallest_i = findSmallestSize();
                smallest_c = &m_chunks[smallest_i];
                if (smallest_c->size < chunk->size)
                {
                    eraseChunk(smallest_i);
                    insertChunk(findInsertPosition(chunk), chunk);
                }
            }
        }
    }
}

ChunkIdx CfdpChunkList::findSmallestSize() const
{
    ChunkIdx i;
    ChunkIdx smallest = 0;

    for (i = 1; i < m_count; ++i)
    {
        if (m_chunks[i].size < m_chunks[smallest].size)
        {
            smallest = i;
        }
    }

    return smallest;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
