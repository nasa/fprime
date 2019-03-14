#include <Autocoders/Python/test/active_tester/Simple_Active_TesterComponentAc.hpp>


namespace Simple_Active_Tester{

    class Simple_Active_TesterImpl : public Simple_Active_TesterComponentBase {




    public:
        Simple_Active_TesterImpl(const char* compName);
        void init(NATIVE_INT_TYPE instance);

    };

};
