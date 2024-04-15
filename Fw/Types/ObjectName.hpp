#ifndef FW_OBJECT_NAME_TYPE_HPP
#define FW_OBJECT_NAME_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Cfg/SerIds.hpp>
#include <Fw/Types/StringType.hpp>

namespace Fw {

class ObjectName : public Fw::StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_OBJECT_NAME,             //!< typeid for string type
        STRING_SIZE = FW_OBJ_NAME_MAX_SIZE,                     //!< Storage for string
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwBuffSizeType)  //!< Serialized size is size of buffer + size field
    };

    explicit ObjectName(const CHAR* src);            //!< char* source constructor
    explicit ObjectName(const StringBase& src);      //!< StringBase string constructor
    ObjectName(const ObjectName& src);               //!< ObjectName string constructor
    ObjectName();                                    //!< default constructor
    ObjectName& operator=(const ObjectName& other);  //!< assignment operator
    ObjectName& operator=(const StringBase& other);  //!< StringBase string assignment operator
    ObjectName& operator=(const CHAR* other);        //!< char* assignment operator
    ~ObjectName();                                   //!< destructor

    const CHAR* toChar() const;            //!< gets char buffer
    NATIVE_UINT_TYPE getCapacity() const;  //!< return buffer size

  private:
    CHAR m_buf[STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif
