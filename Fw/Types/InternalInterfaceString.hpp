#ifndef FW_INTERNAL_INTERFACE_STRING_TYPE_HPP
#define FW_INTERNAL_INTERFACE_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>
#include <FpConfig.hpp>

namespace Fw {

    class InternalInterfaceString : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_INTERNAL_INTERFACE_STRING, //!< typeid for string type
                SERIALIZED_SIZE = FW_INTERNAL_INTERFACE_STRING_MAX_SIZE + sizeof(FwBuffSizeType) //!< Serialized size is size of buffer + size field
            };

            InternalInterfaceString(const char* src); //!< char* source constructor
            InternalInterfaceString(const StringBase& src); //!< other string constructor
            InternalInterfaceString(const InternalInterfaceString& src); //!< other string constructor
            InternalInterfaceString(void); //!< default constructor
            InternalInterfaceString& operator=(const InternalInterfaceString& other); //!< assignment operator
            InternalInterfaceString& operator=(const StringBase& other); //!< other string assignment operator
            InternalInterfaceString& operator=(const char* other); //!< char* assignment operator
            ~InternalInterfaceString(void); //!< destructor

            const char* toChar(void) const; //!< gets char buffer
            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size

        private:

            char m_buf[FW_INTERNAL_INTERFACE_STRING_MAX_SIZE]; //!< storage for string data
    };
}

#endif // FW_INTERNAL_INTERFACE_STRING_TYPE_HPP
