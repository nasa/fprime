#ifndef EXAMPLE_ARRAY_IMPL_HPP
#define EXAMPLE_ARRAY_IMPL_HPP

#include <Autocoders/Python/test/array_xml/Component1ComponentAc.hpp>

namespace Example {

	class ExampleArrayImpl : public Component1ComponentBase  {
    public:

		// Only called by derived class
		ExampleArrayImpl(const char* compName);
		~ExampleArrayImpl();
		void init(NATIVE_INT_TYPE queueDepth);

    private:
        void ExArrayIn_handler(NATIVE_INT_TYPE portNum, const Example::SubNameSpace::ArrayType& array1, const Example::ArrSerial& serial1);
        void ArrayIn_handler(NATIVE_INT_TYPE portNum, const Example::SubNameSpace::ArrayType& array1, const Example::ArrSerial& serial1);
	};

};

#endif
