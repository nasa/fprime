#include <TestLibrary2/TestComponent/TestComponent.hpp>

namespace TestLibrary2 {

TestComponent ::TestComponent(const char* name) :
    TestComponentComponentBase(name)
{}

void TestComponent ::init(const NATIVE_INT_TYPE instance) {
    TestComponentComponentBase::init(instance);
}

TestComponent ::~TestComponent() {}

void TestComponent ::schedIn_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {}
};
