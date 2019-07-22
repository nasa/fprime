#ifndef EXAMPLE_ENUM_IMPL_HPP
#define EXAMPLE_ENUM_IMPL_HPP

#include <Autocoders/Python/test/testgen/TestComponentAc.hpp>

namespace TestGen {

	class TestComponentComponentImpl : public TestComponentComponentBase  {
    public:

		// Only called by derived class
		TestComponentComponentImpl(const char *compName);
		~TestComponentComponentImpl(void);
		void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance);

    private:
        void aport_handler(NATIVE_INT_TYPE portNum, I32 arg1, F32 arg2, U8 arg3);
        void bport_handler(NATIVE_INT_TYPE portNum, I32 arg1, F32 arg2, U8 arg3);
        void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3);
	};

};

#endif
