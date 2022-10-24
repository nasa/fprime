#include <Autocoders/Python/test/partition/PartitionImpl.hpp>
#include <FpConfig.hpp>
#include <iostream>
#include <cstdio>

using namespace std;

namespace Partition {

		PartitionImpl::PartitionImpl(const char* compName) : PartitionBase(compName) {

		}

		PartitionImpl::~PartitionImpl() {

		}

		// downcall for input port
		I32 PartitionImpl::inputPort1_Serialize_handler(Fw::SerializeBufferBase &Buffer) {

			printf("\n\t\t*** %s: inputPort1_Serialize_handler down-call\n", this->m_objName);
			outputPort1_Serialize_out(Buffer);
			return 0;
		}

		I32 PartitionImpl::inputPort2_Serialize_handler(Fw::SerializeBufferBase &Buffer) {
			printf("\n\t\t*** %s: inputPort2_Serialize_handler down-call\n", this->m_objName);
			outputPort2_Serialize_out(Buffer);
			return 0;
		}
};

