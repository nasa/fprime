#ifndef FW_EIGHTY_CHAR_STRING_TYPE_HPP
#define FW_EIGHTY_CHAR_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class EightyCharString : public Fw::StringBase {
        public:
        
            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_EIGHTY_CHAR_STRING, //!< typeid for string type
                STRING_SIZE = 80, //!< Storage for string
                SERIALIZED_SIZE = STRING_SIZE + sizeof(FwBuffSizeType) //!< Serialized size is size of buffer + size field
            };
        
            EightyCharString(const char* src); //!< char* source constructor
            EightyCharString(const StringBase& src); //!< other string constructor
            EightyCharString(const EightyCharString& src); //!< EightyCharString string constructor
            EightyCharString(void); //!< default constructor
            ~EightyCharString(void); //!< destructor
            const char* toChar(void) const; //!< gets char buffer
            NATIVE_UINT_TYPE length(void) const; //!< returns length of stored string

            const EightyCharString& operator=(const EightyCharString& other); //!< equal operator
            
            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialization function
            SerializeStatus deserialize(SerializeBufferBase& buffer); //!< deserialization function
            
        PRIVATE:
            void copyBuff(const char* buff, NATIVE_UINT_TYPE size); //!< copy source buffer, overwriting
            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[STRING_SIZE]; //!< storage for string data
    };
}

#endif
