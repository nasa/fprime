/*
 * FileBuffer.hpp
 *
 *  Created on: May 2014
 *      Author: tcanham
 */

/*
 * Description:
 * This object contains the FileBuffer type, used for storing a piece of an uplinked file
 */
#ifndef FW_FILE_BUFFER_HPP
#define FW_FILE_BUFFER_HPP

#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class FileBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_FILE_BUFF,
                SERIALIZED_SIZE = FW_FILE_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            FileBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            FileBuffer();
            FileBuffer(const FileBuffer& other);
            virtual ~FileBuffer();
            const FileBuffer& operator=(const FileBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity(void) const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr(void);
            const U8* getBuffAddr(void) const;

        private:
            U8 m_data[FW_FILE_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
