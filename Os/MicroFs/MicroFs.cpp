#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/File.hpp>
#include <Os/MicroFs/MicroFs.hpp>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <Utils/Hash/libcrc/lib_crc.h>  // borrow CRC

#ifdef __cplusplus
}
#endif  // __cplusplus

#include <cstdio>
#include <cstring>
#include <limits>

namespace Os {

struct MicroFsFileState {
    PlatformIntType loc;       //!< location in file where last operation left off
    PlatformIntType currSize;  //!< current size of the file after writes were done
    PlatformIntType dataSize;  //!< alloted size of the file
    BYTE* data;                //!< location of file data
};

STATIC void* MicroFsMem = 0;
// offset from zero for fds to allow zero checks
STATIC const PlatformIntType MICROFS_FD_OFFSET = 1;

void MicroFsInit(const MicroFsConfig& cfg, const PlatformUIntType id, Fw::MemAllocator& allocator) {
    // check things...
    FW_ASSERT(cfg.numBins <= MAX_MICROFS_BINS, cfg.numBins, MAX_MICROFS_BINS);
    // avoid re-init
    FW_ASSERT(0 == MicroFsMem);

    // compute the amount of memory needed to hold the file system state
    // and data

    // initialize it to the config struct size
    PlatformUIntType memSize = sizeof(MicroFsConfig);
    PlatformUIntType totalNumFiles = 0;
    // iterate through the bins
    for (PlatformUIntType bin = 0; bin < cfg.numBins; bin++) {
        // memory needed is struct for file state + file buffer size
        memSize += cfg.bins[bin].numFiles * (sizeof(MicroFsFileState) + cfg.bins[bin].fileSize);
        totalNumFiles += cfg.bins[bin].numFiles;
    }

    // request the memory
    PlatformUIntType reqMem = memSize;
    bool dontcare;
    MicroFsMem = allocator.allocate(id, reqMem, dontcare);

    // make sure got the amount requested.
    // improvement could be best effort based on received memory
    FW_ASSERT(reqMem == memSize, reqMem, memSize);
    // make sure we got a non-null pointer
    FW_ASSERT(MicroFsMem);

    // copy the configuration structure into the memory
    MicroFsConfig* cfgPtr = static_cast<MicroFsConfig*>(MicroFsMem);
    *cfgPtr = cfg;

    // lay out the memory with the state and the buffers after the config section
    MicroFsFileState* statePtr = reinterpret_cast<MicroFsFileState*>(&cfgPtr[1]);

    // point to memory after state structs for beginning of file data
    BYTE* currFileBuff = reinterpret_cast<BYTE*>(&statePtr[totalNumFiles]);
    // fill in the file state structs
    for (PlatformUIntType bin = 0; bin < cfg.numBins; bin++) {
        for (PlatformUIntType file = 0; file < cfg.bins[bin].numFiles; file++) {
            // clear state structure memory
            memset(statePtr, 0, sizeof(MicroFsFileState));
            // initialize state
            statePtr->loc = -1;                           // no operation in progress
            statePtr->currSize = -1;                      // nothing written yet
            statePtr->data = currFileBuff;                // point to data for the file
            statePtr->dataSize = cfg.bins[bin].fileSize;  // store allocated size for file data
#if MICROFS_INIT_FILE_DATA
            ::memset(currFileBuff, 0, cfg.bins[bin].fileSize);
#endif
            // advance file data pointer
            currFileBuff += cfg.bins[bin].fileSize;
            // advance file state pointer
            statePtr += 1;
        }
    }
}

// helper to find file state entry from file name. Will return index if found, -1 if not
STATIC PlatformIntType getFileStateIndex(const CHAR* fileName) {
    // the directory/filename rule is very strict - it has to be /MICROFS_BIN_STRING<n>/MICROFS_FILE_STRING<m>,
    // where n = number of file bins, and m = number of files in a particular bin
    // any other name will return an error
    PlatformUIntType bin = 0;
    PlatformUIntType file = 0;
    MicroFsFileState* statePtr = 0;

    const char* filePathSpec = "/" 
        MICROFS_BIN_STRING 
        "%d/" 
        MICROFS_FILE_STRING 
        "%d";

    PlatformIntType binIndex;
    PlatformIntType fileIndex;

    PlatformIntType stat = sscanf(fileName,filePathSpec,binIndex,fileIndex);
    // must match two entries, i.e. the bin and file numbers
    if (stat != 2) {
        return -1;
    }

    // get number of bins
    MicroFsConfig* cfgPtr = static_cast<MicroFsConfig*>(MicroFsMem);
    FW_ASSERT(cfgPtr);

    // check to see that indexes don't exceed config
    if (binIndex >= cfgPtr->numBins) {
        return -1;
    }

    if (fileIndex >= cfgPtr->bins[binIndex].numFiles) {
        return -1;
    }    

    PlatformIntType stateIndex = 0;
    // compute file state index

    // add each chunk of file numbers from full bins
    for (PlatformIntType currBin = 0; currBin < binIndex; binIndex++) {
        stateIndex += cfgPtr->bins[currBin].numFiles;
    }

    // get residual file number from last bin
    stateIndex += fileIndex;

    return stateIndex;
}

// helper to get state pointer from index
STATIC MicroFsFileState* getFileStateFromIndex(PlatformIntType index) {
    // should be non-zero by the time this is called
    FW_ASSERT(index > 0, index);
    FW_ASSERT(MicroFsMem);
    // Get base of state structures
    MicroFsConfig* cfg = reinterpret_cast<MicroFsConfig*>(MicroFsMem);
    MicroFsFileState* ptr = reinterpret_cast<MicroFsFileState*>(&cfg[1]);
    return &ptr[index];
}

File::File() : m_fd(-1), m_mode(OPEN_NO_MODE), m_lastError(0) {}

File::~File() {
    if (this->m_mode != OPEN_NO_MODE) {
        this->close();
    }
}

File::Status File::open(const char* fileName, File::Mode mode) {
    return this->open(fileName, mode, true);
}
File::Status File::open(const char* fileName, File::Mode mode, bool include_excl) {
    Status stat = OP_OK;

    // common checks
    // retrieve index to file entry
    PlatformIntType entry = getFileStateIndex(fileName);
    // not found
    if (-1 == entry) {
        return File::Status::DOESNT_EXIST;
    }

    MicroFsFileState* state = getFileStateFromIndex(entry);
    FW_ASSERT(state);

    // store mode
    this->m_mode = mode;

    switch (mode) {
        case OPEN_READ:
            // if not written to yet, doesn't exist for read
            if (-1 == state->currSize) {
                return File::Status::DOESNT_EXIST;
            }
            // if file is already open, return an error
            if (state->loc != -1) {
                return File::Status::NO_PERMISSION;
            }
            // initialize operation location to zero
            state->loc = 0;
            // set file descriptor to index into state structure
            this->m_fd = entry + MICROFS_FD_OFFSET;
            return OP_OK;
        case OPEN_WRITE:
        case OPEN_SYNC_WRITE:         // fall through; same for microfs
        case OPEN_SYNC_DIRECT_WRITE:  // fall through; same for microfs
            // if the file doesn't exist yet, WRITE shouldn't work. Needs CREATE
            if (-1 == state->currSize) {
                return File::Status::DOESNT_EXIST;
            }
            // if file is already open, return an error
            if (state->loc != -1) {
                return File::Status::NO_PERMISSION;
            }
            // initialize write location to zero
            state->loc = 0;
            // set file descriptor to index into state structure
            this->m_fd = entry + MICROFS_FD_OFFSET;
            return OP_OK;
        case OPEN_CREATE:
            // same as write, but can create, so zero file length is okay

            // if file is already open, return an error
            if (state->loc != -1) {
                return File::Status::NO_PERMISSION;
            }
            // initialize write location to zero
            state->loc = 0;
            // CREATE restarts file, initialize file size to zero
            state->currSize = 0;
            // set file descriptor to index into state structure
            this->m_fd = entry + MICROFS_FD_OFFSET;
            return OP_OK;
        case OPEN_APPEND:
            // if the file doesn't exist yet, WRITE shouldn't work. Needs CREATE
            if (-1 == state->currSize) {
                return File::Status::DOESNT_EXIST;
            }
            // if file is already open for write, return an error
            if (state->loc != -1) {
                return File::Status::NO_PERMISSION;
            }
            // initialize write location to length of file for append
            state->loc = state->currSize;
            // set file descriptor to index into state structure
            this->m_fd = entry + MICROFS_FD_OFFSET;
            return OP_OK;
        default:
            FW_ASSERT(0, mode);
            break;
    }

    return stat;
}

bool File::isOpen() {
    return (this->m_fd > 0);
}

File::Status File::prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len) {
    // do nothing; in RAM
    return OP_OK;
}

File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {
    // make sure it has been opened
    if (OPEN_NO_MODE == this->m_mode) {
        return NOT_OPENED;
    }

    NATIVE_INT_TYPE newOffset = 0;

    // get file state entry
    FW_ASSERT(this->m_fd != -1);
    MicroFsFileState* state = getFileStateFromIndex(this->m_fd - MICROFS_FD_OFFSET);
    FW_ASSERT(state);

    // compute new operation location
    if (absolute) {
        // make sure not too far
        if (offset >= state->dataSize) {
            return BAD_SIZE;
        }
        state->loc = offset;
    } else {
        // make sure not too far
        if (state->loc + offset >= state->dataSize) {
            return BAD_SIZE;
        }
        state->loc = state->loc + offset;
    }

    // move current size as well if needed
    if (state->loc > state->currSize) {
        state->currSize = state->loc;
    }
    return OP_OK;
}

File::Status File::read(void* buffer, NATIVE_INT_TYPE& size, bool waitForFull) {
    FW_ASSERT(buffer);

    // make sure it has been opened
    if (OPEN_NO_MODE == this->m_mode) {
        size = 0;
        return NOT_OPENED;
    }

    // get file state entry
    FW_ASSERT(this->m_fd != -1);
    MicroFsFileState* state = getFileStateFromIndex(this->m_fd - MICROFS_FD_OFFSET);
    FW_ASSERT(state);

    // find size to copy

    // check to see if already at the end of the file. If so, return 0 for size
    if (state->loc == state->currSize - 1) {
        size = 0;
        return OP_OK;
    }

    // copy requested bytes, unless it would be more than the file size.
    // If it would be more than the file size, copy the remainder and set
    // the size to the actual copied
    if (state->loc + size > state->currSize - 1) {
        size = state->currSize - state->loc;
    }

    // copy data from location to buffer
    (void)memcpy(buffer, state->data + state->loc, size);

    return OP_OK;
}

File::Status File::write(const void* buffer, NATIVE_INT_TYPE& size, bool waitForDone) {
    FW_ASSERT(buffer);

    // make sure it has been opened in write mode
    switch (this->m_mode) {
        case OPEN_NO_MODE:
            return NOT_OPENED;
        case OPEN_WRITE:  // all fall through for write cases
        case OPEN_SYNC_WRITE:
        case OPEN_SYNC_DIRECT_WRITE:
        case OPEN_CREATE:
        case OPEN_APPEND:
            break;
        default:
            size = 0;
            return OTHER_ERROR;
    }

    // get file state entry
    FW_ASSERT(this->m_fd != -1);
    MicroFsFileState* state = getFileStateFromIndex(this->m_fd - MICROFS_FD_OFFSET);
    FW_ASSERT(state);

    // write up to the end of the allocated buffer
    // if write size is greater, truncate the write
    // and set size to what was actually written
    if (state->loc + size > state->dataSize) {
        size = state->dataSize - state->loc;
    }

    // copy data to file buffer
    memcpy(&state->data[state->loc], buffer, size);
    // increment new file size
    state->currSize += size;

    return OP_OK;
}

File::Status File::bulkWrite(const void* buffer, NATIVE_UINT_TYPE& totalSize, NATIVE_INT_TYPE chunkSize) {
    // make sure it has been opened
    if (OPEN_NO_MODE == this->m_mode) {
        return NOT_OPENED;
    }

    // RAM, so use write call
    NATIVE_INT_TYPE writeSize = totalSize;
    return write(buffer, writeSize, true);
}

File::Status File::flush() {
    // make sure it has been opened
    if (OPEN_NO_MODE == this->m_mode) {
        return NOT_OPENED;
    }

    // RAM, so no need to flush
    return OP_OK;
}

void File::close() {
    if (this->m_fd != -1) {
        // get state to clear it
        MicroFsFileState* state = getFileStateFromIndex(this->m_fd - MICROFS_FD_OFFSET);
        FW_ASSERT(state);
        state->loc = -1;
    }
    // reset fd
    this->m_fd = -1;
    this->m_mode = OPEN_NO_MODE;
    // clear
}

NATIVE_INT_TYPE File::getLastError() {
    return this->m_lastError;
}

const char* File::getLastErrorString() {
    return strerror(this->m_lastError);
}

File::Status File::calculateCRC32(U32& crc) {
    // make sure it has been opened
    if (OPEN_NO_MODE == this->m_mode) {
        crc = 0;
        return NOT_OPENED;
    }

    const U32 maxChunkSize = 32;
    const U32 initialSeed = 0xFFFFFFFF;

    // Seek to beginning of file
    Status status = seek(0, true);
    if (status != OP_OK) {
        crc = 0;
        return status;
    }

    U8 file_buffer[maxChunkSize];

    bool endOfFile = false;

    U32 seed = initialSeed;
    const U32 maxIters = std::numeric_limits<U32>::max();  // loop limit
    U32 numIters = 0;

    while (!endOfFile && numIters < maxIters) {
        ++numIters;
        int chunkSize = maxChunkSize;

        status = read(file_buffer, chunkSize, false);
        if (status == OP_OK) {
            // chunkSize modified by file.read

            if (chunkSize == 0) {
                endOfFile = true;
                continue;
            }

            int chunkIdx = 0;

            while (chunkIdx < chunkSize) {
                seed = update_crc_32(seed, file_buffer[chunkIdx]);
                chunkIdx++;
            }

        } else {
            crc = 0;
            return status;
        }
    }

    if (!endOfFile) {
        crc = 0;
        return OTHER_ERROR;
    } else {
        crc = seed;
        return OP_OK;
    }
}
}  // namespace Os
