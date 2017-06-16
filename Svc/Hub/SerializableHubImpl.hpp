#ifndef SVC_COMHUB_IMPL_HPP
#define SVC_COMHUB_IMPL_HPP

#include <Fw/Cfg/Config.hpp>


#include <Svc/Hub/CommHubComponentAc.hpp>

namespace Svc {

	class SerializableHubImpl : public CommHubBase  {
	public:

		// Only called by derived class
#if FW_OBJECT_NAMES	        
		SerializableHubImpl(const char* compName);
#else
        SerializableHubImpl(U32 buffSize);
#endif
		~SerializableHubImpl();

#if FW_PORT_SERIALIZATION == 1
        void SerIn_Serialize_handler(NATIVE_INT_TYPE portNum, Fw::SerializeBufferBase &Buffer);
#endif
        void DataIn_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data);


	private:

	};
	
}

#endif
