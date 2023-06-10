#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC

#ifdef __cplusplus
}
#endif // __cplusplus

namespace Os {
    File::Status File::niceCRC32(U32 &crc, const char* fileName)
    {
        //Constants used in this function
        const U32 CHUNK_SIZE = 4096;
        const U32 INITIAL_SEED = 0xFFFFFFFF;
        const U32 MAX_IT = 0xFFFFFFFF; //Max int for U32
        //Loop variables for calculating CRC
        NATIVE_INT_TYPE offset = 0;
        U32 seed = INITIAL_SEED;
        Status status;
        File file;
        U8 file_buffer[CHUNK_SIZE];
        bool eof = false;
        //Loop across the whole file
        for (U32 i = 0; !eof && i < MAX_IT; i++) {
            //Open and check status
            status = file.open(fileName, OPEN_READ);
            if (status != OP_OK) {
                crc = 0;
                return status;
            }
            //Find our place
            status = file.seek(offset, true);
            if (status != OP_OK) {
                crc = 0;
                return status;
            }
            NATIVE_INT_TYPE chunk = CHUNK_SIZE;
            //Read data and check status
            status = file.read(file_buffer, chunk, false);
            offset += chunk;
            //Close file, then update CRC. This reduces time file is required open
            file.close();
            if (chunk != 0 && status == OP_OK) {
                for (U32 ci = 0; static_cast<NATIVE_INT_TYPE>(ci) < chunk; ci++) {
                    seed = update_crc_32(seed, file_buffer[ci]);
                }
            } else if (chunk == 0 && status == OP_OK) {
                eof = true;
                break;
            } else {
                crc = 0;
                return status;
            }
        }
        //Reach max-loop
        if (!eof) {
            crc = 0;
            return OTHER_ERROR;
        }
        //Good CRC
        crc = seed;
        return OP_OK;
    }
}
