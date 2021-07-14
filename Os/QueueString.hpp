#ifndef OS_QUEUE_STRING_TYPE_HPP
#define OS_QUEUE_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <FpConfig.hpp>

namespace Os {

    class QueueString : public Fw::StringBase {
        public:

            QueueString(const char* src); //!< char buffer constructor
            QueueString(const StringBase& src); //!< copy constructor
            QueueString(const QueueString& src); //!< copy constructor
            QueueString(); //!< default constructor
            ~QueueString(); //!< destructor
            const char* toChar() const; //!< get pointer to char buffer
            NATIVE_UINT_TYPE length() const; //!< get current length of string

            const QueueString& operator=(const QueueString& other); //!< equal operator

        private:
            Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const { return Fw::FW_SERIALIZE_NO_ROOM_LEFT; } //!< disabled
            Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer)  { return Fw::FW_SERIALIZE_NO_ROOM_LEFT; } //!< disabled
            NATIVE_UINT_TYPE getCapacity() const ;
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string
            char m_buf[FW_QUEUE_NAME_MAX_SIZE]; //!< buffer for string
    };

}

#endif
