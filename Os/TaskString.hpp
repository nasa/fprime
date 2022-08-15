#ifndef OS_TASK_STRING_TYPE_HPP
#define OS_TASK_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/StringType.hpp>

namespace Os {

    class TaskString : public Fw::StringBase {
        public:

            TaskString(const char* src); //!< char buffer constructor
            TaskString(const StringBase& src); //!< Copy constructor
            TaskString(const TaskString& src); //!< Copy constructor
            TaskString(); //!< default constructor
            TaskString& operator=(const TaskString& other); //!< assignment operator
            TaskString& operator=(const StringBase& other); //!< other string assignment operator
            TaskString& operator=(const char* other); //!< char* assignment operator
            ~TaskString(); //!< destructor

            const char* toChar() const; //!< get pointer to internal char buffer
            NATIVE_UINT_TYPE getCapacity() const; //!< return buffer size

        private:

            char m_buf[FW_TASK_NAME_MAX_SIZE]; //!< buffer for string

    };

}

#endif
