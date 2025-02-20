/*
 * ExampleComponentImpl.hpp
 *
 *  Created on: Nov 3, 2014
 *      Author: tcanham
 */

#ifndef EXAMPLECOMPONENTIMPL_HPP_
#define EXAMPLECOMPONENTIMPL_HPP_

#include <Autocoders/Python/templates/ExampleComponentAc.hpp>

namespace ExampleComponents {

    class ExampleComponentImpl: public ExampleComponentBase {
        public:
            ExampleComponentImpl(const char* name);
            void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance = 0);
            virtual ~ExampleComponentImpl();
        protected:
        private:
            void exampleInput_handler(FwIndexType portNum, I32 arg1, const ANameSpace::mytype& arg2, U8 arg3, const Example3::ExampleSerializable& arg4, AnotherExample::SomeEnum arg5);
            SomeOtherNamespace::AnotherEnum anotherInput_handler(FwIndexType portNum, I32 arg1, F64 arg2, SomeOtherNamespace::SomeEnum arg3);
            void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, ExampleComponentBase::CmdEnum arg2, const Fw::CmdStringArg& arg3);
            void TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2);

            // interfaces

            void test_internalInterfaceHandler(I32 arg1, F32 arg2, U8 arg3);
            void test2_internalInterfaceHandler(I32 arg1, SomeEnum arg2, const Fw::InternalInterfaceString& arg3, const Example4::Example2& arg4);

            void parameterUpdated(FwPrmIdType id);

    };

} /* namespace ExampleComponents */

#endif /* EXAMPLECOMPONENTIMPL_HPP_ */
