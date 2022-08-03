#include <Autocoders/Python/test/enum_xml/Component1Impl.hpp>
#include <FpConfig.hpp>
#include <iostream>
#include <cstdio>

using namespace std;

namespace Example {

    ExampleEnumImpl::ExampleEnumImpl(const char* compName) : Component1ComponentBase(compName) {
    }

    ExampleEnumImpl::~ExampleEnumImpl() {

    }

    void ExampleEnumImpl::init(NATIVE_INT_TYPE queueDepth) {
        Component1ComponentBase::init(queueDepth);
    }

    void ExampleEnumImpl::ExEnumIn_handler(NATIVE_INT_TYPE portNum, const Example::Enum1& enum1, const Example::Serial1& serial1) {
        printf("%s Invoked ExEnumIn_handler(%d, %d, %d, %d, %d);\n", FW_OPTIONAL_NAME(this->getObjName()), portNum, static_cast<FwEnumStoreType>(enum1.e), serial1.getMember1(), serial1.getMember2(), static_cast<FwEnumStoreType>(serial1.getMember3().e));
        this->EnumOut_out(0, enum1, serial1);
    }

    void ExampleEnumImpl::EnumIn_handler(NATIVE_INT_TYPE portNum, const Example::Enum1& enum1, const Example::Serial1& serial1) {
        printf("%s Invoked EnumIn_handler(%d, %d, %d, %d, %d);\n", FW_OPTIONAL_NAME(this->getObjName()), portNum, static_cast<FwEnumStoreType>(enum1.e), serial1.getMember1(), serial1.getMember2(), static_cast<FwEnumStoreType>(serial1.getMember3().e));
    }
};
