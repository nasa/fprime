#include <Autocoders/Python/test/enum_xml/ExampleEnumImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Example {

		ExampleEnumImpl::ExampleEnumImpl(const char* compName) : Component1ComponentBase(compName) {

		}

		ExampleEnumImpl::~ExampleEnumImpl(void) {

		}

		void ExampleEnumImpl::init(NATIVE_INT_TYPE queueDepth) {
			Component1ComponentBase::init(queueDepth);
		}

		void ExampleEnumImpl:: ExEnumIn_handler(NATIVE_INT_TYPE portNum, Example::Enum1 enum1, Example::Serial1 serial1) {
			//printf("%s Invoked ExEnumIn_handler(%d, %d, %d, %d, %d, %d);\n", this->getObjName(), portNum, enum1, serial1.arg1, serial1.arg2, serial1.arg3);
			this->ExEnumOut_out(0, enum1, serial1);
		}
};
