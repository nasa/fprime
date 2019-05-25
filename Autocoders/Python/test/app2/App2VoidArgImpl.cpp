#include <Autocoders/Python/test/app2/App2VoidArgImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace App2 {

        VoidArgImpl::VoidArgImpl(const char* compName) : VoidArgComponentBase(compName) {

        }

        VoidArgImpl::~VoidArgImpl(void) {

        }

		void VoidArgImpl::init(NATIVE_INT_TYPE queueDepth) {
			VoidArgComponentBase::init(queueDepth);
		}

        // downcall for input port ExtIn
        void VoidArgImpl::ExtIn_handler(NATIVE_INT_TYPE portNum) {
			printf("*** %s(%d): ExtIn_Void_handler down-call\n", this->m_objName, portNum);
			this->Out_out(portNum);
        }

        // downcall for input port In
        void VoidArgImpl::In_handler(NATIVE_INT_TYPE portNum) {
			printf("*** %s(%d): In_Void_handler down-call\n", this->m_objName, portNum);
        }

        // downcall for input port ExtMsgIn
        void VoidArgImpl::ExtMsgIn_handler(NATIVE_INT_TYPE portNum, U32 arg1, I32 arg2, F32 arg3) {
			printf("*** %s(%d): ExtMsgIn_Msg_handler down-call\n", this->m_objName, portNum);
        	MsgOut_out(portNum,arg1,arg2,arg3);
        }

        // downcall for input port MsgIn
        void VoidArgImpl::MsgIn_handler(NATIVE_INT_TYPE portNum, U32 arg1, I32 arg2, F32 arg3) {
			printf("*** %s(%d): MsgIn_Msg_handler down-call\n", this->m_objName, portNum);
			printf("*** %s(%d): arg1 = %u arg2 = %d arg3 = %f\n", this->m_objName, portNum, arg1, arg2, arg3);
        }

};


