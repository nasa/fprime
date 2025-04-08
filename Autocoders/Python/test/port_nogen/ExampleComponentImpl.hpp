/*
 * ExampleComponentImpl.hpp
 *
 *  Created on: Nov 3, 2014
 *      Author: tcanham
 */

#ifndef EXAMPLECOMPONENTIMPL_HPP_
#define EXAMPLECOMPONENTIMPL_HPP_

#include <Autocoders/Python/test/port_nogen/ExampleComponentAc.hpp>

namespace ExampleComponents {

    class ExampleComponentImpl: public ExampleComponentBase {
        public:
            ExampleComponentImpl(const char* name);
            void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance = 0);
            virtual ~ExampleComponentImpl();
        protected:
        private:
            void exampleInput_handler(FwIndexType portNum, I32 arg1, const ANameSpace::mytype& arg2, U8 arg3, const Example3::ExampleSerializable& arg4, AnotherExample::SomeEnum arg5);
            void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, ExampleComponentBase::CmdEnum arg2, const Fw::CmdStringArg& arg3);
            void TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2);

    };

} /* namespace ExampleComponents */

#endif /* EXAMPLECOMPONENTIMPL_HPP_ */
