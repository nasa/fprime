#ifndef EXAMPLE_TYPE_HPP
#define EXAMPLE_TYPE_HPP

// A hand-coded serializable
#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/StringType.hpp>
#include <cstdio> // snprintf
#endif


namespace ANameSpace {

    class mytype : public Fw::Serializable {
    public:

        enum {
            SERIALIZED_SIZE = sizeof(U32),
            TYPE_ID = 2000
        };

        mytype(); // Default constructor
        mytype(const mytype* src); // copy constructor
        mytype(const mytype& src); // copy constructor
        mytype(U32 arg); // constructor with arguments
        mytype& operator=(const mytype& src); // Equal operator
        bool operator==(const mytype& src) const; // equality operator

        void setVal(U32 arg); // set values

        U32 getVal();

        Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const;
        Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer);
    #if FW_SERIALIZABLE_TO_STRING
        void toString(Fw::StringBase& text) const; //!< generate text from serializable
    #endif
    protected:

    private:

        U32 m_val; // stored value

    };
}

#endif
