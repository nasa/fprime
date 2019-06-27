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
			printf("Component received input.");
			// this->m_ExEnumOut_OutputPort[0](portNum, cmd, str);
		}
};
