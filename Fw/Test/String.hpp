#ifndef TEST_STRING_TYPE_HPP
#define TEST_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Test {

    //! A longer string for testing
    class String : public Fw::StringBase {
        public:

            enum {
                STRING_SIZE = 256, //!< Storage for string
                SERIALIZED_SIZE = STRING_SIZE + sizeof(FwBuffSizeType) //!< Serialized size is size of buffer + size field
            };

            String(const char* src); //!< char* source constructor
            String(const StringBase& src); //!< other string constructor
            String(const String& src); //!< String string constructor
            String(); //!< default constructor
            String& operator=(const String& other); //!< assignment operator
            String& operator=(const StringBase& other); //!< other string assignment operator
            String& operator=(const char* other); //!< char* assignment operator
            ~String(); //!< destructor

            const char* toChar() const; //!< gets char buffer
            NATIVE_UINT_TYPE getCapacity() const ; //!< return buffer size

        private:

            char m_buf[STRING_SIZE]; //!< storage for string data
    };
}

#endif
