/*
 * Buffer.hpp
 *
 *  Created on: Wednesday, 04 November 2020
 *  Author:     mstarch
 *
 */
#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#if FW_SERIALIZABLE_TO_STRING
#include <Fw/Types/StringType.hpp>
#include <stdio.h> // snprintf
#ifdef BUILD_UT
#include <iostream>
#include <Fw/Types/EightyCharString.hpp>
#endif
#endif

namespace Fw {

class Buffer : public Fw::Serializable {


public:

    enum {
        SERIALIZED_SIZE =
        sizeof(U32) +
        sizeof(U32) +
        sizeof(U64) +
        sizeof(U32)
    }; //!< serializable size of Buffer

    Buffer(void); //!< Default constructor
    Buffer(const Buffer* src); //!< pointer copy constructor
    Buffer(const Buffer& src); //!< reference copy constructor
    Buffer(U32 managerID, U32 bufferID, U64 data, U32 size); //!< constructor with arguments
    const Buffer& operator=(const Buffer& src); //!< equal operator
    bool operator==(const Buffer& src) const; //!< equality operator
#ifdef BUILD_UT
    // to support GoogleTest framework in unit tests
    friend std::ostream& operator<<(std::ostream& os, const Buffer& obj);
#endif

    void set(U32 managerID, U32 bufferID, U64 data, U32 size); //!< set values

    U32 getmanagerID(void); //!< get member managerID
    U32 getbufferID(void); //!< get member bufferID
    U64 getdata(void); //!< get member data
    U32 getsize(void); //!< get member size

    void setmanagerID(U32 val); //!< set member managerID
    void setbufferID(U32 val); //!< set member bufferID
    void setdata(U64 val); //!< set member data
    void setsize(U32 val); //!< set member size


    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const; //!< serialization function
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer); //!< deserialization function
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    void toString(Fw::StringBase& text) const; //!< generate text from serializable
#endif
protected:

    enum {
        TYPE_ID = 0xA4583D9C //!< type id
    };

    U32 m_managerID; //<! managerID - The ID of the buffer manager that allocated this buffer
    U32 m_bufferID; //<! bufferID - The ID of this buffer
    U64 m_data; //<! data - A pointer to the data
    U32 m_size; //<! size - The data size in bytes
private:

};
} // end namespace Fw
#endif /* BUFFER_HPP_ */

