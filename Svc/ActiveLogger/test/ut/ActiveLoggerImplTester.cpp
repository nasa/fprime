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

    void ActiveLoggerImplTester::runEventNominal(void) {
        REQUIREMENT("AL-001");

        this->writeEvent(29,Fw::LOG_WARNING_HI,10);
    }

    void ActiveLoggerImplTester::runWithFilters(Fw::LogSeverity filter) {

        REQUIREMENT("AL-004");
        REQUIREMENT("AL-005");
        REQUIREMENT("AL-006");

        Fw::LogBuffer buff;
        U32 val = 10;
        FwEventIdType id = 29;

        Fw::SerializeStatus stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,0,0);
        U32 cmdSeq = 21;

        // enable report filter
        this->clearHistory();
        ActiveLoggerImpl::InputFilterLevel reportFilterLevel = ActiveLoggerImpl::INPUT_WARNING_HI;

        switch (filter) {
            case Fw::LOG_WARNING_HI:
                reportFilterLevel = ActiveLoggerImpl::INPUT_WARNING_HI;
                break;
            case Fw::LOG_WARNING_LO:
                reportFilterLevel = ActiveLoggerImpl::INPUT_WARNING_LO;
                break;
            case Fw::LOG_COMMAND:
                reportFilterLevel = ActiveLoggerImpl::INPUT_COMMAND;
                break;
            case Fw::LOG_ACTIVITY_HI:
                reportFilterLevel = ActiveLoggerImpl::INPUT_ACTIVITY_HI;
                break;
            case Fw::LOG_ACTIVITY_LO:
                reportFilterLevel = ActiveLoggerImpl::INPUT_ACTIVITY_LO;
                break;
            case Fw::LOG_DIAGNOSTIC:
                reportFilterLevel = ActiveLoggerImpl::INPUT_DIAGNOSTIC;
                break;
            default:
                ASSERT_TRUE(false);
                break;
        }

        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,reportFilterLevel,ActiveLoggerImpl::INPUT_ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );

        // enable send filter
        ActiveLoggerImpl::SendFilterLevel sendFilterLevel = ActiveLoggerImpl::SEND_WARNING_HI;

        switch (filter) {
            case Fw::LOG_WARNING_HI:
                sendFilterLevel = ActiveLoggerImpl::SEND_WARNING_HI;
                break;
            case Fw::LOG_WARNING_LO:
                sendFilterLevel = ActiveLoggerImpl::SEND_WARNING_LO;
                break;
            case Fw::LOG_COMMAND:
                sendFilterLevel = ActiveLoggerImpl::SEND_COMMAND;
                break;
            case Fw::LOG_ACTIVITY_HI:
                sendFilterLevel = ActiveLoggerImpl::SEND_ACTIVITY_HI;
                break;
            case Fw::LOG_ACTIVITY_LO:
                sendFilterLevel = ActiveLoggerImpl::SEND_ACTIVITY_LO;
                break;
            case Fw::LOG_DIAGNOSTIC:
                sendFilterLevel = ActiveLoggerImpl::SEND_DIAGNOSTIC;
                break;
            default:
                ASSERT_TRUE(false);
                break;
        }

        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,sendFilterLevel,ActiveLoggerImpl::SEND_ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
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
        ASSERT_EQ(desc,(FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_LOG);
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
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),(NATIVE_UINT_TYPE)0);

        // Disable severity in send filter

        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,sendFilterLevel,ActiveLoggerImpl::SEND_DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,filter,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // dispatch message
        this->m_impl.doDispatch();
        // should have received packet
        ASSERT_FALSE(this->m_receivedPacket);

        // Turn on receive filter. This isn't testable other
        // than "it doesn't crash" since nothing goes on the queue

        // enable receive filter
        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,reportFilterLevel,ActiveLoggerImpl::INPUT_DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,filter,buff);

        // should not have received packet
        ASSERT_FALSE(this->m_receivedPacket);
        // clear filters
        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,reportFilterLevel,ActiveLoggerImpl::INPUT_ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );

        this->clearHistory();
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,sendFilterLevel,ActiveLoggerImpl::SEND_ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );

    }

    void ActiveLoggerImplTester::runFilterInvalidCommands(void) {

        U32 cmdSeq = 21;

        this->clearHistory();
        ActiveLoggerImpl::InputFilterLevel reportFilterLevel = ActiveLoggerImpl::INPUT_WARNING_HI;
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,reportFilterLevel,(ActiveLoggerImpl::InputFilterEnabled)10);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        reportFilterLevel = ActiveLoggerImpl::INPUT_WARNING_HI;
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,reportFilterLevel,(ActiveLoggerImpl::InputFilterEnabled)-2);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        ActiveLoggerImpl::InputFilterEnabled reportEnable = ActiveLoggerImpl::INPUT_ENABLED;
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,(ActiveLoggerImpl::InputFilterLevel)-1,reportEnable);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        reportEnable = ActiveLoggerImpl::INPUT_ENABLED;
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,(ActiveLoggerImpl::InputFilterLevel)100,reportEnable);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        ActiveLoggerImpl::SendFilterLevel sendFilterLevel = ActiveLoggerImpl::SEND_WARNING_HI;
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,sendFilterLevel,(ActiveLoggerImpl::SendFilterEnabled) 10);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        sendFilterLevel = ActiveLoggerImpl::SEND_WARNING_HI;
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,sendFilterLevel,(ActiveLoggerImpl::SendFilterEnabled) -2);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        ActiveLoggerImpl::SendFilterEnabled sendFilterEnabled = ActiveLoggerImpl::SEND_ENABLED;
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,(ActiveLoggerImpl::SendFilterLevel)-1,sendFilterEnabled);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );

        this->clearHistory();
        sendFilterEnabled = ActiveLoggerImpl::SEND_ENABLED;
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,(ActiveLoggerImpl::SendFilterLevel)100,sendFilterEnabled);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_SEND_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );
    }

    void ActiveLoggerImplTester::runFilterEventNominal(void) {

        for (Fw::LogSeverity sev = Fw::LOG_WARNING_HI; sev <= Fw::LOG_DIAGNOSTIC; sev = (Fw::LogSeverity)((NATIVE_INT_TYPE)sev + 1)) {
            this->runWithFilters(sev);
        }

    }

    void ActiveLoggerImplTester::runFilterIdNominal(void) {

        U32 cmdSeq = 21;

        // for a set of IDs, fill filter

        REQUIREMENT("AL-007");

        for (NATIVE_INT_TYPE filterID = 1; filterID <= TELEM_ID_FILTER_SIZE; filterID++) {
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,filterID,ActiveLoggerComponentBase::ID_ENABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                    cmdSeq,
                    Fw::COMMAND_OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED(0,filterID);
            // send it again, to verify it will accept a second add
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,filterID,ActiveLoggerComponentBase::ID_ENABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                    cmdSeq,
                    Fw::COMMAND_OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED(0,filterID);

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

            this->invoke_to_LogRecv(0,id,timeTag,Fw::LOG_ACTIVITY_HI,buff);

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

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LOG_FATAL,buff);
        this->m_impl.doDispatch();

        // should get a packet anyway
        ASSERT_TRUE(this->m_receivedPacket);

        // Try to add to the full filter. It should be rejected
        this->clearHistory();
        this->clearEvents();
        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,TELEM_ID_FILTER_SIZE+1,ActiveLoggerComponentBase::ID_ENABLED);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                cmdSeq,
                Fw::COMMAND_EXECUTION_ERROR
                );
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ALOG_ID_FILTER_LIST_FULL_SIZE(1);
        ASSERT_EVENTS_ALOG_ID_FILTER_LIST_FULL(0,TELEM_ID_FILTER_SIZE+1);

        // Now clear them

        for (NATIVE_INT_TYPE filterID = 1; filterID <= TELEM_ID_FILTER_SIZE; filterID++) {
            this->clearHistory();
            this->clearEvents();
            this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,filterID,ActiveLoggerComponentBase::ID_DISABLED);
            // dispatch message
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                    cmdSeq,
                    Fw::COMMAND_OK
                    );
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_REMOVED_SIZE(1);
            ASSERT_EVENTS_ALOG_ID_FILTER_REMOVED(0,filterID);
        }

        // Try to clear one that doesn't exist

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,10,ActiveLoggerComponentBase::ID_DISABLED);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                cmdSeq,
                Fw::COMMAND_EXECUTION_ERROR
                );
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ALOG_ID_FILTER_NOT_FOUND_SIZE(1);
        ASSERT_EVENTS_ALOG_ID_FILTER_NOT_FOUND(0,10);

        // Send an invalid argument
        this->clearHistory();
        this->clearEvents();
        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,10,static_cast<ActiveLoggerComponentBase::IdFilterEnabled>(10));
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_ID_FILTER,
                cmdSeq,
                Fw::COMMAND_VALIDATION_ERROR
                );
        ASSERT_EVENTS_SIZE(0);

    }

    void ActiveLoggerImplTester::runFilterDump(void) {
        U32 cmdSeq = 21;
        // set random set of filters

        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_LO,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_COMMAND,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_HI,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_DIAGNOSTIC,ActiveLoggerImpl::INPUT_ENABLED);

        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_HI,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_LO,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_COMMAND,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_HI,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_DIAGNOSTIC,ActiveLoggerImpl::SEND_ENABLED);

        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,4,ActiveLoggerComponentBase::ID_ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,13,ActiveLoggerComponentBase::ID_ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        this->sendCmd_ALOG_SET_ID_FILTER(0,cmdSeq,4000,ActiveLoggerComponentBase::ID_ENABLED);
        // dispatch message
        this->m_impl.doDispatch();

        // send command to dump the filters

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_ALOG_DUMP_FILTER_STATE(0,cmdSeq);
        // dispatch message
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_DUMP_FILTER_STATE,
                cmdSeq,
                Fw::COMMAND_OK
                );
        ASSERT_EVENTS_SIZE(6+3);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE_SIZE(6);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(0,ActiveLoggerImpl::EVENT_FILTER_WARNING_HI,true,false);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(1,ActiveLoggerImpl::EVENT_FILTER_WARNING_LO,false,false);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(2,ActiveLoggerImpl::EVENT_FILTER_COMMAND,true,true);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(3,ActiveLoggerImpl::EVENT_FILTER_ACTIVITY_HI,false,false);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(4,ActiveLoggerImpl::EVENT_FILTER_ACTIVITY_LO,true,true);
        ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(5,ActiveLoggerImpl::EVENT_FILTER_DIAGNOSTIC,true,true);
    }

    void ActiveLoggerImplTester::runEventFatal(void) {
        Fw::LogBuffer buff;
        U32 val = 10;
        FwEventIdType id = 29;
        U32 cmdSeq = 21;

        Fw::SerializeStatus stat = buff.serialize(val);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat);
        Fw::Time timeTag(TB_NONE,0,0);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LOG_FATAL,buff);

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
        ASSERT_EQ(desc,(FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_LOG);
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
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),(NATIVE_UINT_TYPE)0);
        // Turn on all filters and make sure FATAL still gets through

        this->clearHistory();
        this->clearEvents();
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_WARNING_HI,ActiveLoggerImpl::INPUT_DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_SET_EVENT_REPORT_FILTER,
                cmdSeq,
                Fw::COMMAND_OK
                );


        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_WARNING_LO,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_COMMAND,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_ACTIVITY_HI,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_ACTIVITY_LO,ActiveLoggerImpl::INPUT_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_DIAGNOSTIC,ActiveLoggerImpl::INPUT_DISABLED);

        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_WARNING_HI,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_WARNING_LO,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_COMMAND,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_ACTIVITY_HI,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_ACTIVITY_LO,ActiveLoggerImpl::SEND_DISABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_DIAGNOSTIC,ActiveLoggerImpl::SEND_DISABLED);

        this->m_receivedPacket = false;

        this->invoke_to_LogRecv(0,id,timeTag,Fw::LOG_FATAL,buff);

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
        ASSERT_EQ(desc,(FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_LOG);
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
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),(NATIVE_UINT_TYPE)0);

        // turn off filters

        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_WARNING_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_WARNING_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_COMMAND,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_ACTIVITY_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_ACTIVITY_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,cmdSeq,ActiveLoggerImpl::INPUT_DIAGNOSTIC,ActiveLoggerImpl::INPUT_ENABLED);

        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_WARNING_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_WARNING_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_COMMAND,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_ACTIVITY_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_ACTIVITY_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,cmdSeq,ActiveLoggerImpl::SEND_DIAGNOSTIC,ActiveLoggerImpl::SEND_ENABLED);

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
        ASSERT_EQ(desc,(FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_LOG);
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
        ASSERT_EQ(this->m_sentPacket.getBuffLeft(),(NATIVE_UINT_TYPE)0);

    }

    void ActiveLoggerImplTester::readEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value, Os::File& file) {
        static const BYTE delimiter = 0xA5;

        // first read should be delimeter
        BYTE de;
        NATIVE_INT_TYPE readSize = sizeof(de);

        ASSERT_EQ(file.read((void*)&de,readSize,true),Os::File::OP_OK);
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

    void ActiveLoggerImplTester::runFileDump(void) {

        // Turn on all the filters to verify we get the events

        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_COMMAND,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_DIAGNOSTIC,ActiveLoggerImpl::INPUT_ENABLED);

        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_COMMAND,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_DIAGNOSTIC,ActiveLoggerImpl::SEND_ENABLED);

        // send events to verify that each circular buffer fills correctly

        REQUIREMENT("AL-002");
        // FATAL
        COMMENT("Write FATAL");
        for (NATIVE_UINT_TYPE event = 0; event < FATAL_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_FATAL,FATAL_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < FATAL_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_fatalHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_fatalHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // WARNING HI
        COMMENT("Write WARNING HI");
        for (NATIVE_UINT_TYPE event = 0; event < WARNING_HI_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_WARNING_HI,WARNING_HI_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < WARNING_HI_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_warningHiHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_warningHiHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // WARNING LO
        COMMENT("Write WARNING LO");
        for (NATIVE_UINT_TYPE event = 0; event < WARNING_LO_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_WARNING_LO,WARNING_LO_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < WARNING_LO_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_warningLoHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_warningLoHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // COMMAND
        COMMENT("Write COMMAND");
        for (NATIVE_UINT_TYPE event = 0; event < COMMAND_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_COMMAND,COMMAND_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < COMMAND_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_commandHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_commandHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // ACTIVITY_HI
        COMMENT("Write ACTIVITY HI");
        for (NATIVE_UINT_TYPE event = 0; event < ACTIVITY_HI_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_ACTIVITY_HI,ACTIVITY_HI_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < ACTIVITY_HI_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_activityHiHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_activityHiHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // ACTIVITY_LO
        COMMENT("Write ACTIVITY LO");
        for (NATIVE_UINT_TYPE event = 0; event < ACTIVITY_LO_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_ACTIVITY_LO,ACTIVITY_LO_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < ACTIVITY_LO_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_activityLoHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_activityLoHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // DIAGNOSTIC
        COMMENT("Write DIAGNOSTIC");
        for (NATIVE_UINT_TYPE event = 0; event < DIAGNOSTIC_EVENT_CB_DEPTH; event++) {
            this->writeEvent(event+1,Fw::LOG_DIAGNOSTIC,DIAGNOSTIC_EVENT_CB_DEPTH-event);
            // check for wraparound
            if (event < DIAGNOSTIC_EVENT_CB_DEPTH - 1) {
                ASSERT_EQ(this->m_impl.m_diagnosticHead,event + 1);
            } else {
                ASSERT_EQ(this->m_impl.m_diagnosticHead,(NATIVE_UINT_TYPE)0);
            }
        }

        // dump file
        REQUIREMENT("AL-003");

        this->clearHistory();
        Fw::CmdStringArg file("dump.dat");
        this->sendCmd_ALOG_DUMP_EVENT_LOG(0,12,file);
        // send command
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_DUMP_EVENT_LOG,
                12,
                Fw::COMMAND_OK
                );

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ALOG_FILE_WRITE_ERR_SIZE(0);
        ASSERT_EVENTS_ALOG_FILE_WRITE_COMPLETE(0,
                FATAL_EVENT_CB_DEPTH +
                WARNING_HI_EVENT_CB_DEPTH +
                WARNING_LO_EVENT_CB_DEPTH +
                COMMAND_EVENT_CB_DEPTH +
                ACTIVITY_HI_EVENT_CB_DEPTH +
                ACTIVITY_LO_EVENT_CB_DEPTH +
                DIAGNOSTIC_EVENT_CB_DEPTH);


        // read file back and confirm
        Os::File readFile;
        Os::File::Status stat = readFile.open("dump.dat",Os::File::OPEN_READ);
        ASSERT_EQ(Os::File::OP_OK,stat);

        // FATAL
        COMMENT("Verify FATAL");
        for (NATIVE_UINT_TYPE event = 0; event < FATAL_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_FATAL,FATAL_EVENT_CB_DEPTH-event,readFile);
        }

        // WARNING HI
        COMMENT("Verify WARNING HI");
        for (NATIVE_UINT_TYPE event = 0; event < WARNING_HI_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_WARNING_HI,WARNING_HI_EVENT_CB_DEPTH-event,readFile);
        }

        // WARNING LO
        COMMENT("Verify WARNING LO");
        for (NATIVE_UINT_TYPE event = 0; event < WARNING_LO_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_WARNING_LO,WARNING_LO_EVENT_CB_DEPTH-event,readFile);
        }

        // COMMAND
        COMMENT("Verify COMMAND");
        for (NATIVE_UINT_TYPE event = 0; event < COMMAND_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_COMMAND,COMMAND_EVENT_CB_DEPTH-event,readFile);
        }

        // ACTIVITY_HI
        COMMENT("Verify ACTIVITY_HI");
        for (NATIVE_UINT_TYPE event = 0; event < ACTIVITY_HI_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_ACTIVITY_HI,ACTIVITY_HI_EVENT_CB_DEPTH-event,readFile);
        }

        // ACTIVITY_LO
        COMMENT("Verify ACTIVITY_LO");
        for (NATIVE_UINT_TYPE event = 0; event < ACTIVITY_LO_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_ACTIVITY_LO,ACTIVITY_LO_EVENT_CB_DEPTH-event,readFile);
        }

        // DIAGNOSTIC
        COMMENT("Verify DIAGNOSTIC");
        for (NATIVE_UINT_TYPE event = 0; event < DIAGNOSTIC_EVENT_CB_DEPTH; event++) {
            this->readEvent(event+1,Fw::LOG_DIAGNOSTIC,DIAGNOSTIC_EVENT_CB_DEPTH-event,readFile);
        }

        readFile.close();
    }

    void ActiveLoggerImplTester::runFileDumpErrors(void) {

        // Turn on all the filters to verify we get the events

        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_WARNING_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_COMMAND,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_HI,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_ACTIVITY_LO,ActiveLoggerImpl::INPUT_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_REPORT_FILTER(0,0,ActiveLoggerImpl::INPUT_DIAGNOSTIC,ActiveLoggerImpl::INPUT_ENABLED);

        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_WARNING_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_COMMAND,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_HI,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_ACTIVITY_LO,ActiveLoggerImpl::SEND_ENABLED);
        this->sendCmd_ALOG_SET_EVENT_SEND_FILTER(0,0,ActiveLoggerImpl::SEND_DIAGNOSTIC,ActiveLoggerImpl::SEND_ENABLED);

        // write one event for each type to verify file errors

        // FATAL
        this->writeEvent(1,Fw::LOG_FATAL,10);

        // WARNING HI
        this->writeEvent(2,Fw::LOG_WARNING_HI,11);

        // WARNING LO
        this->writeEvent(3,Fw::LOG_WARNING_LO,12);

        // COMMAND
        this->writeEvent(4,Fw::LOG_COMMAND,13);

        // ACTIVITY_HI
        this->writeEvent(5,Fw::LOG_ACTIVITY_HI,14);

        // ACTIVITY_LO
        this->writeEvent(6,Fw::LOG_ACTIVITY_LO,15);

        // DIAGNOSTIC
        this->writeEvent(7,Fw::LOG_DIAGNOSTIC,16);

        // dump file with error on FATAL

        Os::registerOpenInterceptor(this->OpenInterceptor,static_cast<void*>(this));
        this->m_testOpenStatus = Os::File::DOESNT_EXIST;

        this->clearEvents();
        this->clearHistory();
        Fw::CmdStringArg file("dump.dat");
        this->sendCmd_ALOG_DUMP_EVENT_LOG(0,12,file);
        // send command
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_DUMP_EVENT_LOG,
                12,
                Fw::COMMAND_EXECUTION_ERROR
                );

        this->printTextLogHistory(stdout);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ALOG_FILE_WRITE_ERR_SIZE(1);
        ASSERT_EVENTS_ALOG_FILE_WRITE_ERR(0,ActiveLoggerImpl::LOG_WRITE_OPEN,Os::File::DOESNT_EXIST);

        Os::clearOpenInterceptor();

        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // delimiter is first read

        for (NATIVE_INT_TYPE stage = static_cast<NATIVE_INT_TYPE>(ActiveLoggerImpl::LOG_WRITE_FATAL_DELIMETER);
                stage <= static_cast<NATIVE_INT_TYPE>(ActiveLoggerImpl::LOG_WRITE_DIAGNOSTIC_RECORD); stage++) {

            this->m_writesToWait = stage - 1;
            // set read status to bad
            this->m_testWriteStatus = Os::File::NOT_OPENED;
            // set test type to read error
            this->m_writeTestType = FILE_WRITE_WRITE_ERROR;
            // send command
            this->clearEvents();
            this->clearHistory();
            this->sendCmd_ALOG_DUMP_EVENT_LOG(0,12,file);
            this->m_impl.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(
                    0,
                    ActiveLoggerImpl::OPCODE_ALOG_DUMP_EVENT_LOG,
                    12,
                    Fw::COMMAND_EXECUTION_ERROR
                    );

            this->printTextLogHistory(stdout);

            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_ALOG_FILE_WRITE_ERR_SIZE(1);
            ASSERT_EVENTS_ALOG_FILE_WRITE_ERR(0,static_cast<ActiveLoggerImpl::LogWriteError>(stage),Os::File::NOT_OPENED);
        }

        Os::clearWriteInterceptor();

        // do a normal diagnostic write to get to break case
        this->clearEvents();
        this->clearHistory();
        this->sendCmd_ALOG_DUMP_EVENT_LOG(0,12,file);
        this->m_impl.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(
                0,
                ActiveLoggerImpl::OPCODE_ALOG_DUMP_EVENT_LOG,
                12,
                Fw::COMMAND_OK
                );

        this->printTextLogHistory(stdout);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_ALOG_FILE_WRITE_ERR_SIZE(0);
        ASSERT_EVENTS_ALOG_FILE_WRITE_COMPLETE(0,7);

    }

    bool ActiveLoggerImplTester::OpenInterceptor(Os::File::Status& stat, const char* fileName, Os::File::Mode mode, void* ptr) {
        EXPECT_TRUE(ptr);
        ActiveLoggerImplTester* compPtr = static_cast<ActiveLoggerImplTester*>(ptr);
        stat = compPtr->m_testOpenStatus;
        return false;
    }

    bool ActiveLoggerImplTester::WriteInterceptor(Os::File::Status& stat, const void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr) {
        EXPECT_TRUE(ptr);
        ActiveLoggerImplTester* compPtr = static_cast<ActiveLoggerImplTester*>(ptr);
        if (not compPtr->m_writesToWait--) {
            // check test scenario
            switch (compPtr->m_writeTestType) {
                case FILE_WRITE_WRITE_ERROR:
                    stat = compPtr->m_testWriteStatus;
                    break;
                case FILE_WRITE_SIZE_ERROR:
                    size = compPtr->m_writeSize;
                    stat =  Os::File::OP_OK;
                    break;
                default:
                    EXPECT_TRUE(false);
                    break;
            }
            return false;
        } else {
            return true;
        }
    }

    void ActiveLoggerImplTester::textLogIn(const FwEventIdType id, //!< The event ID
            Fw::Time& timeTag, //!< The time
            const Fw::TextLogSeverity severity, //!< The severity
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
