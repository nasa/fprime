#ifndef FW_PRM_STRING_TYPE_HPP
#define FW_PRM_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class ParamString : public Fw::StringBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_PRM_STR,
                SERIALIZED_SIZE = FW_PARAM_STRING_MAX_SIZE + sizeof(FwBuffSizeType) // size of buffer + storage of two size words
            };

            ParamString(const char* src);
            ParamString(const StringBase& src);
            ParamString(const ParamString& src);
            ParamString();
            ParamString& operator=(const ParamString& other);
            ParamString& operator=(const StringBase& other);
            ParamString& operator=(const char* other);
            ~ParamString();

            const char* toChar() const;
            NATIVE_UINT_TYPE getCapacity() const;

        private:

            char m_buf[FW_PARAM_STRING_MAX_SIZE];
    };

}

#endif
