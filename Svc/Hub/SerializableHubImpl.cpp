
#include <Svc/Hub/SerializableHubImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/Log.hpp>
#include <stdio.h>

namespace Svc {

    
#if FW_OBJECT_NAMES == 1    
	SerializableHubImpl::SerializableHubImpl(const char* compName) :
        CommHubBase(compName) {
#else
    SerializableHubImpl::SerializableHubImpl() :
        CommHubBase() {
#endif
	}
	
	SerializableHubImpl::~SerializableHubImpl(void) {
	
	}
	
#if FW_PORT_SERIALIZATION == 1
    void SerializableHubImpl::SerIn_Serialize_handler(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer) {
        Fw::SerializeStatus status;
        Fw::ComBuffer buff;

        status = buff.serialize(Buffer);
        // The call in should be internal, so assert on failure
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(status));
        status = buff.serialize(portNum);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(status));

        this->DataOut_out(0,buff);
    }

#endif

    void SerializableHubImpl::DataIn_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data) {

#if FW_PORT_SERIALIZATION == 1
        Fw::SerializeStatus status;

        // deserialize source port number
        U32 sourcePort;
        status = data.deserialize(sourcePort);
        if (status != Fw::FW_SERIALIZE_OK) {
            (void)printf("SerializableHubImpl: unable to deserialize port: %d\n",status);
        }

        // deserialize buffer
        Fw::ComBuffer buff;
        status = data.deserialize(buff);

        if (status != Fw::FW_SERIALIZE_OK) {
            (void)printf("SerializableHubImpl: unable to deserialize buffer: %d\n",status);
        }

        // call output port
        this->SerOut_out(sourcePort,buff);

#else

    // to make it compile, just call output com port

    this->DataOut_out(portNum,data);

#endif
    }

}
