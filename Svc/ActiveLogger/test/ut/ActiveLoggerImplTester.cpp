/*
 * ActiveLoggerImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/ActiveLogger/test/ut/ActiveLoggerImplTester.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Os/IntervalTimer.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Os/Stubs/FileStubs.hpp>

#include <cstdio>

namespace Svc {

    typedef ActiveLogger_Enabled Enabled;
    typedef ActiveLogger_FilterSeverity FilterSeverity;

    void ActiveLoggerImplTester::init(NATIVE_INT_TYPE instance) {
        Svc::ActiveLoggerGTestBase::init();
    }

    ActiveLoggerImplTester::ActiveLoggerImplTester(Svc::ActiveLoggerImpl& inst) :
            Svc::ActiveLoggerGTestBase("testerbase",100),
            m_impl(inst),
            m_receivedPacket(false),
            m_receivedFatalEvent(false) {
    }

    ActiveLoggerImplTester::~ActiveLoggerImplTester() {
    }

    void ActiveLoggerImplTester::from_PktSend_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::ComBuffer &data, //!< Buffer containing packet data
            U32 context //!< context; not used
        ) {
        this->m_sentPacket = data;
        this->m_receivedPacket = true;
    }

    void ActiveLoggerImplTester::from_FatalAnnounce_handler(
                          const NATIVE_INT_TYPE portNum, //!< The port number
                          FwEventIdType Id //!< The ID of the FATAL event
                      ) {
        this->m_receivedFatalEvent = true;
        this->m_fatalID = Id;
    }

    void ActiveLoggerImplTester::runEventNominal() {
        REQUIREMENT("AL-001");

        this->writeEvent(29,Fw::LogSeverity::WARNING_HI,10);
    }

    void ActiveLoggerImplTester::runWithFilters(Fw::LogSeverity filter) {

        REQUIREMENT("AL-002");

        Fw::LogBuffer buff;
        U32 val = 10;
        FwEventIdType id = 29;

        Fw::SerializeStatus stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,0,0);
        U32 cmdSeq = 21;

        // enable report filter
        this->clearHistory();
        FilterSeverity reportFilterLevel = FilterSeverity::WARNING_HI;

        switch (filter.e) {
            case Fw::LogSeverity::WARNING_HI:
                reportFilterLevel = FilterSeverity::WARNING_HI;
                break;
            case Fw::LogSeverity::WARNING_LO:
                reportFilterLevel = FilterSeverity::WARNING_LO;
                break;
            case Fw::LogSeverity::COMMAND:
                reportFilterLevel = FilterSeverity::COMMAND;
                break;
            case Fw::LogSeverity::ACTIVITY_HI:
                reportFilterLevel = FilterSeverity::ACTIVITY_HI;
                break;
            case Fw::LogSeverity::ACTIVITY_LO:
                reportFilterLevel = FilterSeverity::ACTIVITY_LO;
                break;
            case Fw::LogSeverity::DIAGNOSTIC:
                reportFilterLevel = FilterSeverity::DIAGNOSTIC;
                break;
            default:
                ASSERT_TRUE(false);
                break;
        }

        this->clearHistory();
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,reportFilterLevel,Enabled::ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::OK
                );

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,filter,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // dispatch message
        this->m_impl.doDispatch();
        // should have received packet
        ASSERT_TRUE(this->m_receivedPacket);
        // verify contents
        // first piece should be log packet descriptor
        FwPacketDescriptorType desc;
        stat = this->m_sentPacket.deserialize(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(desc,static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_LOG));
        // next piece should be event ID
        FwEventIdType sentId;
        stat = this->m_sentPacket.deserialize(sentId);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(sentId,id);
        // next piece is time tag
        Fw::Time recTimeTag(TB_NONE,0,0);
        stat = this->m_sentPacket.deserialize(recTimeTag);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_TRUE(timeTag == recTimeTag);
        // next piece is event argument
        U32 readVal;
        stat = this->m_sentPacket.deserialize(readVal);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(readVal, val);
        // packet should be empty
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),0u);

        // Disable severity filter
        this->clearHistory();
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,reportFilterLevel,Enabled::DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::OK
                );

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,filter,buff);

        // should not have received packet - all we can check since no message is dispatched.
        ASSERT_FALSE(this->m_receivedPacket);
    }

    void ActiveLoggerImplTester::runFilterInvalidCommands() {

        U32 cmdSeq = 21;
        this->clearHistory();
        FilterSeverity reportFilterLevel = FilterSeverity::WARNING_HI;
        Enabled filterEnabled(static_cast<Enabled::t>(10));
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,reportFilterLevel,filterEnabled);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::FORMAT_ERROR
                );
        this->clearHistory();
        reportFilterLevel = FilterSeverity::WARNING_HI;
        filterEnabled.e = static_cast<Enabled::t>(-2);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,reportFilterLevel,filterEnabled);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::FORMAT_ERROR
                );
        FilterSeverity eventLevel;
        this->clearHistory();
        Enabled reportEnable = Enabled::ENABLED;
        eventLevel.e = static_cast<FilterSeverity::t>(-1);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,eventLevel,reportEnable);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::FORMAT_ERROR
                );

        this->clearHistory();

        reportEnable = Enabled::ENABLED;
        eventLevel.e = static_cast<FilterSeverity::t>(100);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,eventLevel,reportEnable);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::FORMAT_ERROR
                );

    }

    void ActiveLoggerImplTester::runFilterEventNominal() {

        for (Fw::LogSeverity::t sev = Fw::LogSeverity::WARNING_HI; sev <= Fw::LogSeverity::DIAGNOSTIC; sev = static_cast<Fw::LogSeverity::t>(sev + 1)) {
            this->runWithFilters(sev);
        }

    }

    void ActiveLoggerImplTester::runFilterIdNominal() {

        U32 cmdSeq = 21;

        // for a set of IDs, fill filter

        REQUIREMENT("AL-003");

        for (NATIVE_INT_TYPE filterID = 1; filterID <= TELEM_ID_FILTER_SIZE; filterID++) {
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_SET_ID_FILTER(0,cmdSeq,filterID,Enabled::ENABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                    cmdSeq,
                    Fw::CmdResponse::OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_ENABLED_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_ENABLED(0,filterID);
            // send it again, to verify it will accept a second add
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_SET_ID_FILTER(0,cmdSeq,filterID,Enabled::ENABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                    cmdSeq,
                    Fw::CmdResponse::OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_ENABLED_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_ENABLED(0,filterID);

        }

        // Try to send the IDs that are filtered
        for (NATIVE_INT_TYPE filterID = 1; filterID <= TELEM_ID_FILTER_SIZE; filterID++) {
            this->clearHistory();
            this->clearEvents();

            Fw::LogBuffer buff;
            U32 val = 10;
            FwEventIdType id = filterID;

            Fw::SerializeStatus stat = buff.serialize(val);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
            Fw::Time timeTag(TB_NONE,0,0);

            this->m_receivedPacket = false;

            this->invoke_to_LogRecv(0,id,timeTag,Fw::LogSeverity::ACTIVITY_HI,buff);

            // should not get a packet
            ASSERT_FALSE(this->m_receivedPacket);

        }

        // send one of the IDs as a FATAL, it should not be filtered event thought the ID is in the filter
        this->clearHistory();
        this->clearEvents();

        Fw::LogBuffer buff;
        U32 val = 10;
        FwEventIdType id = 1;

        Fw::SerializeStatus stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,0,0);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LogSeverity::FATAL,buff);
        this->m_impl.doDispatch();

        // should get a packet anyway
        ASSERT_TRUE(this->m_receivedPacket);

        // Try to add to the full filter. It should be rejected
        this->clearHistory();
        this->clearEvents();
        this->sendCmd_SET_ID_FILTER(0,cmdSeq,TELEM_ID_FILTER_SIZE+1,Enabled::ENABLED);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                cmdSeq,
                Fw::CmdResponse::EXECUTION_ERROR
                );
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ID_FILTER_LIST_FULL_SIZE(1);
        ASSERT_EVENTS_ID_FILTER_LIST_FULL(0,TELEM_ID_FILTER_SIZE+1);

        // Now clear them

        for (NATIVE_INT_TYPE filterID = 1; filterID <= TELEM_ID_FILTER_SIZE; filterID++) {
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_SET_ID_FILTER(0,cmdSeq,filterID,Enabled::DISABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                    cmdSeq,
                    Fw::CmdResponse::OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_REMOVED_SIZE(1);
            ASSERT_EVENTS_ID_FILTER_REMOVED(0,filterID);
        }

        // Try to clear one that doesn't exist

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_SET_ID_FILTER(0,cmdSeq,10,Enabled::DISABLED);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                cmdSeq,
                Fw::CmdResponse::EXECUTION_ERROR
                );
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ID_FILTER_NOT_FOUND_SIZE(1);
        ASSERT_EVENTS_ID_FILTER_NOT_FOUND(0,10);

        // Send an invalid argument
        this->clearHistory();
        this->clearEvents();
        Enabled idEnabled(static_cast<Enabled::t>(10));
        this->sendCmd_SET_ID_FILTER(0,cmdSeq,10,idEnabled);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_ID_FILTER,
                cmdSeq,
                Fw::CmdResponse::FORMAT_ERROR
                );
        ASSERT_EVENTS_SIZE(0);

    }

    void ActiveLoggerImplTester::runFilterDump() {
        U32 cmdSeq = 21;
        // set random set of filters

        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::WARNING_HI,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::WARNING_LO,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::COMMAND,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::ACTIVITY_HI,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::ACTIVITY_LO,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,0,FilterSeverity::DIAGNOSTIC,Enabled::ENABLED);

        this->sendCmd_SET_ID_FILTER(0,cmdSeq,4,Enabled::ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        this->sendCmd_SET_ID_FILTER(0,cmdSeq,13,Enabled::ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        this->sendCmd_SET_ID_FILTER(0,cmdSeq,4000,Enabled::ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        // send command to dump the filters

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_DUMP_FILTER_STATE(0,cmdSeq);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_DUMP_FILTER_STATE,
                cmdSeq,
                Fw::CmdResponse::OK
                );
        ASSERT_EVENTS_SIZE(6+3);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE_SIZE(6);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(0,FilterSeverity::WARNING_HI,true);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(1,FilterSeverity::WARNING_LO,false);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(2,FilterSeverity::COMMAND,true);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(3,FilterSeverity::ACTIVITY_HI,false);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(4,FilterSeverity::ACTIVITY_LO,true);
        ASSERT_EVENTS_SEVERITY_FILTER_STATE(5,FilterSeverity::DIAGNOSTIC,true);
    }

    void ActiveLoggerImplTester::runEventFatal() {
        Fw::LogBuffer buff;
        U32 val = 10;
        FwEventIdType id = 29;
        U32 cmdSeq = 21;
        REQUIREMENT("AL-004");

        Fw::SerializeStatus stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,0,0);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LogSeverity::FATAL,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // should have seen event port
        ASSERT_TRUE(this->m_receivedFatalEvent);
        ASSERT_EQ(this->m_fatalID,id);
        // dispatch message
        this->m_impl.doDispatch();
        // should have received packet
        ASSERT_TRUE(this->m_receivedPacket);
        // verify contents
        // first piece should be log packet descriptor
        FwPacketDescriptorType desc;
        stat = this->m_sentPacket.deserialize(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(desc,static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_LOG));
        // next piece should be event ID
        FwEventIdType sentId;
        stat = this->m_sentPacket.deserialize(sentId);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(sentId,id);
        // next piece is time tag
        Fw::Time recTimeTag(TB_NONE,0,0);
        stat = this->m_sentPacket.deserialize(recTimeTag);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_TRUE(timeTag == recTimeTag);
        // next piece is event argument
        U32 readVal;
        stat = this->m_sentPacket.deserialize(readVal);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(readVal, val);
        // packet should be empty
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),0u);
        // Turn on all filters and make sure FATAL still gets through

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::WARNING_HI,Enabled::DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_SET_EVENT_FILTER,
                cmdSeq,
                Fw::CmdResponse::OK
                );


        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::WARNING_LO,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::COMMAND,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::ACTIVITY_HI,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::ACTIVITY_LO,Enabled::DISABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::DIAGNOSTIC,Enabled::DISABLED);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LogSeverity::FATAL,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // dispatch message
        this->m_impl.doDispatch();
        // should have received packet
        ASSERT_TRUE(this->m_receivedPacket);
        // verify contents
        // first piece should be log packet descriptor
        stat = this->m_sentPacket.deserialize(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(desc,static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_LOG));
        // next piece should be event ID
        stat = this->m_sentPacket.deserialize(sentId);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(sentId,id);
        // next piece is time tag
        stat = this->m_sentPacket.deserialize(recTimeTag);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_TRUE(timeTag == recTimeTag);
        // next piece is event argument
        stat = this->m_sentPacket.deserialize(readVal);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(readVal, val);
        // packet should be empty
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),0u);

        // turn off filters

        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::WARNING_HI,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::WARNING_LO,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::COMMAND,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::ACTIVITY_HI,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::ACTIVITY_LO,Enabled::ENABLED);
        this->sendCmd_SET_EVENT_FILTER(0,cmdSeq,FilterSeverity::DIAGNOSTIC,Enabled::ENABLED);

    }

    void ActiveLoggerImplTester::writeEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value) {
        Fw::LogBuffer buff;

        Fw::SerializeStatus stat = buff.serialize(value);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,1,2);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,severity,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // dispatch message
        this->m_impl.doDispatch();
        // should have received packet
        ASSERT_TRUE(this->m_receivedPacket);
        // verify contents
        // first piece should be log packet descriptor
        FwPacketDescriptorType desc;
        stat = this->m_sentPacket.deserialize(desc);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(desc,static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_LOG));
        // next piece should be event ID
        FwEventIdType sentId;
        stat = this->m_sentPacket.deserialize(sentId);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(sentId,id);
        // next piece is time tag
        Fw::Time recTimeTag(TB_NONE,1,2);
        stat = this->m_sentPacket.deserialize(recTimeTag);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_TRUE(timeTag == recTimeTag);
        // next piece is event argument
        U32 readVal;
        stat = this->m_sentPacket.deserialize(readVal);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        ASSERT_EQ(readVal, value);
        // packet should be empty
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),0u);

    }

    void ActiveLoggerImplTester::readEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value, Os::File& file) {
        static const BYTE delimiter = 0xA5;

        // first read should be delimiter
        BYTE de;
        NATIVE_INT_TYPE readSize = sizeof(de);

        ASSERT_EQ(file.read(&de,readSize,true),Os::File::OP_OK);
        ASSERT_EQ(delimiter,de);
        // next is LogPacket
        Fw::ComBuffer comBuff;
        // size is specific to this test
        readSize = sizeof(FwPacketDescriptorType) + sizeof(FwEventIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(U32);
        ASSERT_EQ(file.read(comBuff.getBuffAddr(),readSize,true),Os::File::OP_OK);
        comBuff.setBuffLen(readSize);

        // deserialize LogPacket
        Fw::LogPacket packet;
        Fw::Time time(TB_NONE,1,2);
        Fw::LogBuffer logBuff;
        ASSERT_EQ(comBuff.deserialize(packet),Fw::FW_SERIALIZE_OK);

        // read back values
        ASSERT_EQ(id,packet.getId());
        ASSERT_EQ(time,packet.getTimeTag());
        logBuff = packet.getLogBuffer();
        U32 readValue;
        ASSERT_EQ(logBuff.deserialize(readValue),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(value,readValue);

    }

    void ActiveLoggerImplTester::textLogIn(const FwEventIdType id, //!< The event ID
            Fw::Time& timeTag, //!< The time
            const Fw::LogSeverity severity, //!< The severity
            const Fw::TextLogString& text //!< The event string
            ) {
        TextLogEntry e = { id, timeTag, severity, text };

        printTextLogHistoryEntry(e, stdout);
    }
    void ActiveLoggerImplTester ::
      from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_pingOut(key);
    }
} /* namespace SvcTest */
