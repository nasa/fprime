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
                AnotherExample::SomeEnum arg2, //!< The ENUM argument
                AnotherExample::arg6String arg6
        );
    };

    void ATester::from_exampleOutput_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg1, //!< A built-in type argument
            AnotherExample::SomeEnum arg2, //!< The ENUM argument
            AnotherExample::arg6String arg6
    ) {

    }

    int main(int argc, char* argv[]) {

        ATester testBase;

    }
