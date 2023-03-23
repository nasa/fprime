#include <Ref/SendBuffApp/SendBuffComponentImpl.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Log.hpp>
#include <cstring>

#include <cstdio>

#define DEBUG_LEVEL 1

namespace Ref {

    SendBuffImpl::SendBuffImpl(const char* compName) :
        SendBuffComponentBase(compName) {
        this->m_currPacketId = 0;
        this->m_invocations = 0;
        this->m_buffsSent = 0;
        this->m_errorsInjected = 0;
        this->m_injectError = false;
        this->m_sendPackets = false;
        this->m_currPacketId = 0;
        this->m_firstPacketSent = false;
        this->m_state = SendBuff_ActiveState::SEND_IDLE;
    }

    SendBuffImpl::~SendBuffImpl() {

    }

    void SendBuffImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        SendBuffComponentBase::init(queueDepth,instance);
    }

    void SendBuffImpl::SchedIn_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {

        // first, dequeue any messages

        MsgDispatchStatus stat = MSG_DISPATCH_OK;

        while (MSG_DISPATCH_OK == stat) {
            stat = this->doDispatch();
            FW_ASSERT(stat != MSG_DISPATCH_ERROR);
        }

        if (this->m_sendPackets) {
            // check to see if first
            if (this->m_firstPacketSent) {
                this->m_firstPacketSent = false;
                this->log_ACTIVITY_HI_FirstPacketSent(this->m_currPacketId);
                this->tlmWrite_NumErrorsInjected(this->m_errorsInjected);
            }
            // reset buffer
            this->m_testBuff.resetSer();
            // serialize packet id
            FW_ASSERT(this->m_testBuff.serialize(this->m_currPacketId) == Fw::FW_SERIALIZE_OK);
            // increment packet id
            this->m_currPacketId++;
            this->m_buffsSent++;
            // set telemetry
            this->tlmWrite_PacketsSent(this->m_buffsSent);
            // write data
            U8 testData[24];
            NATIVE_UINT_TYPE dataSize = sizeof(testData);
            memset(testData,0xFF,dataSize);
            // compute checksum
            U32 csum = 0;
            for (U32 byte = 0; byte < dataSize; byte++) {
                csum += testData[byte];
            }
            // inject error, if requested
            if (this->m_injectError) {
                this->m_injectError = false;
                this->m_errorsInjected++;
                testData[5] = 0;
                this->log_WARNING_HI_PacketErrorInserted(this->m_currPacketId-1);
            }
            // serialize data
            FW_ASSERT(this->m_testBuff.serialize(testData,dataSize) == Fw::FW_SERIALIZE_OK);
            // serialize checksum
            FW_ASSERT(this->m_testBuff.serialize(csum) == Fw::FW_SERIALIZE_OK);
            // send data
            this->Data_out(0,this->m_testBuff);

        }

        this->m_invocations++;

        this->tlmWrite_SendState(this->m_state);
    }

    void SendBuffImpl::toString(char* str, I32 buffer_size) {
#if FW_OBJECT_NAMES == 1
        (void) snprintf(str, buffer_size, "Send Buff Component: %s: count: %d Buffs: %d", this->m_objName,
                        (int) this->m_invocations, (int) this->m_buffsSent);
        str[buffer_size-1] = 0;
#else
        (void) snprintf(str, buffer_size, "Lps Atm Component: count: %d ATMs: %d",
                        (int) this->m_invocations, (int) this->m_buffsSent);
#endif
    }


    void SendBuffImpl::SB_START_PKTS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        this->m_sendPackets = true;
        this->m_state = SendBuff_ActiveState::SEND_ACTIVE;
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void SendBuffImpl::SB_INJECT_PKT_ERROR_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        this->m_injectError = true;
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void SendBuffImpl::SB_GEN_FATAL_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq, /*!< The command sequence number*/
            U32 arg1, /*!< First FATAL Argument*/
            U32 arg2, /*!< Second FATAL Argument*/
            U32 arg3 /*!< Third FATAL Argument*/
        ) {
        this->log_FATAL_SendBuffFatal(arg1,arg2,arg3);
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

        //! Handler for command SB_GEN_ASSERT
        /* Generate an ASSERT */
     void SendBuffImpl::SB_GEN_ASSERT_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq, /*!< The command sequence number*/
            U32 arg1, /*!< First ASSERT Argument*/
            U32 arg2, /*!< Second ASSERT Argument*/
            U32 arg3, /*!< Third ASSERT Argument*/
            U32 arg4, /*!< Fourth ASSERT Argument*/
            U32 arg5, /*!< Fifth ASSERT Argument*/
            U32 arg6 /*!< Sixth ASSERT Argument*/
        ) {
         FW_ASSERT(0,arg1,arg2,arg3,arg4,arg5,arg6);
         this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
     }

    void SendBuffImpl::parameterUpdated(FwPrmIdType id) {
        this->log_ACTIVITY_LO_BuffSendParameterUpdated(id);
        Fw::ParamValid valid;
        switch(id) {
            case PARAMID_PARAMETER3: {
                U8 val = this->paramGet_parameter3(valid);
                this->tlmWrite_Parameter3(val);
                break;
            }
            case PARAMID_PARAMETER4: {
                F32 val = this->paramGet_parameter4(valid);
                this->tlmWrite_Parameter4(val);
                break;
            }
            default:
                FW_ASSERT(0,id);
                break;
        }
    }
}
