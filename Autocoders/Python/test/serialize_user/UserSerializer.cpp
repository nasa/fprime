#include <Autocoders/Python/test/serialize_user/UserSerializer.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

namespace ANameSpace {

UserSerializer::UserSerializer(void): Serializable() {

}

UserSerializer::UserSerializer(const SomeUserStruct& src) : Serializable() {
    this->setVal(src);
}

UserSerializer::UserSerializer(const SomeUserStruct* src) : Serializable() {
    FW_ASSERT(src);
    this->setVal(*src);
}

UserSerializer::UserSerializer(SomeUserStruct val) : Serializable() {
    this->setVal(val);
}

const SomeUserStruct& UserSerializer::operator=(const SomeUserStruct& src) {
    this->setVal(src);
    return src;
}

void UserSerializer::getVal(SomeUserStruct& arg) {
    arg = this->m_struct;
}

void UserSerializer::setVal(const SomeUserStruct& val) {
    this->m_struct = val;
}

Fw::SerializeStatus UserSerializer::serialize(Fw::SerializeBufferBase& buffer) const {
    return buffer.serialize((U8*)&m_struct,sizeof(m_struct));
}

Fw::SerializeStatus UserSerializer::deserialize(Fw::SerializeBufferBase& buffer) {
    NATIVE_UINT_TYPE serSize = sizeof(m_struct);
    Fw::SerializeStatus stat =  buffer.deserialize((U8*)&m_struct,serSize);
    FW_ASSERT(serSize == sizeof(m_struct));
    return stat;
}



#if FW_SERIALIZABLE_TO_STRING

void UserSerializer::toString(Fw::StringBase& text) const {

    // declare strings to hold any serializable toString() arguments

    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,
            "SomeUserStruct: %d %f %d",
            this->m_struct.mem1,this->m_struct.mem2,this->m_struct.mem3);
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

    text = outputString;
}

#endif
}
