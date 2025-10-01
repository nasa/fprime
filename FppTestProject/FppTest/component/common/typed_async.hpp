//! Handler implementation for arrayArgsAsync
void arrayArgsAsync_handler(FwIndexType portNum,              //!< The port number
                            const FormalParamArray& a,        //!< An array
                            FormalParamArray& aRef,           //!< An array ref
                            const FormalAliasArray& b,        //!< alias of an array
                            FormalAliasArray& bRef,           //!< alias of array ref
                            const FormalAliasStringArray& c,  //!< array of string aliases
                            FormalAliasStringArray& cRef      //!< array of string aliases ref
                            ) override;

//! Handler implementation for enumArgsAsync
void enumArgsAsync_handler(FwIndexType portNum,         //!< The port number
                           const FormalParamEnum& en,   //!< An enum
                           FormalParamEnum& enRef,      //!< An enum ref
                           const FormalAliasEnum& enA,  //!< An enum alias
                           FormalAliasEnum& enARef      //!< An enum alias ref
                           ) override;

//! Handler implementation for noArgsAsync
void noArgsAsync_handler(FwIndexType portNum  //!< The port number
                         ) override;

//! Handler implementation for primitiveArgsAsync
void primitiveArgsAsync_handler(FwIndexType portNum,  //!< The port number
                                U32 u32,
                                U32& u32Ref,
                                F32 f32,
                                F32& f32Ref,
                                bool b,
                                bool& bRef) override;

//! Handler implementation for stringArgsAsync
void stringArgsAsync_handler(FwIndexType portNum,          //!< The port number
                             const Fw::StringBase& str80,  //!< A string of size 80
                             Fw::StringBase& str80Ref,
                             const Fw::StringBase& str100,  //!< A string of size 100
                             Fw::StringBase& str100Ref) override;

//! Handler implementation for structArgsAsync
void structArgsAsync_handler(FwIndexType portNum,         //!< The port number
                             const FormalParamStruct& s,  //!< A struct
                             FormalParamStruct& sRef      //!< A struct ref
                             ) override;

//! Handler implementation for enumArgsOverflow
void enumArgsHook_handler(FwIndexType portNum,         //!< The port number
                          const FormalParamEnum& en,   //!< An enum
                          FormalParamEnum& enRef,      //!< An enum ref
                          const FormalAliasEnum& enA,  //!< An enum alias
                          FormalAliasEnum& enARef      //!< An enum alias ref
                          ) override;
