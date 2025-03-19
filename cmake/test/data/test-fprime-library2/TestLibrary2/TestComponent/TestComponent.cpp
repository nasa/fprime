#include <TestLibrary2/TestComponent/TestComponent.hpp>

namespace TestLibrary2 {

TestComponent ::TestComponent(const char* name) :
    TestComponentComponentBase(name)
{}

TestComponent ::~TestComponent() {}

void TestComponent ::schedIn_handler(FwIndexType portNum, U32 context) {}
};
