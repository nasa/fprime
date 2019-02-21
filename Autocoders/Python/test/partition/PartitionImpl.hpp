#ifndef PARTITION_PARTITION_IMPL_HPP
#define PARTITION_PARTITION_IMPL_HPP

#include <Autocoders/Python/test/partition/PartitionPartitionComponentAc.hpp>

namespace Partition {

	class PartitionImpl : public PartitionBase  {
	public:
		PartitionImpl(const char* compName);
		~PartitionImpl(void);

	private:
		// downcall for input port
		I32 inputPort1_Serialize_handler(Fw::SerializeBufferBase &Buffer);
		I32 inputPort2_Serialize_handler(Fw::SerializeBufferBase &Buffer);
	};

};

#endif
