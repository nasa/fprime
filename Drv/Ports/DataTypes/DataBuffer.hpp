#ifndef _DrvDataBuffer_hpp_
#define _DrvDataBuffer_hpp_

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Drv {

//! Fixed-size buffer for driver data transfer
class DataBuffer : public Fw::SerializeBufferBase {
  public:
    enum {
        DATA_BUFFER_SIZE = 256,          //!< Buffer capacity in bytes
        SERIALIZED_TYPE_ID = 1010,       //!< Type ID for serialization
        SERIALIZED_SIZE = DATA_BUFFER_SIZE + sizeof(FwBuffSizeType)  //!< Total serialized size
    };

    //! Construct from existing data
    DataBuffer(const U8* args, FwSizeType size);
    //! Default constructor
    DataBuffer();
    //! Copy constructor
    DataBuffer(const DataBuffer& other);
    //! Destructor
    virtual ~DataBuffer();
    //! Assignment operator
    DataBuffer& operator=(const DataBuffer& other);

    //! Get buffer capacity
    FwSizeType getBuffCapacity() const;
    //! Get buffer address
    U8* getBuffAddr();
    //! Get const buffer address
    const U8* getBuffAddr() const;

  private:
    U8 m_data[DATA_BUFFER_SIZE];  //!< Internal data storage
};
}  // namespace Drv

#endif
