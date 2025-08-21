// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

//! Handler implementation for arrayArgsGuarded
void arrayArgsGuarded_handler(FwIndexType portNum,              //!< The port number
                              const FormalParamArray& a,        //!< An array
                              FormalParamArray& aRef,           //!< An array ref
                              const FormalAliasArray& b,        //!< alias of an array
                              FormalAliasArray& bRef,           //!< alias of array ref
                              const FormalAliasStringArray& c,  //!< array of string aliases
                              FormalAliasStringArray& cRef      //!< array of string aliases ref
                              ) override;

//! Handler implementation for arrayArgsSync
void arrayArgsSync_handler(FwIndexType portNum,              //!< The port number
                           const FormalParamArray& a,        //!< An array
                           FormalParamArray& aRef,           //!< An array ref
                           const FormalAliasArray& b,        //!< alias of an array
                           FormalAliasArray& bRef,           //!< alias of array ref
                           const FormalAliasStringArray& c,  //!< array of string aliases
                           FormalAliasStringArray& cRef      //!< array of string aliases ref
                           ) override;

//! Handler implementation for arrayReturnGuarded
FormalParamArray arrayReturnGuarded_handler(FwIndexType portNum,        //!< The port number
                                            const FormalParamArray& a,  //!< An array
                                            FormalParamArray& aRef      //!< An array ref
                                            ) override;

//! Handler implementation for arrayReturnSync
FormalParamArray arrayReturnSync_handler(FwIndexType portNum,        //!< The port number
                                         const FormalParamArray& a,  //!< An array
                                         FormalParamArray& aRef      //!< An array ref
                                         ) override;

//! Handler implementation for arrayReturnGuarded
FormalAliasStringArray arrayStringAliasReturnGuarded_handler(FwIndexType portNum,        //!< The port number
                                                             const FormalParamArray& a,  //!< A string
                                                             FormalParamArray& aRef      //!< A string ref
                                                             ) override;

//! Handler implementation for arrayReturnSync
FormalAliasStringArray arrayStringAliasReturnSync_handler(FwIndexType portNum,        //!< The port number
                                                          const FormalParamArray& a,  //!< A string
                                                          FormalParamArray& aRef      //!< A string ref
                                                          ) override;

//! Handler implementation for enumArgsGuarded
void enumArgsGuarded_handler(FwIndexType portNum,         //!< The port number
                             const FormalParamEnum& en,   //!< An enum
                             FormalParamEnum& enRef,      //!< An enum ref
                             const FormalAliasEnum& enA,  //!< An enum alias
                             FormalAliasEnum& enARef      //!< An enum alias ref
                             ) override;

//! Handler implementation for enumArgsSync
void enumArgsSync_handler(FwIndexType portNum,         //!< The port number
                          const FormalParamEnum& en,   //!< An enum
                          FormalParamEnum& enRef,      //!< An enum ref
                          const FormalAliasEnum& enA,  //!< An enum alias
                          FormalAliasEnum& enARef      //!< An enum alias ref
                          ) override;

//! Handler implementation for enumReturnGuarded
FormalParamEnum enumReturnGuarded_handler(FwIndexType portNum,        //!< The port number
                                          const FormalParamEnum& en,  //!< An enum
                                          FormalParamEnum& enRef      //!< An enum ref
                                          ) override;

//! Handler implementation for enumReturnSync
FormalParamEnum enumReturnSync_handler(FwIndexType portNum,        //!< The port number
                                       const FormalParamEnum& en,  //!< An enum
                                       FormalParamEnum& enRef      //!< An enum ref
                                       ) override;

//! Handler implementation for noArgsGuarded
void noArgsGuarded_handler(FwIndexType portNum  //!< The port number
                           ) override;

//! Handler implementation for noArgsReturnGuarded
bool noArgsReturnGuarded_handler(FwIndexType portNum  //!< The port number
                                 ) override;

//! Handler implementation for noArgsReturnSync
bool noArgsReturnSync_handler(FwIndexType portNum  //!< The port number
                              ) override;

//! Handler implementation for noArgsSync
void noArgsSync_handler(FwIndexType portNum  //!< The port number
                        ) override;

//! Handler implementation for primitiveArgsGuarded
void primitiveArgsGuarded_handler(FwIndexType portNum,  //!< The port number
                                  U32 u32,
                                  U32& u32Ref,
                                  F32 f32,
                                  F32& f32Ref,
                                  bool b,
                                  bool& bRef) override;

//! Handler implementation for primitiveArgsSync
void primitiveArgsSync_handler(FwIndexType portNum,  //!< The port number
                               U32 u32,
                               U32& u32Ref,
                               F32 f32,
                               F32& f32Ref,
                               bool b,
                               bool& bRef) override;

//! Handler implementation for primitiveReturnGuarded
U32 primitiveReturnGuarded_handler(FwIndexType portNum,  //!< The port number
                                   U32 u32,
                                   U32& u32Ref,
                                   F32 f32,
                                   F32& f32Ref,
                                   bool b,
                                   bool& bRef) override;

//! Handler implementation for primitiveReturnSync
U32 primitiveReturnSync_handler(FwIndexType portNum,  //!< The port number
                                U32 u32,
                                U32& u32Ref,
                                F32 f32,
                                F32& f32Ref,
                                bool b,
                                bool& bRef) override;

//! Handler implementation for stringArgsGuarded
void stringArgsGuarded_handler(FwIndexType portNum,          //!< The port number
                               const Fw::StringBase& str80,  //!< A string of size 80
                               Fw::StringBase& str80Ref,
                               const Fw::StringBase& str100,  //!< A string of size 100
                               Fw::StringBase& str100Ref) override;

//! Handler implementation for stringArgsSync
void stringArgsSync_handler(FwIndexType portNum,          //!< The port number
                            const Fw::StringBase& str80,  //!< A string of size 80
                            Fw::StringBase& str80Ref,
                            const Fw::StringBase& str100,  //!< A string of size 100
                            Fw::StringBase& str100Ref) override;

//! Handler implementation for structArgsGuarded
void structArgsGuarded_handler(FwIndexType portNum,         //!< The port number
                               const FormalParamStruct& s,  //!< A struct
                               FormalParamStruct& sRef      //!< A struct ref
                               ) override;

//! Handler implementation for stringReturnGuarded
Fw::String stringReturnGuarded_handler(FwIndexType portNum,        //!< The port number
                                       const Fw::StringBase& str,  //!< A string
                                       Fw::StringBase& strRef      //!< A string ref
                                       ) override;

//! Handler implementation for stringReturnSync
Fw::String stringReturnSync_handler(FwIndexType portNum,        //!< The port number
                                    const Fw::StringBase& str,  //!< A string
                                    Fw::StringBase& strRef      //!< A string ref
                                    ) override;

//! Handler implementation for stringAliasReturnGuarded
FormalAliasString stringAliasReturnGuarded_handler(FwIndexType portNum,        //!< The port number
                                                   const Fw::StringBase& str,  //!< A string
                                                   Fw::StringBase& strRef      //!< A string ref
                                                   ) override;

//! Handler implementation for stringAliasReturnSync
FormalAliasString stringAliasReturnSync_handler(FwIndexType portNum,        //!< The port number
                                                const Fw::StringBase& str,  //!< A string
                                                Fw::StringBase& strRef      //!< A string ref
                                                ) override;

//! Handler implementation for structArgsSync
void structArgsSync_handler(FwIndexType portNum,         //!< The port number
                            const FormalParamStruct& s,  //!< A struct
                            FormalParamStruct& sRef      //!< A struct ref
                            ) override;

//! Handler implementation for structReturnGuarded
FormalParamStruct structReturnGuarded_handler(FwIndexType portNum,         //!< The port number
                                              const FormalParamStruct& s,  //!< A struct
                                              FormalParamStruct& sRef      //!< A struct ref
                                              ) override;

//! Handler implementation for structReturnSync
FormalParamStruct structReturnSync_handler(FwIndexType portNum,         //!< The port number
                                           const FormalParamStruct& s,  //!< A struct
                                           FormalParamStruct& sRef      //!< A struct ref
                                           ) override;
