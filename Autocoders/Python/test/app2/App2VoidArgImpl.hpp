#ifndef DUCK_DUCK_IMPL_HPP
#define DUCK_DUCK_IMPL_HPP

#include <Autocoders/Python/test/app2/VoidArgComponentAc.hpp>

namespace App2 {

    class VoidArgImpl : public VoidArgComponentBase  {
    public:

        // Only called by derived class
        VoidArgImpl(const char* compName);
        ~VoidArgImpl(void);
		void init(NATIVE_INT_TYPE queueDepth);

    private:
        // downcall for input ports
        void ExtIn_handler(NATIVE_INT_TYPE portNum);
        void In_handler(NATIVE_INT_TYPE portNum);
        void ExtMsgIn_handler(NATIVE_INT_TYPE portNum, U32 arg1, I32 arg2, F32 arg3);
        void MsgIn_handler(NATIVE_INT_TYPE portNum, U32 arg1, I32 arg2, F32 arg3);
    };
};

#endif



