// ======================================================================
// \title  TesterHandlers.cpp
// \author T. Chieu
// \brief  cpp file for tester handler functions
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_arrayArgsOut_handler(const NATIVE_INT_TYPE portNum,
                                        const FormalParamArray& a,
                                        FormalParamArray& aRef) {
    this->pushFromPortEntry_arrayArgsOut(a, aRef);
}

FormalParamArray Tester ::from_arrayReturnOut_handler(const NATIVE_INT_TYPE portNum,
                                                      const FormalParamArray& a,
                                                      FormalParamArray& aRef) {
    this->pushFromPortEntry_arrayReturnOut(a, aRef);
    return arrayReturnVal.val;
}

void Tester ::from_enumArgsOut_handler(const NATIVE_INT_TYPE portNum,
                                       const FormalParamEnum& en,
                                       FormalParamEnum& enRef) {
    this->pushFromPortEntry_enumArgsOut(en, enRef);
}

FormalParamEnum Tester ::from_enumReturnOut_handler(const NATIVE_INT_TYPE portNum,
                                                    const FormalParamEnum& en,
                                                    FormalParamEnum& enRef) {
    this->pushFromPortEntry_enumReturnOut(en, enRef);
    return enumReturnVal.val;
}

void Tester ::from_noArgsOut_handler(const NATIVE_INT_TYPE portNum) {
    this->pushFromPortEntry_noArgsOut();
}

bool Tester ::from_noArgsReturnOut_handler(const NATIVE_INT_TYPE portNum) {
    this->pushFromPortEntry_noArgsReturnOut();
    return noParamReturnVal.val;
}

void Tester ::from_primitiveArgsOut_handler(const NATIVE_INT_TYPE portNum,
                                            U32 u32,
                                            U32& u32Ref,
                                            F32 f32,
                                            F32& f32Ref,
                                            bool b,
                                            bool& bRef) {
    this->pushFromPortEntry_primitiveArgsOut(u32, u32Ref, f32, f32Ref, b, bRef);
}

U32 Tester ::from_primitiveReturnOut_handler(const NATIVE_INT_TYPE portNum,
                                             U32 u32,
                                             U32& u32Ref,
                                             F32 f32,
                                             F32& f32Ref,
                                             bool b,
                                             bool& bRef) {
    this->pushFromPortEntry_primitiveReturnOut(u32, u32Ref, f32, f32Ref, b, bRef);
    return primitiveReturnVal.val;
}

void Tester ::from_stringArgsOut_handler(const NATIVE_INT_TYPE portNum,
                                         const str80String& str80,
                                         str80RefString& str80Ref,
                                         const str100String& str100,
                                         str100RefString& str100Ref) {
    this->pushFromPortEntry_stringArgsOut(str80, str80Ref, str100, str100Ref);
}

void Tester ::from_structArgsOut_handler(const NATIVE_INT_TYPE portNum,
                                         const FormalParamStruct& s,
                                         FormalParamStruct& sRef) {
    this->pushFromPortEntry_structArgsOut(s, sRef);
}

FormalParamStruct Tester ::from_structReturnOut_handler(const NATIVE_INT_TYPE portNum,
                                                        const FormalParamStruct& s,
                                                        FormalParamStruct& sRef) {
    this->pushFromPortEntry_structReturnOut(s, sRef);
    return structReturnVal.val;
}

// ----------------------------------------------------------------------
// Handlers for serial from ports
// ----------------------------------------------------------------------

void Tester ::from_serialOut_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                                     Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
) {
    Fw::SerializeStatus status;

    switch (portNum) {
        case SerialPortIndex::NO_ARGS:
            status = Fw::FW_SERIALIZE_OK;
            break;

        case SerialPortIndex::PRIMITIVE:
            status = Buffer.copyRaw(this->primitiveBuf, Buffer.getBuffCapacity());
            break;

        case SerialPortIndex::STRING:
            status = Buffer.copyRaw(this->stringBuf, Buffer.getBuffCapacity());
            break;

        case SerialPortIndex::ENUM:
            status = Buffer.copyRaw(this->enumBuf, Buffer.getBuffCapacity());
            break;

        case SerialPortIndex::ARRAY:
            status = Buffer.copyRaw(this->arrayBuf, Buffer.getBuffCapacity());
            break;

        case SerialPortIndex::STRUCT:
            status = Buffer.copyRaw(this->structBuf, Buffer.getBuffCapacity());
            break;
    }

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
}

// ----------------------------------------------------------------------
// Handlers for special from ports
// ----------------------------------------------------------------------

void Tester ::cmdResponseIn(const FwOpcodeType opCode, const U32 cmdSeq, const Fw::CmdResponse response) {
    this->cmdResp = response;
}
