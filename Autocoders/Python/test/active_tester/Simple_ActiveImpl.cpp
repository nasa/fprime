#include <Autocoders/Python/test/active_tester/Simple_ActiveImpl.hpp>
#include <iostream>

namespace Simple_Active{

    Simple_ActiveImpl::Simple_ActiveImpl(const char* compName) : Simple_ActiveComponentBase(compName){
    }

    Simple_ActiveImpl::~Simple_ActiveImpl(void){
    }

    void Simple_ActiveImpl::init(NATIVE_INT_TYPE queueDepth){
        Simple_ActiveComponentBase::init(queueDepth);
    }

    void Simple_ActiveImpl::U32_Async_handler(NATIVE_INT_TYPE portNum, U32 int_arg){
        //Delay Output
        for(int i = 0; i < 9999; i++){
        }
        printf("\nComp_A Recieved Input: %d\n", int_arg);
    }

    void Simple_ActiveImpl::F32_Sync_handler(NATIVE_INT_TYPE portNum, F32 float_args){
        //Delay Output
        for(int i = 0; i < 9999; i++){
        }
        printf("\nComp_A Recieved Input: %lf\n", float_args);
    }

};
