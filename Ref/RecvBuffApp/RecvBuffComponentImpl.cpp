#include <Ref/RecvBuffApp/RecvBuffComponentImpl.hpp>
#include <FpConfig.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/Assert.hpp>

#include <cstdio>

#define DEBUG_LVL 1

namespace Ref {

    RecvBuffImpl::RecvBuffImpl(const char* compName) :
            RecvBuffComponentBase(compName) {
        this->m_firstBuffReceived = 0;
        this->m_sensor1 = 1000.0;
        this->m_sensor2 = 10.0;
        this->m_stats.setBuffRecv(0);
        this->m_stats.setBuffErr(0);
        this->m_stats.setPacketStatus(PacketRecvStatus::PACKET_STATE_NO_PACKETS);
    }


    void RecvBuffImpl::init(NATIVE_INT_TYPE instanceId) {
        RecvBuffComponentBase::init(instanceId);
    }

    RecvBuffImpl::~RecvBuffImpl() {

    }

    void RecvBuffImpl::Data_handler(NATIVE_INT_TYPE portNum, Drv::DataBuffer &buff) {

        this->m_stats.setBuffRecv(++this->m_buffsReceived);
        // reset deserialization of buffer
        buff.resetDeser();
        // deserialize packet ID
        U32 id = 0;
        Fw::SerializeStatus stat = buff.deserialize(id);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // deserialize data
        U8 testData[24] = {0};
        NATIVE_UINT_TYPE size = sizeof(testData);
        stat = buff.deserialize(testData,size);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // deserialize checksum
        U32 csum = 0;
        stat = buff.deserialize(csum);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        // if first packet, send event
        if (not this->m_firstBuffReceived) {
            this->log_ACTIVITY_LO_FirstPacketReceived(id);
            this->m_stats.setPacketStatus(PacketRecvStatus::PACKET_STATE_OK);
            this->m_firstBuffReceived = true;
        }

        // compute checksum
        U32 sum = 0;
        for (U32 byte = 0; byte < size; byte++) {
            sum += testData[byte];
        }
        // check checksum
        if (sum != csum) {
            // increment error count
            this->m_stats.setBuffErr(++this->m_errBuffs);
            // send error event
            this->log_WARNING_HI_PacketChecksumError(id);
            // update stats
            this->m_stats.setPacketStatus(PacketRecvStatus::PACKET_STATE_ERRORS);
        }
        // update sensor values
        this->m_sensor1 += 5.0;
        this->m_sensor2 += 1.2;
        // update channels
        this->tlmWrite_Sensor1(this->m_sensor1);
        this->tlmWrite_Sensor2(this->m_sensor2);
        this->tlmWrite_PktState(this->m_stats);

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
