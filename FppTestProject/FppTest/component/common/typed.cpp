#ifndef TestComponentName
#error "TestComponentName must be defined before typed.cpp is included"
#endif

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Fw::String TestComponentName ::stringReturnGuarded_handler(FwIndexType portNum,
                                                           const Fw::StringBase& str,
                                                           Fw::StringBase& strRef) {
    return this->stringReturnOut_out(portNum, str, strRef);
}

void TestComponentName ::enumArgsSync_handler(const FwIndexType portNum,
                                              const FormalParamEnum& en,
                                              FormalParamEnum& enRef,
                                              const FormalAliasEnum& enA,
                                              FormalAliasEnum& enARef) {
    this->enumArgsOut_out(portNum, en, enRef, enA, enARef);
}

Fw::String TestComponentName ::stringReturnSync_handler(FwIndexType portNum,
                                                        const Fw::StringBase& str,
                                                        Fw::StringBase& strRef) {
    return this->stringReturnOut_out(portNum, str, strRef);
}

FormalAliasString TestComponentName ::stringAliasReturnGuarded_handler(FwIndexType portNum,
                                                                       const Fw::StringBase& str,
                                                                       Fw::StringBase& strRef) {
    return this->stringAliasReturnOut_out(portNum, str, strRef);
}

FormalAliasString TestComponentName ::stringAliasReturnSync_handler(FwIndexType portNum,
                                                                    const Fw::StringBase& str,
                                                                    Fw::StringBase& strRef) {
    return this->stringAliasReturnOut_out(portNum, str, strRef);
}

void TestComponentName ::arrayArgsGuarded_handler(const FwIndexType portNum,
                                                  const FormalParamArray& a,
                                                  FormalParamArray& aRef,
                                                  const FormalAliasArray& b,
                                                  FormalAliasArray& bRef,
                                                  const FormalAliasStringArray& c,
                                                  FormalAliasStringArray& cRef) {
    this->arrayArgsOut_out(portNum, a, aRef, b, bRef, c, cRef);
}

void TestComponentName ::arrayArgsSync_handler(const FwIndexType portNum,
                                               const FormalParamArray& a,
                                               FormalParamArray& aRef,
                                               const FormalAliasArray& b,
                                               FormalAliasArray& bRef,
                                               const FormalAliasStringArray& c,
                                               FormalAliasStringArray& cRef) {
    this->arrayArgsOut_out(portNum, a, aRef, b, bRef, c, cRef);
}

FormalParamArray TestComponentName ::arrayReturnGuarded_handler(const FwIndexType portNum,
                                                                const FormalParamArray& a,
                                                                FormalParamArray& aRef) {
    return this->arrayReturnOut_out(portNum, a, aRef);
}

FormalParamArray TestComponentName ::arrayReturnSync_handler(const FwIndexType portNum,
                                                             const FormalParamArray& a,
                                                             FormalParamArray& aRef) {
    return this->arrayReturnOut_out(portNum, a, aRef);
}

FormalAliasStringArray TestComponentName ::arrayStringAliasReturnGuarded_handler(const FwIndexType portNum,
                                                                                 const FormalParamArray& a,
                                                                                 FormalParamArray& aRef) {
    return this->arrayStringAliasReturnOut_out(portNum, a, aRef);
}

FormalAliasStringArray TestComponentName ::arrayStringAliasReturnSync_handler(const FwIndexType portNum,
                                                                              const FormalParamArray& a,
                                                                              FormalParamArray& aRef) {
    return this->arrayStringAliasReturnOut_out(portNum, a, aRef);
}

void TestComponentName ::enumArgsGuarded_handler(const FwIndexType portNum,
                                                 const FormalParamEnum& en,
                                                 FormalParamEnum& enRef,
                                                 const FormalAliasEnum& enA,
                                                 FormalAliasEnum& enARef) {
    this->enumArgsOut_out(portNum, en, enRef, enA, enARef);
}

FormalParamEnum TestComponentName ::enumReturnGuarded_handler(const FwIndexType portNum,
                                                              const FormalParamEnum& en,
                                                              FormalParamEnum& enRef) {
    return this->enumReturnOut_out(portNum, en, enRef);
}

FormalParamEnum TestComponentName ::enumReturnSync_handler(const FwIndexType portNum,
                                                           const FormalParamEnum& en,
                                                           FormalParamEnum& enRef) {
    return this->enumReturnOut_out(portNum, en, enRef);
}

void TestComponentName ::noArgsGuarded_handler(const FwIndexType portNum) {
    this->noArgsOut_out(portNum);
}

bool TestComponentName ::noArgsReturnGuarded_handler(const FwIndexType portNum) {
    return this->noArgsReturnOut_out(portNum);
}

bool TestComponentName ::noArgsReturnSync_handler(const FwIndexType portNum) {
    return this->noArgsReturnOut_out(portNum);
}

void TestComponentName ::noArgsSync_handler(const FwIndexType portNum) {
    this->noArgsOut_out(portNum);
}

void TestComponentName ::primitiveArgsGuarded_handler(const FwIndexType portNum,
                                                      U32 u32,
                                                      U32& u32Ref,
                                                      F32 f32,
                                                      F32& f32Ref,
                                                      bool b,
                                                      bool& bRef) {
    this->primitiveArgsOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
}

void TestComponentName ::primitiveArgsSync_handler(const FwIndexType portNum,
                                                   U32 u32,
                                                   U32& u32Ref,
                                                   F32 f32,
                                                   F32& f32Ref,
                                                   bool b,
                                                   bool& bRef) {
    this->primitiveArgsOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
}

U32 TestComponentName ::primitiveReturnGuarded_handler(const FwIndexType portNum,
                                                       U32 u32,
                                                       U32& u32Ref,
                                                       F32 f32,
                                                       F32& f32Ref,
                                                       bool b,
                                                       bool& bRef) {
    return this->primitiveReturnOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
}

U32 TestComponentName ::primitiveReturnSync_handler(const FwIndexType portNum,
                                                    U32 u32,
                                                    U32& u32Ref,
                                                    F32 f32,
                                                    F32& f32Ref,
                                                    bool b,
                                                    bool& bRef) {
    return this->primitiveReturnOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
}

void TestComponentName ::stringArgsGuarded_handler(const FwIndexType portNum,
                                                   const Fw::StringBase& str80,
                                                   Fw::StringBase& str80Ref,
                                                   const Fw::StringBase& str100,
                                                   Fw::StringBase& str100Ref) {
    this->stringArgsOut_out(portNum, str80, str80Ref, str100, str100Ref);
}

void TestComponentName ::stringArgsSync_handler(const FwIndexType portNum,
                                                const Fw::StringBase& str80,
                                                Fw::StringBase& str80Ref,
                                                const Fw::StringBase& str100,
                                                Fw::StringBase& str100Ref) {
    this->stringArgsOut_out(portNum, str80, str80Ref, str100, str100Ref);
}

void TestComponentName ::structArgsGuarded_handler(const FwIndexType portNum,
                                                   const FormalParamStruct& s,
                                                   FormalParamStruct& sRef) {
    this->structArgsOut_out(portNum, s, sRef);
}

void TestComponentName ::structArgsSync_handler(const FwIndexType portNum,
                                                const FormalParamStruct& s,
                                                FormalParamStruct& sRef) {
    this->structArgsOut_out(portNum, s, sRef);
}

FormalParamStruct TestComponentName ::structReturnGuarded_handler(const FwIndexType portNum,
                                                                  const FormalParamStruct& s,
                                                                  FormalParamStruct& sRef) {
    return this->structReturnOut_out(portNum, s, sRef);
}

FormalParamStruct TestComponentName ::structReturnSync_handler(const FwIndexType portNum,
                                                               const FormalParamStruct& s,
                                                               FormalParamStruct& sRef) {
    return this->structReturnOut_out(portNum, s, sRef);
}
