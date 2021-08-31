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
            QueueString(void); //!< default constructor
            QueueString& operator=(const QueueString& other); //!< assignment operator
            QueueString& operator=(const StringBase& other); //!< other string assignment operator
            QueueString& operator=(const char* other); //!< char* assignment operator
            ~QueueString(void); //!< destructor

            const char* toChar(void) const; //!< get pointer to char buffer
            NATIVE_UINT_TYPE getCapacity(void) const ;

        private:

            char m_buf[FW_QUEUE_NAME_MAX_SIZE]; //!< buffer for string
    };
}

#endif
