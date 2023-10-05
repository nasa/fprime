#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <port_loopbackGTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public ExampleComponents::ExampleGTestBase {
public:
    ATester() : ExampleComponents::ExampleGTestBase("comp",10) {
        }

        void from_exampleOutput_handler(
                const NATIVE_INT_TYPE portNum, //!< The port number
                I32 arg1, //!< A built-in type argument
                AnotherExample::SomeEnum arg2, //!< The ENUM argument
                const AnotherExample::arg6String& arg6
        );
    };

    void ATester::from_exampleOutput_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg1, //!< A built-in type argument
            AnotherExample::SomeEnum arg2, //!< The ENUM argument
            const AnotherExample::arg6String& arg6
    ) {

    }

    int main(int argc, char* argv[]) {

        ATester testBase;

    }
