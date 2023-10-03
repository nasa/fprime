#include "time_getGTestBase.hpp"
#include <FpConfig.hpp>
#include <Autocoders/Python/test/time_get/TestTimeGetImpl.hpp>

// Very minimal to test autocoder. Some day they'll be actual unit test code
// Here we test to ensure test harness generates correctly
class ATester : public TimeGet::TimeGetTesterGTestBase {
    public:
        ATester() : TimeGet::TimeGetTesterGTestBase("comp",10) {}
};

int main(int argc, char* argv[]) {

    ATester testBase;
    TimeGetTesterImpl component("name");
    component.getTime();
}
