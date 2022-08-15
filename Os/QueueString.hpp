#ifndef OS_QUEUE_STRING_TYPE_HPP
#define OS_QUEUE_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>

namespace Os {

    class QueueString : public Fw::StringBase {
        public:

            QueueString(const char* src); //!< char buffer constructor
            QueueString(const StringBase& src); //!< copy constructor
            QueueString(const QueueString& src); //!< copy constructor
            QueueString(); //!< default constructor
            QueueString& operator=(const QueueString& other); //!< assignment operator
            QueueString& operator=(const StringBase& other); //!< other string assignment operator
            QueueString& operator=(const char* other); //!< char* assignment operator
            ~QueueString(); //!< destructor

            const char* toChar() const; //!< get pointer to char buffer
            NATIVE_UINT_TYPE getCapacity() const ;

        private:
            char m_buf[FW_QUEUE_NAME_MAX_SIZE]; //!< buffer for string
    };
}

#endif
