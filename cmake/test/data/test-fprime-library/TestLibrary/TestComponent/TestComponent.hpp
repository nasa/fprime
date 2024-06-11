#ifndef TestLibrary_TestComponent_HPP
#define TestLibrary_TestComponent_HPP

#include <TestLibrary/TestComponent/TestComponentComponentAc.hpp>

namespace TestLibrary {

class TestComponent : public TestComponentComponentBase
{
  public:
    TestComponent(const char* name);
    void init(const NATIVE_INT_TYPE instance);
    ~TestComponent();
  private:
    void schedIn_handler(NATIVE_INT_TYPE portNum, U32 context);
};

};
#endif
