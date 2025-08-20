void TestComponentName ::arrayArgsAsync_handler(const FwIndexType portNum,
                                                const FormalParamArray& a,
                                                FormalParamArray& aRef,
                                                const FormalAliasArray& b,
                                                FormalAliasArray& bRef,
                                                const FormalAliasStringArray& c,
                                                FormalAliasStringArray& cRef) {
    this->arrayArgsOut_out(portNum, a, aRef, b, bRef, c, cRef);
}

void TestComponentName ::enumArgsAsync_handler(const FwIndexType portNum,
                                               const FormalParamEnum& en,
                                               FormalParamEnum& enRef,
                                               const FormalAliasEnum& enA,
                                               FormalAliasEnum& enARef) {
    this->enumArgsOut_out(portNum, en, enRef, enA, enARef);
}

void TestComponentName ::noArgsAsync_handler(const FwIndexType portNum) {
    this->noArgsOut_out(portNum);
}

void TestComponentName ::primitiveArgsAsync_handler(const FwIndexType portNum,
                                                    U32 u32,
                                                    U32& u32Ref,
                                                    F32 f32,
                                                    F32& f32Ref,
                                                    bool b,
                                                    bool& bRef) {
    this->primitiveArgsOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
}

void TestComponentName ::structArgsAsync_handler(const FwIndexType portNum,
                                                 const FormalParamStruct& s,
                                                 FormalParamStruct& sRef) {
    this->structArgsOut_out(portNum, s, sRef);
}

void TestComponentName ::stringArgsAsync_handler(const FwIndexType portNum,
                                                 const Fw::StringBase& str80,
                                                 Fw::StringBase& str80Ref,
                                                 const Fw::StringBase& str100,
                                                 Fw::StringBase& str100Ref) {
    this->stringArgsOut_out(portNum, str80, str80Ref, str100, str100Ref);
}

void TestComponentName ::enumArgsHook_handler(const FwIndexType portNum,
                                              const FormalParamEnum& en,
                                              FormalParamEnum& enRef,
                                              const FormalAliasEnum& enA,
                                              FormalAliasEnum& enARef) {}