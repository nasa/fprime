#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <string_portGTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public ExampleComponents::TestComponentGTestBase {
public:
    ATester() : ExampleComponents::TestComponentGTestBase("comp",10) {
        }

        void from_testOut_handler(
                const NATIVE_INT_TYPE portNum, //!< The port number
                I32 arg1, //!< A built-in type argument
                AnotherExample::arg2String &arg2, //!< A string argument
                const AnotherExample::arg3Buffer &arg3 //!< A buffer argument
        );
    };


    void ATester::from_testOut_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg1, //!< A built-in type argument
            AnotherExample::arg2String &arg2, //!< A string argument
            const AnotherExample::arg3Buffer &arg3 //!< A buffer argument
    ) {

    }


    int main(int argc, char* argv[]) {

        ATester testBase;

    }
