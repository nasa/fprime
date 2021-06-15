#ifndef OS_TASK_STRING_TYPE_HPP
#define OS_TASK_STRING_TYPE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/StringType.hpp>
#include <FpConfig.hpp>

namespace Os {

    class TaskString : public Fw::StringBase {
        public:

            TaskString(const char* src); //!< char buffer constructor
            TaskString(const StringBase& src); //!< Copy constructor
            TaskString(const TaskString& src); //!< Copy constructor
            TaskString(void); //!< default constructor
            ~TaskString(void); //!< destructor
            const char* toChar(void) const; //!< get pointer to internal char buffer
            NATIVE_UINT_TYPE length(void) const; //!< return current string length

            Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const;
            Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer);

            const TaskString& operator=(const TaskString& other); //!< equal operator

        private:
            NATIVE_UINT_TYPE getCapacity(void) const ;
            void terminate(NATIVE_UINT_TYPE size); //!< terminate the string

            char m_buf[FW_TASK_NAME_MAX_SIZE]; //!< buffer for string

    };

}

#endif
