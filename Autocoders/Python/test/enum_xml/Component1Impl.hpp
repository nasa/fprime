#ifndef EXAMPLE_ENUM_IMPL_HPP
#define EXAMPLE_ENUM_IMPL_HPP

#include <Autocoders/Python/test/enum_xml/Component1ComponentAc.hpp>

namespace Example {

	class ExampleEnumImpl : public Component1ComponentBase  {
    public:

		// Only called by derived class
		ExampleEnumImpl(const char* compName);
		~ExampleEnumImpl();
		void init(NATIVE_INT_TYPE queueDepth);

    private:
        void ExEnumIn_handler(NATIVE_INT_TYPE portNum, const Example::SubNamespace::Enum1& enum1, const Example::Serial1& serial1);
        void EnumIn_handler(NATIVE_INT_TYPE portNum, const Example::SubNamespace::Enum1& enum1, const Example::Serial1& serial1);
	};

};

#endif
