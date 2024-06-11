#include <Autocoders/Python/test/pass_by_kind/Component1ComponentAc.hpp>
#include <Autocoders/Python/test/pass_by_kind/Component1.hpp>
#include <Autocoders/Python/test/pass_by_kind/Port1PortAc.hpp>
#include <Autocoders/Python/test/pass_by_kind/AllTypes.hpp>

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

#include <iostream>

using namespace std;

// Create the inst1 object
Example::Component1 inst1("inst1");

void constructArchitecture() {
    // Instantiate components
    inst1.init(100);
}

TEST(PassByTest, OK) {

    // Create arguments to test pass-by logic for async and sync cases
    cout << "Invoking inst1..." << endl;

    Example::AllTypes async_args(true);
    Example::AllTypes sync_args(false);

    // Invoke sync and async ports and pass arguments
    inst1.get_AsyncPort_InputPort(0)->invoke(
        &async_args.arg1,
        async_args.arg2,
        async_args.arg3,
        &async_args.arg4,
        async_args.arg5,
        async_args.arg6,
        &async_args.arg7,
        async_args.arg8,
        async_args.arg9
    );
    inst1.doDispatch();


    inst1.get_SyncPort_InputPort(0)->invoke(
        &sync_args.arg1,
        sync_args.arg2,
        sync_args.arg3,
        &sync_args.arg4,
        sync_args.arg5,
        sync_args.arg6,
        &sync_args.arg7,
        sync_args.arg8,
        sync_args.arg9
    );
    inst1.doDispatch();

    // Check output from ports is expected
    async_args.checkAsserts();
    sync_args.checkAsserts();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    constructArchitecture();

    int status = RUN_ALL_TESTS();

    return status;
}
