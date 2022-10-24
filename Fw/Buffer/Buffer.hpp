// ======================================================================
// \title  Buffer.hpp
// \author mstarch
// \brief  hpp file for Fw::Buffer definition
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#if FW_SERIALIZABLE_TO_STRING
    #include <Fw/Types/StringType.hpp>
    #include <cstdio> // snprintf
    #ifdef BUILD_UT
        #include <iostream>
        #include <Fw/Types/String.hpp>
    #endif
#endif

namespace Fw {

//! Buffer used for wrapping pointer to data for efficient transmission
//!
//! Fw::Buffer is a wrapper for a pointer to data. It allows for data to be passed around the system without a copy of
//! the data itself. However, it comes with the expectation that the user maintain and protect this memory as it moves
//! about the system until such a time as it is returned.
//!
//! Fw::Buffer is composed of several elements: a U8* pointer to the data, a U32 size of that data, and a U32 context
//! describing the origin of that data, such that it may be freed at some later point. The default context of 0xFFFFFFFF
//! should not be used for tracking purposes, as it represents a context-free buffer.
//!
//! Fw::Buffer also comes with functions to return a representation of the data as a SerializeBufferBase. These two
//! functions allow easy access to the data as if it were a serialize or deserialize buffer. This can aid in writing and
//! reading the wrapped data whereas the standard serialize and deserialize methods treat the data as a pointer to
//! prevent excessive copying.
//!
class Buffer : public Fw::Serializable {

public:

    enum {
        SERIALIZED_SIZE = sizeof(U32) + sizeof(U32) + sizeof(U8*), //!< Size of Fw::Buffer when serialized
        NO_CONTEXT = 0xFFFFFFFF //!< Value representing no context
    };

    //! Construct a buffer with no context nor data
    //!
    //! Constructs a buffer setting the context to the default no-context value of 0xffffffff. In addition, the size
    //! and data pointers are zeroed-out.
    Buffer();

    //! Construct a buffer by copying members from a reference to another buffer. Does not copy wrapped data.
    //!
    Buffer(const Buffer& src);

    //! Construct a buffer to wrap the given data pointer of given size
    //!
    //! Wraps the given data pointer with given size in a buffer. The context by default is set to NO_CONTEXT but can
    //! be set to specify a specific context.
    //! \param data: data pointer to wrap
    //! \param size: size of data located at data pointer
    //! \param context: user-specified context to track creation. Default: no context
    Buffer(U8* data, U32 size, U32 context=NO_CONTEXT);

    //! Assignment operator to set given buffer's members from another without copying wrapped data
    //!
    Buffer& operator=(const Buffer& src);

    //! Equality operator returning true when buffers are equivalent
    //!
    //! Buffers are deemed equivalent if they contain a pointer to the same data, with the same size, and the same
    //! context. The representation of that buffer for use with serialization and deserialization need not be
    //! equivalent.
    //! \param src: buffer to test against
    //! \return: true if equivalent, false otherwise
    bool operator==(const Buffer& src) const;

    // ----------------------------------------------------------------------
    // Serialization functions
    // ----------------------------------------------------------------------

    //! Returns a SerializeBufferBase representation of the wrapped data for serializing
    //!
    //! Returns a SerializeBufferBase representation of the wrapped data allowing for serializing other types of data
    //! to the wrapped buffer. Once obtained the user should call one of two functions: `sbb.resetSer();` to setup for
    //! serialization, or `sbb.setBuffLen(buffer.getSize());` to setup for deserializing.
    //! \return representation of the wrapped data to aid in serializing to it
    SerializeBufferBase& getSerializeRepr();

    //! Serializes this buffer to a SerializeBufferBase
    //!
    //! This serializes the buffer to a SerializeBufferBase, however, it DOES NOT serialize the wrapped data. It only
    //! serializes the pointer to said data, the size, and context. This is done for efficiency in moving around data,
    //! and is the primary usage of Fw::Buffer. To serialize the wrapped data, use either the data pointer accessor
    //! or the serialize buffer base representation and serialize from that.
    //! \param serialBuffer: serialize buffer to write data into
    //! \return: status of serialization
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& serialBuffer) const;

    //! Deserializes this buffer from a SerializeBufferBase
    //!
    //! This deserializes the buffer from a SerializeBufferBase, however, it DOES NOT handle serialized data. It only
    //! deserializes the pointer to said data, the size, and context. This is done for efficiency in moving around data,
    //! and is the primary usage of Fw::Buffer. To deserialize the wrapped data, use either the data pointer accessor
    //! or the serialize buffer base representation and deserialize from that.
    //! \param buffer: serialize buffer to read data into
    //! \return: status of serialization
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer);


    // ----------------------------------------------------------------------
    // Accessor functions
    // ----------------------------------------------------------------------

    //! Returns wrapped data pointer
    //!
    U8* getData() const;

    //! Returns size of wrapped data
    //!
    U32 getSize() const;

    //! Returns creation context
    //!
    U32 getContext() const;

    //! Sets pointer to wrapped data and the size of the given data
    //!
    void setData(U8* data);

    //! Sets pointer to wrapped data and the size of the given data
    //!
    void setSize(U32 size);

    //! Sets creation context
    //!
    void setContext(U32 context);

    //! Sets all values
    //! \param data: data pointer to wrap
    //! \param size: size of data located at data pointer
    //! \param context: user-specified context to track creation. Default: no context
    void set(U8* data, U32 size, U32 context=NO_CONTEXT);

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    //! Supports writing this buffer to a string representation
    void toString(Fw::StringBase& text) const;
#endif

#ifdef BUILD_UT
    //! Supports GTest framework for outputting this type to a stream
    //!
    friend std::ostream& operator<<(std::ostream& os, const Buffer& obj);
#endif

PRIVATE:
    Fw::ExternalSerializeBuffer m_serialize_repr; //<! Representation for serialization and deserialization functions
    U8* m_bufferData; //<! data - A pointer to the data
    U32 m_size; //<! size - The data size in bytes
    U32 m_context; //!< Creation context for disposal

};
} // end namespace Fw
#endif /* BUFFER_HPP_ */

