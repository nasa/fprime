#ifndef FW_EIGHTY_CHAR_STRING_TYPE_HPP
#define FW_EIGHTY_CHAR_STRING_TYPE_HPP

#include <FpConfig.hpp>
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
            EightyCharString(); //!< default constructor
            EightyCharString& operator=(const EightyCharString& other); //!< assignment operator
            EightyCharString& operator=(const StringBase& other); //!< other string assignment operator
            EightyCharString& operator=(const char* other); //!< char* assignment operator
            ~EightyCharString(); //!< destructor

            const char* toChar() const; //!< gets char buffer
            NATIVE_UINT_TYPE getCapacity() const; //!< return buffer size

        private:

            char m_buf[STRING_SIZE]; //!< storage for string data
    };
}

#endif
