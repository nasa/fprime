#ifndef EXAMPLE_TYPE_HPP
#define EXAMPLE_TYPE_HPP

// A hand-coded serializable
#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Autocoders/Python/test/serialize_user/SomeStruct.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/StringType.hpp>
#endif


namespace ANameSpace {

    class UserSerializer : public Fw::Serializable {
    public:

        enum {
            SERIALIZED_SIZE = sizeof(SomeUserStruct) + sizeof(I32)
        };

        UserSerializer(); // Default constructor
        UserSerializer(const SomeUserStruct* src); // copy constructor
        UserSerializer(const SomeUserStruct& src); // copy constructor
        UserSerializer(SomeUserStruct arg); // constructor with arguments
        SomeUserStruct& operator=(const SomeUserStruct& src); // Equal operator

        void setVal(const SomeUserStruct& arg); // set values

        void getVal(SomeUserStruct& arg);

        Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const;
        Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer);
    #if FW_SERIALIZABLE_TO_STRING
        void toString(Fw::StringBase& text) const; //!< generate text from serializable
    #endif
    protected:

    private:

        SomeUserStruct m_struct; // stored value

    };
}

#endif
