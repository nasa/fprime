#ifndef EXAMPLE_ARRAY_IMPL_HPP
#define EXAMPLE_ARRAY_IMPL_HPP

#include <Fw/Types/EightyCharString.hpp>
#include <Autocoders/Python/test/array_xml/Component1ComponentAc.hpp>

namespace Example {

	class ExampleArrayImpl : public Component1ComponentBase  {
    public:

		// Only called by derived class
		ExampleArrayImpl(const char* compName);
		~ExampleArrayImpl();
		void init(NATIVE_INT_TYPE queueDepth);

    private:
        void ExArrayIn_handler(NATIVE_INT_TYPE portNum, Example::ArrayType array1, Example::ArrSerial serial1);
        void ArrayIn_handler(NATIVE_INT_TYPE portNum, Example::ArrayType array1, Example::ArrSerial serial1);
	};

};

#endif
