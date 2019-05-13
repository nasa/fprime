#ifndef DUCK_DUCK_IMPL_HPP
#define DUCK_DUCK_IMPL_HPP

#include <Autocoders/Python/test/partition/DuckDuckComponentAc.hpp>

namespace Duck {

    class DuckImpl : public DuckBase  {
    public:

        // Only called by derived class
        DuckImpl(const char* compName);
        ~DuckImpl(void);

    private:
        // downcall for input ports
        I32 externInputPort1_Msg1_handler(U32 cmd, Fw::EightyCharString str);
        I32 externInputPort3_Msg3_handler(U32 cmd);
        I32 inputPort1_Msg1_handler(U32 cmd, Fw::EightyCharString str);
        I32 inputPort2_Msg1_handler(U32 cmd, Fw::EightyCharString str);
        I32 inputPort3_Msg3_handler(U32 cmd);
    };
};

#endif



