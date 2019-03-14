#ifdef FPRIME_CMAKE
#include "Autocoder/GTestBase.hpp"
#else
#include <GTestBase.hpp>
#endif

// Very minimal to test autocoder. Some day they'll be actual unit test code

class ATester : public StressTest::TestPortGTestBase {
public:
#if FW_OBJECT_NAMES == 1
    ATester(void) : StressTest::TestPortGTestBase("comp",10) {
#else
        ATester(void) : StressTest::TestPortGTestBase(10)  {
#endif
        }

        void from_aport_handler(
                const NATIVE_INT_TYPE portNum, //!< The port number
                I32 arg4, //!< The first argument
                F32 arg5, //!< The second argument
                U8 arg6 //!< The third argument
        );

        void from_aport2_handler(
                const NATIVE_INT_TYPE portNum, //!< The port number
                I32 arg4, //!< The first argument
                F32 arg5, //!< The second argument
                Ref::Gnc::Quaternion arg6 //!< The third argument
        );
    };


    void ATester::from_aport_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg4, //!< The first argument
            F32 arg5, //!< The second argument
            U8 arg6 //!< The third argument
    ) {

    }

    void ATester::from_aport2_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            I32 arg4, //!< The first argument
            F32 arg5, //!< The second argument
            Ref::Gnc::Quaternion arg6 //!< The third argument
    ) {

    }

    int main(int argc, char* argv[]) {

        ATester testBase;

    }
