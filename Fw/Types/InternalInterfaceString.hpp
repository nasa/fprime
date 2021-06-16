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
            ~InternalInterfaceString(void); //!< destructor
            const char* toChar(void) const; //!< gets char buffer
            NATIVE_UINT_TYPE length(void) const; //!< returns length of stored string

            const InternalInterfaceString& operator=(const InternalInterfaceString& other); //!< equal operator

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialization function
            SerializeStatus deserialize(SerializeBufferBase& buffer); //!< deserialization function

        PRIVATE:
            NATIVE_UINT_TYPE getCapacity(void) const ; //!< return buffer size
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_INTERNAL_INTERFACE_STRING_MAX_SIZE]; //!< storage for string data
};
}

#endif // FW_INTERNAL_INTERFACE_STRING_TYPE_HPP
