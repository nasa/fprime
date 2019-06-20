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

		// Internal call - implemented by hand.
		// downcall for output port OutputPort1
		void ExampleEnumImpl:: OutputPort1_handler(NATIVE_INT_TYPE portNum, Example::Enum1 enum1, Example::Serial1 serial1) {
			printf("*** %d %s: OutputPort1_Port1_handler down-call\n", portNum, this->m_objName);
			// this->m_ExEnumOut_OutputPort[0](portNum, cmd, str);
		}

		// downcall for input port InputPort1
		void ExampleEnumImpl::InputPort1_handler(NATIVE_INT_TYPE portNum, Example::Enum1 enum1, Example::Serial1 serial1) {
			printf("*** %d %s: InputPort1_Port1_handler down-call\n", portNum, this->m_objName);
			// printf("*** %s: arg1 = %d arg2 = %s\n", this->m_objName, cmd, str.toChar());
		}
};
