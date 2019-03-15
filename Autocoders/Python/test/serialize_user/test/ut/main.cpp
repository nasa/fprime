#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public ExampleComponents::ExampleComponentGTestBase {
    public:
#if FW_OBJECT_NAMES == 1
        ATester(void) : ExampleComponents::ExampleComponentGTestBase("comp",10) {
#else
        ATester(void) : ExampleComponents::ExampleComponentGTestBase(10)  {
#endif
    }


   void from_exampleOutput_handler(
		const NATIVE_INT_TYPE portNum, //!< The port number
		I32 arg1, //!< A built-in type argument
		ANameSpace::UserSerializer arg2 //!< A user-defined type argument
	);
};



void ATester::from_exampleOutput_handler(
	const NATIVE_INT_TYPE portNum, //!< The port number
	I32 arg1, //!< A built-in type argument
	ANameSpace::UserSerializer arg2 //!< A user-defined type argument
) {

}

int main(int argc, char* argv[]) {

    ATester testBase;

}
