#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public ExampleComponents::ExampleGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : ExampleComponents::ExampleGTestBase("comp",10) {
#else
        ATester(void) : ExampleComponents::ExampleGTestBase(10)  {
#endif
    }

		void from_exampleOutput_handler(
			const NATIVE_INT_TYPE portNum, //!< The port number
			I32 arg1, //!< A built-in type argument
			ANameSpace::mytype arg2, //!< A user-defined type argument
			U8 arg3, //!< The third argument
			Example3::ExampleSerializable arg4, //!< The third argument
			AnotherExample::SomeEnum arg5 //!< The ENUM argument
		);
};



		void ATester::from_exampleOutput_handler(
			const NATIVE_INT_TYPE portNum, //!< The port number
			I32 arg1, //!< A built-in type argument
			ANameSpace::mytype arg2, //!< A user-defined type argument
			U8 arg3, //!< The third argument
			Example3::ExampleSerializable arg4, //!< The third argument
			AnotherExample::SomeEnum arg5 //!< The ENUM argument
		) {

		}

int main(int argc, char* argv[]) {

    ATester testBase;

}
