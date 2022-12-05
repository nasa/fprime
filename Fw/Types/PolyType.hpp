#ifndef FW_POLY_TYPE_HPP
#define FW_POLY_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class PolyType : public Serializable {
        public:

            PolyType(U8 val); //!< U8 constructor
            operator U8(); //!< U8 cast operator
            void get(U8& val); //!< U8 accessor
            bool isU8(); //!< U8 checker
            PolyType& operator=(U8 val); //!< U8 operator=

            PolyType(I8 val); //!< I8 constructor
            operator I8(); //!< I8 cast operator
            void get(I8& val); //!< I8 accessor
            bool isI8(); //!< I8 checker
            PolyType& operator=(I8 val); //!< I8 operator=

#if FW_HAS_16_BIT
            PolyType(U16 val); //!< U16 constructor
            operator U16(); //!< U16 cast operator
            void get(U16& val); //!< U16 accessor
            bool isU16(); //!< U16 checker
            PolyType& operator=(U16 val); //!< I8 operator=

            PolyType(I16 val); //!< I16 constructor
            operator I16(); //!< I16 cast operator
            void get(I16& val); //!< I16 accessor
            bool isI16(); //!< I16 checker
            PolyType& operator=(I16 val); //!< I16 operator=
#endif
#if FW_HAS_32_BIT
            PolyType(U32 val); //!< U32 constructor
            operator U32(); //!< U32 cast operator
            void get(U32& val); //!< U32 accessor
            bool isU32(); //!< U32 checker
            PolyType& operator=(U32 val); //!< U32 operator=

            PolyType(I32 val); //!< I32 constructor
            operator I32(); //!< I32 cast operator
            void get(I32& val); //!< I32 accessor
            bool isI32(); //!< I32 checker
            PolyType& operator=(I32 val); //!< I32 operator=
#endif
#if FW_HAS_64_BIT
            PolyType(U64 val); //!< U64 constructor
            operator U64(); //!< U64 cast operator
            void get(U64& val); //!< U64 accessor
            bool isU64(); //!< U64 checker
            PolyType& operator=(U64 val); //!< U64 operator=

            PolyType(I64 val); //!< I64 constructor
            operator I64(); //!< I64 cast operator
            void get(I64& val); //!< I64 accessor
            bool isI64(); //!< I64 checker
            PolyType& operator=(I64 val); //!< I64 operator=
#endif

#if FW_HAS_F64
            PolyType(F64 val); //!< F64 constructor
            operator F64(); //!< F64 cast operator
            void get(F64& val); //!< F64 accessor
            bool isF64(); //!< F64 checker
            PolyType& operator=(F64 val); //!< F64 operator=
#endif
            PolyType(F32 val); //!< F32 constructor
            operator F32(); //!< F32 cast operator
            void get(F32& val); //!< F32 accessor
            bool isF32(); //!< F32 checker
            PolyType& operator=(F32 val); //!< F32 operator=

            PolyType(bool val); //!< bool constructor
            operator bool(); //!< bool cast operator
            void get(bool& val); //!< bool accessor
            bool isBool(); //!< bool checker
            PolyType& operator=(bool val); //!< bool operator=

            PolyType(void* val); //!< void* constructor.
            operator void*(); //!< void* cast operator
            void get(void*& val); //!< void* accessor
            bool isPtr(); //!< void* checker
            PolyType& operator=(void* val); //!< void* operator=

            PolyType(); //!< default constructor
            PolyType(const PolyType &original); //!< copy constructor
            virtual ~PolyType(); //!< destructor

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
            void toString(StringBase& dest, bool append) const; //!< get string representation
            void toString(StringBase& dest) const; //!< get string representation
#endif

            PolyType& operator=(const PolyType &src); //!< PolyType operator=
            bool operator<(const PolyType &other) const; //!< PolyType operator<
            bool operator>(const PolyType &other) const; //!< PolyType operator>
            bool operator>=(const PolyType &other) const; //!< PolyType operator>=
            bool operator<=(const PolyType &other) const; //!< PolyType operator<=
            bool operator==(const PolyType &other) const; //!< PolyType operator==
            bool operator!=(const PolyType &other) const; //!< PolyType operator!=
            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< Serialize function
            SerializeStatus deserialize(SerializeBufferBase& buffer); //!< Deserialize function

        PRIVATE:

            typedef enum {
                TYPE_NOTYPE, // !< No type stored yet
                TYPE_U8, // !< U8 type stored
                TYPE_I8, // !< I8 type stored
#if FW_HAS_16_BIT
                TYPE_U16, // !< U16 type stored
                TYPE_I16, // !< I16 type stored
#endif
#if FW_HAS_32_BIT
                TYPE_U32, // !< U32 type stored
                TYPE_I32, // !< I32 type stored
#endif
#if FW_HAS_64_BIT
                TYPE_U64, // !< U64 type stored
                TYPE_I64, // !< I64 type stored
#endif
                TYPE_F32, // !< F32 type stored
#if FW_HAS_F64
                TYPE_F64, // !< F64 type stored
#endif
                TYPE_BOOL, // !< bool type stored
                TYPE_PTR // !< pointer type stored
            } Type;

            Type m_dataType; //!< member that indicates type being stored

            union PolyVal {
                    U8 u8Val; //!< U8 data storage
                    I8 i8Val; //!< I8 data storage
#if FW_HAS_16_BIT
                    U16 u16Val; //!< U16 data storage
                    I16 i16Val; //!< I16 data storage
#endif
#if FW_HAS_32_BIT
                    U32 u32Val; //!< U32 data storage
                    I32 i32Val; //!< I32 data storage
#endif
#if FW_HAS_64_BIT
                    U64 u64Val; //!< U64 data storage
                    I64 i64Val; //!< I64 data storage
#endif
#if FW_HAS_F64
                    F64 f64Val; //!< F64 data storage
#endif
                    F32 f32Val; // !< F32 data storage
                    void* ptrVal; // !< pointer data storage
                    bool boolVal; // !< bool data storage
            } m_val; // !< stores data value

        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_POLY, //!< typeid for PolyType
                SERIALIZED_SIZE = sizeof(FwEnumStoreType) + sizeof(PolyVal) //!< stored serialized size
            };


    };

}

#endif
