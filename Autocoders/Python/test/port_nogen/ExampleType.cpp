#include <Autocoders/Python/test/port_nogen/ExampleType.hpp>
#include <Fw/Types/Assert.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/EightyCharString.hpp>
#endif
namespace ANameSpace {

mytype::mytype(void): Serializable() {

}

mytype::mytype(const mytype& src) : Serializable() {
    this->setVal(src.m_val);
}

mytype::mytype(const mytype* src) : Serializable() {
    FW_ASSERT(src);
    this->setVal(src->m_val);
}

mytype::mytype(U32 val) : Serializable() {
    this->setVal(val);
}

const mytype& mytype::operator=(const mytype& src) {
    this->setVal(src.m_val);
    return src;
}

bool mytype::operator==(const mytype& src) const {
    return (this->m_val == src.m_val);
}

U32 mytype::getVal(void) {
    return this->m_val;
}

void mytype::setVal(U32 val) {
    this->m_val = val;
}

Fw::SerializeStatus mytype::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::SerializeStatus stat = buffer.serialize(this->m_val);
    if (Fw::FW_SERIALIZE_OK != stat) {
        return stat;
    }
    return buffer.serialize((NATIVE_INT_TYPE)mytype::TYPE_ID);
}

Fw::SerializeStatus mytype::deserialize(Fw::SerializeBufferBase& buffer) {

    NATIVE_INT_TYPE id;
    Fw::SerializeStatus stat = buffer.deserialize(id);
    if (Fw::FW_SERIALIZE_OK != stat) {
        return stat;
    }

    if (id != (NATIVE_INT_TYPE)mytype::TYPE_ID) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    return buffer.deserialize(this->m_val);
}



#if FW_SERIALIZABLE_TO_STRING

void mytype::toString(Fw::StringBase& text) const {

    static const char * formatString = "(val = %u)";

    // declare strings to hold any serializable toString() arguments


    char outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE];
    snprintf(outputString,FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE,formatString,this->m_val);
    outputString[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE-1] = 0; // NULL terminate

    text = outputString;
}

#endif
}
