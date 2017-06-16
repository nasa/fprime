#include <Ref/RecvBuffApp/RecvBuffComponentImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/Assert.hpp>

#include <stdio.h>

#define DEBUG_LVL 1

namespace Ref {

#if FW_OBJECT_NAMES == 1    
    RecvBuffImpl::RecvBuffImpl(const char* compName) :
        RecvBuffComponentBase(compName) {
#else
    RecvBuffImpl::RecvBuffImpl() {
#endif
        this->m_buffsReceived = 0;
        this->m_firstBuffReceived = 0;
        this->m_errBuffs = 0;
        this->m_sensor1 = 1000.0;
        this->m_sensor2 = 10.0;
    }


    void RecvBuffImpl::init(void) {
        RecvBuffComponentBase::init();
    }
    
    RecvBuffImpl::~RecvBuffImpl(void) {

    }

    void RecvBuffImpl::Data_handler(NATIVE_INT_TYPE portNum, Drv::DataBuffer &buff) {

        this->m_buffsReceived++;
        this->tlmWrite_NumPkts(this->m_buffsReceived);
        // reset deserialization of buffer
        buff.resetDeser();
        // deserialize packet ID
        U32 id;
        Fw::SerializeStatus stat = buff.deserialize(id);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // deserialize data
        U8 testData[24];
        NATIVE_UINT_TYPE size = sizeof(testData);
        stat = buff.deserialize(testData,size);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // deserialize checksum
        U32 csum;
        stat = buff.deserialize(csum);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // if first packet, send event
        if (not this->m_firstBuffReceived) {
            this->log_ACTIVITY_LO_FirstPacketReceived(id);
            this->m_firstBuffReceived = true;
            this->tlmWrite_NumErrPkts(this->m_errBuffs);
        }

        // compute checksum
        U32 sum = 0;
        for (U32 byte = 0; byte < size; byte++) {
            sum += testData[byte];
        }
        // check checksum
        if (sum != csum) {
            // increment error count
            this->m_errBuffs++;
            this->tlmWrite_NumErrPkts(this->m_errBuffs);
            // send error event
            this->log_WARNING_HI_PacketChecksumError(id);
        }
        // update sensor values
        this->m_sensor1 += 5.0;
        this->m_sensor2 += 1.2;
        // update channels
        this->tlmWrite_Sensor1(this->m_sensor1);
        this->tlmWrite_Sensor2(this->m_sensor2);

    }

    void RecvBuffImpl::toString(char* str, I32 buffer_size) {
#if FW_OBJECT_NAMES == 1    
        (void)snprintf(str, buffer_size, "RecvBuffImpl: %s: ATM recd count: %d", this->m_objName,
                        (int) this->m_buffsReceived);
#else
        (void)snprintf(str, buffer_size, "RecvBuffImpl: ATM recd count: %d",
                        (int) this->m_buffsReceived);
#endif
    }

    void RecvBuffImpl::parameterUpdated(FwPrmIdType id) {
        this->log_ACTIVITY_LO_BuffRecvParameterUpdated(id);
        Fw::ParamValid valid;
        switch(id) {
            case PARAMID_PARAMETER1: {
                U32 val = this->paramGet_parameter1(valid);
                this->tlmWrite_Parameter1(val);
                break;
            }
            case PARAMID_PARAMETER2: {
                I16 val = this->paramGet_parameter2(valid);
                this->tlmWrite_Parameter2(val);
                break;
            }
            default:
                FW_ASSERT(0,id);
                break;
        }
    }

}
