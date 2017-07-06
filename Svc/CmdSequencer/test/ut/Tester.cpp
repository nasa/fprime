// ====================================================================== 
// \title  CmdSequencer.hpp
// \author tim
// \brief  cpp file for CmdSequencer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include "Tester.hpp"
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Utils/Hash/libcrc/lib_crc.h>
#include <Fw/Com/ComPacket.hpp>
#include <Os/Stubs/FileStubs.hpp>
#include <Fw/Test/UnitTest.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

    class FileBuffer: public Fw::SerializeBufferBase {
        public:

            NATIVE_UINT_TYPE getBuffCapacity(void) const {
                return sizeof(m_buff);
            }

            U8* getBuffAddr(void) {
                return m_buff;
            }

            const U8* getBuffAddr(void) const {
                return m_buff;
            }

        private:
            // Set to 1k file size
            U8 m_buff[1024];
    };

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    Tester::Tester(void) :
#if FW_OBJECT_NAMES == 1
      CmdSequencerGTestBase("Tester", MAX_HISTORY_SIZE), 
      component("CmdSequencer"),
#else
      CmdSequencerGTestBase(MAX_HISTORY_SIZE),
      component(),
#endif
      m_readsToWait(0),
      m_readTestType(NO_ERRORS)
    {
        this->initComponents();
        this->connectPorts();
    }

    Tester::~Tester(void) {

    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    Fw::MallocAllocator mallocator;

    void Tester::initializeTest(void) {
        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();
        this->component.deallocateBuffer(mallocator);
        this->component.regCommands();
    }

    void Tester::missingFile(void) {

        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();

        // The sequence file name
        const char * const fileName = "SeqF/bad_file_name.seq";

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileNotFound_SIZE(1);
        ASSERT_EVENTS_CS_FileNotFound(0,fileName);

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 1);

        this->component.deallocateBuffer(mallocator);

    }


    void Tester::badFileCrc(void) {
        this->component.allocateBuffer(100, mallocator, 100);

        REQUIREMENT("CMDS-002");

        const char * const fileName = "SeqF/BadCrcFile.seq";
        Fw::LogStringArg logFileName(fileName);
        // Set time base
        Fw::Time testTime(TB_WORKSTATION_TIME,0,0);
        this->setTestTime(testTime);
        // Write test file
        FileBuffer fb;
        // Write header
        this->serializeHeader(2*sizeof(U32),1,TB_WORKSTATION_TIME, 0, fb);
        // Put data
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, fb.serialize((U32) 0x10));

        // get actual CRC to check event
        U32 realCRC;
        this->component.initCrc(realCRC);
        this->component.updateCRC(realCRC,fb.getBuffAddr(),fb.getBuffLength());
        this->component.finalizeCRC(realCRC);

        // put fake CRC
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, fb.serialize((U32) 0x20));
        // write file.
        this->writeBuffer(fb, fileName);


        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileCrcFailure_SIZE(1);
        ASSERT_EVENTS_CS_FileCrcFailure(0,fileName,0x20,realCRC);

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 1);

        this->component.deallocateBuffer(mallocator);
    }

    void Tester::emptyFile(void) {

        this->component.allocateBuffer(100, mallocator, 100);

        const char * const fileName = "SeqF/empty.seq";
        Fw::LogStringArg logFileName(fileName);

        // Write test file
        FileBuffer fb;
        // write empty file.
        this->writeBuffer(fb, fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0, fileName,
                CmdSequencerComponentBase::SEQ_READ_HEADER_SIZE, 0);

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 1);

        this->component.deallocateBuffer(mallocator);
    }

    void Tester::invalidRecord(void) {

        this->component.allocateBuffer(100, mallocator, 100);

        const char * const fileName = "SeqF/invalid_record.seq";
        Fw::LogStringArg logFileName(fileName);

        Fw::Time testTime(TB_WORKSTATION_TIME,0,0);
        this->setTestTime(testTime);
        // Write test file
        FileBuffer fb;
        // header
        this->serializeHeader(1,1,TB_WORKSTATION_TIME, 0, fb);
        // Add a U8 to make record size too small
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, fb.serialize((U8) 1));
        // write
        this->writeBuffer(fb, fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_from_seqDone_SIZE(0);

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0, fileName, CmdSequencerComponentBase::SEQ_READ_SEQ_CRC, sizeof(U8));

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 1);

        // File shouldn't validate either

        this->sendCmd_CS_Validate(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_from_seqDone_SIZE(0);

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0, fileName, CmdSequencerComponentBase::SEQ_READ_SEQ_CRC, sizeof(U8));

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 2);

        // Port call shouldn't work either
        Fw::EightyCharString fArg(fileName);
        this->invoke_to_seqRunIn(0,fArg);
        this->clearAndDispatch();
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0, fileName, CmdSequencerComponentBase::SEQ_READ_SEQ_CRC, sizeof(U8));

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 3);

        this->component.deallocateBuffer(mallocator);
    }



    void Tester::fileTooLarge(void) {

        this->component.allocateBuffer(100, mallocator, 100);
        const char * const fileName = "SeqF/too_large.seq";
        Fw::LogStringArg logFileName(fileName);

        // Write test file
        FileBuffer fb;
        // header
        this->serializeHeader(0x200, 16, TB_WORKSTATION_TIME,0, fb);
        // write
        this->writeBuffer(fb, fileName);

        Fw::Time testTime(TB_WORKSTATION_TIME,0,0);
        this->setTestTime(testTime);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileSizeError_SIZE(1);
        ASSERT_EVENTS_CS_FileSizeError(0, fileName, 0x200);

        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_Errors_SIZE(1);
        ASSERT_TLM_CS_Errors(0, 1);

        this->component.deallocateBuffer(mallocator);
    }

    void Tester::nominalImmediate(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        REQUIREMENT("CMDS-001");

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        REQUIREMENT("CMDS-003");
        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-1; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // make sure timeout timer is set
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimeoutTimer.m_state);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 2) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
        }

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);


        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::nominalRelative(void) {

        // This test will have three commands that are relative
        // with spaced out relative times, so they will wait for
        // the command time between each command

        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,0,0);
        this->setTestTime(testTime);

        const NATIVE_INT_TYPE numRecs = 3;

        // compute header size field for three records
        U32 seqBuffSize = numRecs*(
                sizeof(U32) + // record size
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                 2*sizeof(U32) // two U32 buffer
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize, numRecs,TB_WORKSTATION_TIME,0, fb);
        // three records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
            Fw::Time t(TB_WORKSTATION_TIME,2,0);
            this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomRel.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check stored timer
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);

        // run one cycle to make sure nothing is dispatched yet
        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();

        ASSERT_from_comCmdOut_SIZE(0);
        ASSERT_EVENTS_SIZE(0);
        // make sure timer hasn't expired
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);

        // check for command buffers
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            // set time to after time of command
            testTime.set(TB_WORKSTATION_TIME,2*rec+3,0);
            this->setTestTime(testTime);
            // run a cycle. Should dispatch timed command
            this->invoke_to_schedIn(0,0);
            this->clearAndDispatch();
            // timer should be cleared
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec < numRecs - 1) {
                ASSERT_EVENTS_SIZE(1); // command complete EVR
                ASSERT_TLM_SIZE(1);
                ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
                // timer should be set for next record
                ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);
            } else {
                ASSERT_EVENTS_SIZE(2); // command + sequence complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                // timer should be cleared
                ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
                // end of sequence should have sent command complete
                // sequence done port
                ASSERT_from_seqDone_SIZE(1);
                ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);
            }
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // no port call
            ASSERT_from_comCmdOut_SIZE(0);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // run a cycle. Should be no output
            this->invoke_to_schedIn(0,0);
            this->clearAndDispatch();
            ASSERT_from_comCmdOut_SIZE(0);
            ASSERT_EVENTS_SIZE(0); // command + sequence complete EVR
            ASSERT_TLM_SIZE(0);

        }

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::nominalTimedRelative(void) {

        // This test will have three commands that are the following:
        // 1) Timed command
        // 2) Immediate command
        // 3) Relative command
        // 4) Immediate command

        this->component.allocateBuffer(100, mallocator, 1*1024);
        this->component.preamble();

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,0);
        this->setTestTime(testTime);

        const NATIVE_INT_TYPE numRecs = 4;

        // compute header size field for three records
        U32 seqBuffSize = numRecs*(
                sizeof(U32) + // record size
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize, numRecs,TB_WORKSTATION_TIME,0, fb);

        // first record - timed command
        Fw::ComBuffer buff;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)1));
        Fw::Time t(TB_WORKSTATION_TIME,2,0);
        this->serializeRecord(CmdSequencerComponentImpl::ABSOLUTE,t,buff,fb);

        // second record - immediate command
        buff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)2));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)3));
        t.set(TB_WORKSTATION_TIME,0,0);
        this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);

        // third record - relative command
        buff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)4));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)5));
        t.set(TB_WORKSTATION_TIME,1,0);
        this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);

        // fourth record - immediate command
        buff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)6));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)7));
        t.set(TB_WORKSTATION_TIME,0,0);
        this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomTimedRel.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        testTime.set(TB_WORKSTATION_TIME,0,0);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check stored timer
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);

        // run one cycle to make sure nothing is dispatched yet
        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();

        ASSERT_from_comCmdOut_SIZE(0);
        ASSERT_EVENTS_SIZE(0);
        // make sure timer hasn't expired
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);
        // check for command buffers

        // 1) *** Absolute command ***
        // Set time to past absolute command
        testTime.set(TB_WORKSTATION_TIME,3,0);
        this->setTestTime(testTime);

        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();
        // timer should be cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // Check command buffer sent
        Fw::ComBuffer comBuff;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)1));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // send status back
        this->invoke_to_cmdResponseIn(0,0,0,Fw::COMMAND_OK);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(1); // command complete EVR
        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_CommandsExecuted(0,1);
        // timer should be clear - no scheduled command
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // look for command complete
        ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
        ASSERT_EVENTS_CS_CommandComplete(0,fileName,0,0);

        // 2) *** Immediate command ***
        // Was an immediate command, so there should be a buffer waiting

        // Check command buffer sent
        comBuff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)2));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)3));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // send status back
        this->invoke_to_cmdResponseIn(0,2,0,Fw::COMMAND_OK);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(1); // command complete EVR
        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_CommandsExecuted(0,2);
        // look for command complete
        ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
        ASSERT_EVENTS_CS_CommandComplete(0,fileName,1,2);

        // timer should be waiting for relative command
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimer.m_state);

        // 3) *** Relative command ***
        // set time to past relative timer
        testTime.set(TB_WORKSTATION_TIME,5,0);
        this->setTestTime(testTime);

        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();
        // timer should be cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // Check command buffer sent
        comBuff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)4));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)5));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // send status back
        this->invoke_to_cmdResponseIn(0,4,0,Fw::COMMAND_OK);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(1); // command complete EVR
        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_CommandsExecuted(0,3);
        // timer should be clear - no scheduled command
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // look for command complete
        ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
        ASSERT_EVENTS_CS_CommandComplete(0,fileName,2,4);

        // 4) *** Immediate command ***
        // timer should be cleared - immediate command
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // Check command buffer sent
        comBuff.resetSer();
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)6));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)7));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // send status back
        this->invoke_to_cmdResponseIn(0,6,0,Fw::COMMAND_OK);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(2); // command complete and sequence complete EVR
        // timer should be clear - no scheduled command
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        // look for command complete
        ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
        ASSERT_EVENTS_CS_CommandComplete(0,fileName,3,6);
        ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);

        ASSERT_TLM_SIZE(2);
        ASSERT_TLM_CS_SequencesCompleted(0,1);
        ASSERT_TLM_CS_CommandsExecuted(0,4);

        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        // run a cycle. Should be no output
        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();
        ASSERT_from_comCmdOut_SIZE(0);
        ASSERT_EVENTS_SIZE(0); // command + sequence complete EVR
        ASSERT_TLM_SIZE(0);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::nominalImmediatePort(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        Fw::EightyCharString fArg(fileName);
        this->invoke_to_seqRunIn(0,fArg);
        this->clearAndDispatch();

        // Shouldn't be a command response for a port call
        ASSERT_CMD_RESPONSE_SIZE(0);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);
        ASSERT_EVENTS_CS_PortSequenceStarted_SIZE(1);
        ASSERT_EVENTS_CS_PortSequenceStarted(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-1; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 2) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
        }

        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::cancelCommand(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        REQUIREMENT("CMDS-005");

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-2; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 2) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
        }

        // send cancel command
        this->sendCmd_CS_Cancel(0,100);
        this->clearAndDispatch();
        // verify cancelled EVR
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceCanceled_SIZE(1);
        ASSERT_EVENTS_CS_SequenceCanceled(0,fileName);
        // Verify state
        ASSERT_EQ(CmdSequencerComponentImpl::STOPPED,this->component.m_runMode);
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::manualImmediate(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        // set mode to manual
        this->sendCmd_CS_Manual(0,10);
        this->clearAndDispatch();
        // should get command complete
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_MANUAL, 10,
                Fw::COMMAND_OK);
        // should be event that switched to manual mode
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_ModeSwitched_SIZE(1);
        ASSERT_EVENTS_CS_ModeSwitched(0,CmdSequencerComponentBase::SEQ_STEP_MODE);

        // send run command. This should validate and load the seqence, then stop
        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // should not be a command buffer
        ASSERT_from_comCmdOut_SIZE(0);

        // send start command to load first command
        this->sendCmd_CS_Start(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_START, 14,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_CmdStarted(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-1; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // make sure timeout timer is NOT set
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 1) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);

            // send step command
            this->sendCmd_CS_Step(0,12);
            this->clearAndDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_STEP, 12,
                    Fw::COMMAND_OK);

            // if last record, step should send no step EVR
            if (rec < numRecs - 2) {
                ASSERT_EVENTS_SIZE(1);
                // second command. First was when it was started.
                ASSERT_EVENTS_CS_CmdStepped(0,fileName,rec+1);
            }
        }

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);


        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        // send step command. Should return error since no active sequence
        this->sendCmd_CS_Step(0,12);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_STEP, 12,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

        // Set back to auto mode
        this->sendCmd_CS_Auto(0,12);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_AUTO, 12,
                Fw::COMMAND_OK);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::manualImmediate2(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will skip end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
            this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        // set mode to manual
        this->sendCmd_CS_Manual(0,10);
        this->clearAndDispatch();
        // should get command complete
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_MANUAL, 10,
                Fw::COMMAND_OK);
        // should be event that switched to manual mode
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_ModeSwitched_SIZE(1);
        ASSERT_EVENTS_CS_ModeSwitched(0,CmdSequencerComponentBase::SEQ_STEP_MODE);

        // send run command. This should validate and load the seqence, then stop
        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // should not be a command buffer
        ASSERT_from_comCmdOut_SIZE(0);

        // send start command to load first command
        this->sendCmd_CS_Start(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_START, 14,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_CmdStarted(0,fileName);

        // check for command buffers.
        for (U32 rec = 0; rec < numRecs; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // make sure timeout timer is NOT set
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 1) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);

            // if last record, step should send no step EVR
            if (rec < numRecs - 1) {
                // send step command
                this->sendCmd_CS_Step(0,12);
                this->clearAndDispatch();
                ASSERT_CMD_RESPONSE_SIZE(1);
                ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_STEP, 12,
                        Fw::COMMAND_OK);

                ASSERT_EVENTS_SIZE(1);
                // second command. First was when it was started.
                ASSERT_EVENTS_CS_CmdStepped(0,fileName,rec+1);
            }
        }

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);


        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);
return;
        // Set back to auto mode
        this->sendCmd_CS_Auto(0,12);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_AUTO, 12,
                Fw::COMMAND_OK);

        this->component.deallocateBuffer(mallocator);

    }


    void Tester::failedCommand(void) {

        // This test will have three commands that are relative
        // with zero time tags, so will immediately execute.
        // The second will fail.

        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();

        REQUIREMENT("CMDS-004");

        const NATIVE_INT_TYPE numRecs = 3;

        // compute header size field for three records
        U32 seqBuffSize = numRecs*(
                sizeof(U32) + // record size
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // three records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers
        for (NATIVE_INT_TYPE rec = 0; rec < 2; rec++) {
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            if (0 == rec) {
                // send good status back
                this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
                this->clearAndDispatch();
                ASSERT_EVENTS_SIZE(1); // command complete EVR
                ASSERT_TLM_SIZE(1);
                ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
                // look for command complete
                ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
                ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
                // telemetry
                ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
                ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            } else {
                // send failed status back
                this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_EXECUTION_ERROR);
                this->clearAndDispatch();
                ASSERT_EVENTS_SIZE(1); // command failed EVR
                ASSERT_EVENTS_CS_CommandError_SIZE(1);
                ASSERT_EVENTS_CS_CommandError(0,fileName,1,rec,Fw::COMMAND_EXECUTION_ERROR);
                ASSERT_TLM_SIZE(1);
                ASSERT_TLM_CS_Errors_SIZE(1);
                ASSERT_TLM_CS_Errors(0,1);
                // end of sequence should have sent command error
                // sequence done port
                ASSERT_from_seqDone_SIZE(1);
                ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_EXECUTION_ERROR);
            }
        }

        // Check to see if the component has cleaned up.
        ASSERT_EQ(CmdSequencerComponentImpl::STOPPED,this->component.m_runMode);
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::pingTest(void) {
        // make sure ping is responded to
        this->invoke_to_pingIn(0,(U32)1234);
        this->clearAndDispatch();
        ASSERT_from_pingOut_SIZE(1);
        ASSERT_from_pingOut(0,(U32)1234);
    }

    void Tester::invalidMode(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-1; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // make sure timeout timer is set
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimeoutTimer.m_state);

            if (0 == rec) {
                // try to send a command to start a sequence
                this->sendCmd_CS_Run(0, 0, fileName);
                this->clearAndDispatch();
                ASSERT_CMD_RESPONSE_SIZE(1);
                ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                        Fw::COMMAND_EXECUTION_ERROR);

                // check loaded EVRs
                ASSERT_EVENTS_SIZE(1);
                ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

                // try to validate file
                this->sendCmd_CS_Validate(0, 0, fileName);
                this->clearAndDispatch();
                ASSERT_CMD_RESPONSE_SIZE(1);
                ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                        Fw::COMMAND_EXECUTION_ERROR);

                // check loaded EVRs
                ASSERT_EVENTS_SIZE(1);
                ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

                // try to invoke sequence port
                Fw::EightyCharString fArg(fileName);
                this->invoke_to_seqRunIn(0,fArg);
                this->clearAndDispatch();
                ASSERT_from_seqDone_SIZE(1);
                ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_EXECUTION_ERROR);

                // check loaded EVRs
                ASSERT_EVENTS_SIZE(1);
                ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

            }

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 2) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
        }

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);


        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester ::
     noSequence(void) {

        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();

        this->sendCmd_CS_Cancel(0,0);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);

        this->component.deallocateBuffer(mallocator);
    }

    void Tester ::
     invalidRecordEntries(void) {

        this->component.allocateBuffer(100, mallocator, 100);
        this->component.preamble();

        // compose various flawed command records

        // Bad descriptor value

        NATIVE_INT_TYPE numRecs = 1;

        // compute header size field for records
        U32 seqBuffSize = numRecs*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
            // force an invalid record descriptor
            this->serializeRecord((CmdSequencerComponentImpl::CmdRecordDescriptor)10,t,buff,fb);
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/BadRecord.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_FORMAT_ERROR);

        // send command to execute the file - same error
        this->sendCmd_CS_Run(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_FORMAT_ERROR);

        // Bad microseconds field - too short

        fb.resetSer();
        // compose various flawed command records
        numRecs = 1;

        // compute header size field for records
        seqBuffSize = numRecs*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds (CRC should land here)
                sizeof(U16) // short microseconds
                );

        // don't add size of CRC - make it land on microseconds field
        // seqBuffSize += sizeof(U32);

        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);

        // fill bogus record

        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U8 )CmdSequencerComponentImpl::RELATIVE));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U16)0));

        // compute and add CRC
        this->serializeCRC(fb);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        testTime.set(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        // send command to execute the file - same error
        this->sendCmd_CS_Run(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        // Bad size field - too short

        fb.resetSer();
        // compose various flawed command records
        numRecs = 1;

        // compute header size field for records
        seqBuffSize = numRecs*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // subseconds (CRC should land here)
                sizeof(U16) // short size
                );

        // don't add size of CRC - make it land on microseconds field
        // seqBuffSize += sizeof(U32);

        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);

        // fill bogus record

        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U8 )CmdSequencerComponentImpl::RELATIVE));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U16)0));

        // compute and add CRC
        this->serializeCRC(fb);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        testTime.set(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        // send command to execute the file - same error
        this->sendCmd_CS_Run(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        // Bad size field - value too large

        fb.resetSer();
        // compose various flawed command records
        numRecs = 1;

        // compute header size field for records
        seqBuffSize = numRecs*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // subseconds
                sizeof(U32) // correct size
                );

        // add size of CRC
        seqBuffSize += sizeof(U32);

        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);

        // fill bogus record

        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U8 )CmdSequencerComponentImpl::RELATIVE));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)50000));

        // compute and add CRC
        this->serializeCRC(fb);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        testTime.set(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        // send command to execute the file - same error
        this->sendCmd_CS_Run(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
        ASSERT_EVENTS_CS_RecordInvalid(0,fileName,0,Fw::FW_DESERIALIZE_SIZE_MISMATCH);

        this->component.deallocateBuffer(mallocator);
    }

    void Tester::invalidSequenceTime(void) {

        {
            // Create sequence where time base does not match
            this->component.allocateBuffer(100, mallocator, 255);
            this->component.preamble();
            this->component.setTimeout(100);

            const NATIVE_INT_TYPE numRecs = 5;

            // compute header size field for records minus end-of-sequence
            U32 seqBuffSize = (numRecs-1)*(
                    sizeof(U8) + // descriptor
                    sizeof(U32) + // seconds
                    sizeof(U32) + // microseconds
                    sizeof(U32) + // record size
                    sizeof(FwPacketDescriptorType) + // command packet descriptor
                    2*sizeof(U32) // two U32 buffer
                    );

            // compute end of sequence record size
            seqBuffSize += sizeof(U8); // descriptor

            // add size of CRC
            seqBuffSize += sizeof(U32);

            FileBuffer fb;
            // header
            this->serializeHeader(seqBuffSize,numRecs,TB_PROC_TIME,0,fb);
            // fill records
            for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
                Fw::ComBuffer buff;
                Fw::Time t(TB_WORKSTATION_TIME,0,0);
                if (rec < numRecs - 1) {
                    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                    this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
                } else { // Make last one end of the sequence
                    this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
                }
            }

            // compute and add CRC
            this->serializeCRC(fb);

            const char* fileName = "SeqF/NomImm.seq";
            Fw::LogStringArg logFileName(fileName);

            // write
            this->writeBuffer(fb, fileName);

            // set time
            Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
            this->setTestTime(testTime);

            // send command to validate file
            this->sendCmd_CS_Validate(0,0,fileName);
            this->clearAndDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                    Fw::COMMAND_EXECUTION_ERROR);
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_CS_TimeBaseMismatch_SIZE(1);
            ASSERT_EVENTS_CS_TimeBaseMismatch(0,fileName,TB_WORKSTATION_TIME,TB_PROC_TIME);
        }

        {
            // Create sequence where time base does not match
            this->component.allocateBuffer(100, mallocator, 255);
            this->component.preamble();
            this->component.setTimeout(100);

            const NATIVE_INT_TYPE numRecs = 5;

            // compute header size field for records minus end-of-sequence
            U32 seqBuffSize = (numRecs-1)*(
                    sizeof(U8) + // descriptor
                    sizeof(U32) + // seconds
                    sizeof(U32) + // microseconds
                    sizeof(U32) + // record size
                    sizeof(FwPacketDescriptorType) + // command packet descriptor
                    2*sizeof(U32) // two U32 buffer
                    );

            // compute end of sequence record size
            seqBuffSize += sizeof(U8); // descriptor

            // add size of CRC
            seqBuffSize += sizeof(U32);

            FileBuffer fb;
            // header
            this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,1,fb);
            // fill records
            for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
                Fw::ComBuffer buff;
                Fw::Time t(TB_WORKSTATION_TIME,0,0);
                if (rec < numRecs - 1) {
                    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                    this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
                } else { // Make last one end of the sequence
                    this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
                }
            }

            // compute and add CRC
            this->serializeCRC(fb);

            const char* fileName = "SeqF/NomImm.seq";
            Fw::LogStringArg logFileName(fileName);

            // write
            this->writeBuffer(fb, fileName);

            // set time
            Fw::Time testTime(TB_WORKSTATION_TIME,0,1,1);
            this->setTestTime(testTime);

            // send command to validate file
            this->sendCmd_CS_Validate(0,0,fileName);
            this->clearAndDispatch();
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                    Fw::COMMAND_EXECUTION_ERROR);
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_CS_TimeContextMismatch_SIZE(1);
            ASSERT_EVENTS_CS_TimeContextMismatch(0,fileName,0,1);
        }

        this->component.deallocateBuffer(mallocator);

    }

    void Tester ::
      unexpectedCompletion(void) {
        // run nominal case

        // This test will send a command completed status after the
        // sequence is done

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        for (U32 rec = 0; rec < numRecs-1; rec++) {
            printf("REC %d\n",rec);
            // Check command buffer sent
            Fw::ComBuffer comBuff;
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)rec));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)(rec+1)));

            ASSERT_from_comCmdOut_SIZE(1);
            ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

            // make sure timeout timer is set
            ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimeoutTimer.m_state);

            // send status back
            this->invoke_to_cmdResponseIn(0,rec,0,Fw::COMMAND_OK);
            this->clearAndDispatch();
            if (rec == numRecs - 2) {
                ASSERT_TLM_SIZE(2);
                ASSERT_TLM_CS_SequencesCompleted(0,1);
                ASSERT_EVENTS_SIZE(2); // command complete EVR
                ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            } else {
                ASSERT_TLM_SIZE(1);
                ASSERT_EVENTS_SIZE(1); // command complete EVR
            }
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
            // look for command complete
            ASSERT_EVENTS_CS_CommandComplete_SIZE(1);
            ASSERT_EVENTS_CS_CommandComplete(0,fileName,rec,rec);
            // telemetry
            ASSERT_TLM_CS_CommandsExecuted_SIZE(1);
            ASSERT_TLM_CS_CommandsExecuted(0,rec+1);
        }

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);


        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_OK);

        // send a completion after the sequence is done.
        // Should send a warning EVR
        // send status back
        this->invoke_to_cmdResponseIn(0,0x10,0,Fw::COMMAND_OK);
        this->clearAndDispatch();
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_UnexpectedCompletion_SIZE(1);
        ASSERT_EVENTS_CS_UnexpectedCompletion(0,0x10);

        this->component.deallocateBuffer(mallocator);

    }


    void Tester::sequenceTimeout(void) {

        // This test will let a sequence timeout

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        REQUIREMENT("CMDS-006");

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        // Check command buffer sent
        Fw::ComBuffer comBuff;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)1));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // make sure timeout timer is set
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimeoutTimer.m_state);

        // Set the test time to be after the timeout
        testTime.set(TB_WORKSTATION_TIME,200,1);
        this->setTestTime(testTime);

        // call the schedule port
        this->invoke_to_schedIn(0,0);
        this->clearAndDispatch();

        // should get the timeout EVR
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceTimeout_SIZE(1);
        ASSERT_EVENTS_CS_SequenceTimeout(0,fileName,0);

        // Verify that the sequencer is idle again
        ASSERT_EQ(CmdSequencerComponentImpl::STOPPED,this->component.m_runMode);
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimer.m_state);
        ASSERT_EQ((U32)0,this->component.m_executedCount);

        // end of sequence should have sent command complete
        // sequence done port
        ASSERT_from_seqDone_SIZE(1);
        ASSERT_from_seqDone(0,(U32)0,(U32)0,Fw::COMMAND_EXECUTION_ERROR);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::invalidManualModes(void) {

        // This test will send manual sequence commands in the middle
        // of a sequence running automatically

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // Attempt to start manual mode without a sequence - should fail

        this->sendCmd_CS_Start(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_START, 14,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_CS_SequenceValid_SIZE(1);
        ASSERT_EVENTS_CS_SequenceValid(0,fileName);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // make sure timeout timer is cleared
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR,this->component.m_cmdTimeoutTimer.m_state);

        this->sendCmd_CS_Run(0, 0, fileName);
        this->clearAndDispatch();

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_RUN, 0,
                Fw::COMMAND_OK);

        // check loaded EVRs
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
        ASSERT_EVENTS_CS_SequenceLoaded(0,fileName);

        // check for command buffers. One less than number of
        // records since last is end-of-sequence
        // Check command buffer sent
        Fw::ComBuffer comBuff;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((FwOpcodeType)Fw::ComPacket::FW_PACKET_COMMAND));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)0));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize((U32)1));

        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0,comBuff,(NATIVE_UINT_TYPE)0);

        // make sure timeout timer is set
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::SET,this->component.m_cmdTimeoutTimer.m_state);

        // Attempt to start a manual sequence - should fail
        this->sendCmd_CS_Start(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_START, 14,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

        // Attempt to go to auto mode - should fail

        this->sendCmd_CS_Auto(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_AUTO, 14,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

        // Attempt to go to manual mode - should fail

        this->sendCmd_CS_Manual(0,14);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_MANUAL, 14,
                Fw::COMMAND_EXECUTION_ERROR);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_InvalidMode_SIZE(1);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::fileLoadErrors(void) {

        // This test will let a sequence timeout

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                );

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor

        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // register file intercepters
        Os::registerOpenInterceptor(this->OpenIntercepter,this);
        // set the open status to failure
        this->m_testOpenStatus = Os::File::DOESNT_EXIST;

        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileNotFound_SIZE(1);
        ASSERT_EVENTS_CS_FileNotFound(0,fileName);

        // different error
        this->m_testOpenStatus = Os::File::NO_PERMISSION;
        this->sendCmd_CS_Validate(0,14,fileName);
        this->clearAndDispatch();
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileReadError_SIZE(1);
        ASSERT_EVENTS_CS_FileReadError(0,fileName);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 14,
                Fw::COMMAND_EXECUTION_ERROR);

        // clear open interceptor
        Os::clearOpenInterceptor();

        // register read interceptor
        Os::registerReadInterceptor(this->ReadIntercepter,this);
        // set the first read to fail
        this->m_readsToWait = 0;
        this->m_testReadStatus = Os::File::NO_SPACE;
        this->m_readTestType = FILE_READ_READ_ERROR;
        Os::setLastError(Os::File::NO_SPACE);

        this->sendCmd_CS_Validate(0,14,fileName);
        this->clearAndDispatch();
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0,fileName,CmdSequencerComponentImpl::SEQ_READ_HEADER,Os::File::NO_SPACE);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 14,
                 Fw::COMMAND_EXECUTION_ERROR);

        // set the second read to fail
        this->m_readsToWait = 1;
        this->m_testReadStatus = Os::File::NO_SPACE;
        this->m_readTestType = FILE_READ_READ_ERROR;
        Os::setLastError(Os::File::NO_SPACE);

        this->sendCmd_CS_Validate(0,14,fileName);
        this->clearAndDispatch();
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0,fileName,CmdSequencerComponentImpl::SEQ_READ_SEQ_DATA,Os::File::NO_SPACE);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 14,
                 Fw::COMMAND_EXECUTION_ERROR);

        // set the second read to fail based on read size
        this->m_readsToWait = 1;
        this->m_testReadStatus = Os::File::OP_OK;
        this->m_readTestType = FILE_READ_SIZE_ERROR;
        this->m_readSize = 2;

        this->sendCmd_CS_Validate(0,14,fileName);
        this->clearAndDispatch();
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
        ASSERT_EVENTS_CS_FileInvalid(0,fileName,CmdSequencerComponentImpl::SEQ_READ_SEQ_DATA_SIZE,2);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 14,
                 Fw::COMMAND_EXECUTION_ERROR);

        this->component.deallocateBuffer(mallocator);

    }

    void Tester::dataAfterRecords(void) {

        // This test will have four commands that are relative
        // with zero time tags, so will immediately execute
        // Will have end-of-sequence entry

        this->component.allocateBuffer(100, mallocator, 255);
        this->component.preamble();
        this->component.setTimeout(100);

        REQUIREMENT("CMDS-001");

        const NATIVE_INT_TYPE numRecs = 5;

        // compute header size field for records minus end-of-sequence
        U32 seqBuffSize = (numRecs-1)*(
                sizeof(U8) + // descriptor
                sizeof(U32) + // seconds
                sizeof(U32) + // microseconds
                sizeof(U32) + // record size
                sizeof(FwPacketDescriptorType) + // command packet descriptor
                2*sizeof(U32) // two U32 buffer
                ); // extra junk data

        // compute end of sequence record size
        seqBuffSize += sizeof(U8); // descriptor
        // add junk data
        seqBuffSize += 2*sizeof(U32);
        // add size of CRC
        seqBuffSize += sizeof(U32);

        FileBuffer fb;
        // header
        this->serializeHeader(seqBuffSize,numRecs,TB_WORKSTATION_TIME,0,fb);
        // fill records
        for (NATIVE_INT_TYPE rec = 0; rec < numRecs; rec++) {
            Fw::ComBuffer buff;
            Fw::Time t(TB_WORKSTATION_TIME,0,0);
            if (rec < numRecs - 1) {
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)rec));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize((U32)(rec + 1)));
                this->serializeRecord(CmdSequencerComponentImpl::RELATIVE,t,buff,fb);
            } else { // Make last one end of the sequence
                this->serializeRecord(CmdSequencerComponentImpl::END_OF_SEQUENCE,t,buff,fb);
            }
        }

        // add junk data
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)0x12345678));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,fb.serialize((U32)0x87654321));

        // compute and add CRC
        this->serializeCRC(fb);

        const char* fileName = "SeqF/NomImm.seq";
        Fw::LogStringArg logFileName(fileName);

        // write
        this->writeBuffer(fb, fileName);

        // set time
        Fw::Time testTime(TB_WORKSTATION_TIME,1,1);
        this->setTestTime(testTime);

        // send command to validate file
        this->sendCmd_CS_Validate(0,0,fileName);
        this->clearAndDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, CmdSequencerComponentBase::OPCODE_CS_VALIDATE, 0,
                Fw::COMMAND_EXECUTION_ERROR);

        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_RecordMismatch_SIZE(1);
        ASSERT_EVENTS_CS_RecordMismatch(0,fileName,5,8);

        this->component.deallocateBuffer(mallocator);

    }


    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------
    void Tester ::
      from_seqDone_handler(
          const NATIVE_INT_TYPE portNum,
          FwOpcodeType opCode,
          U32 cmdSeq,
          Fw::CommandResponse response
      )
    {
      this->pushFromPortEntry_seqDone(opCode, cmdSeq, response);
    }

    void Tester::from_comCmdOut_handler(const NATIVE_INT_TYPE portNum,
            Fw::ComBuffer &data, U32 context) {
        this->pushFromPortEntry_comCmdOut(data, context);
    }

    void Tester ::
      from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_pingOut(key);
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void Tester::connectPorts(void) {

        // cmdResponseIn
        this->connect_to_cmdResponseIn(0,
                this->component.get_cmdResponseIn_InputPort(0));

        // cmdIn
        this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

        // schedIn
        this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));

        // seqRunIn
        this->connect_to_seqRunIn(
            0,
            this->component.get_seqRunIn_InputPort(0)
        );

        // pingIn
        this->connect_to_pingIn(
            0,
            this->component.get_pingIn_InputPort(0)
        );
        // seqDone
        this->component.set_seqDone_OutputPort(
            0,
            this->get_from_seqDone(0)
        );
        // comCmdOut
        this->component.set_comCmdOut_OutputPort(0,
                this->get_from_comCmdOut(0));

        // timeCaller
        this->component.set_timeCaller_OutputPort(0,
                this->get_from_timeCaller(0));

        // tlmOut
        this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

        // logOut
        this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));

        // cmdResponseOut
        this->component.set_cmdResponseOut_OutputPort(0,
                this->get_from_cmdResponseOut(0));

        // cmdRegOut
        this->component.set_cmdRegOut_OutputPort(0,
                this->get_from_cmdRegOut(0));

        // pingOut
        this->component.set_pingOut_OutputPort(
            0,
            this->get_from_pingOut(0)
        );

        // LogText
        this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    }

    void Tester::textLogIn(const FwEventIdType id, //!< The event ID
            Fw::Time& timeTag, //!< The time
            const Fw::TextLogSeverity severity, //!< The severity
            const Fw::TextLogString& text //!< The event string
            ) {
        TextLogEntry e = { id, timeTag, severity, text };

        printTextLogHistoryEntry(e, stdout);
    }

    void Tester::initComponents(void) {
        this->init();
        this->component.init(
        QUEUE_DEPTH, INSTANCE);
    }

    void Tester::writeBuffer(const Fw::SerializeBufferBase& buffer,
            const char* fileName) {

        // Write file
        Os::File file;
        ASSERT_EQ(file.open(fileName, Os::File::OPEN_WRITE), Os::File::OP_OK);
        NATIVE_INT_TYPE size = buffer.getBuffLength();
        ASSERT_EQ(file.write(buffer.getBuffAddr(), size, true),
                Os::File::OP_OK);
        ASSERT_EQ((NATIVE_UINT_TYPE) size, buffer.getBuffLength());
        file.close();
    }

    void Tester::serializeHeader(U32 size,
            U32 records,
            FwTimeBaseStoreType timeBase,
            FwTimeContextStoreType timeContext,
            Fw::SerializeBufferBase& destBuffer) {

        ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(size));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(records));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(timeBase));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(timeContext));

    }

    void Tester::serializeRecord(CmdSequencerComponentImpl::CmdRecordDescriptor desc,
            const Fw::Time &time, const Fw::ComBuffer &buff,
            Fw::SerializeBufferBase& destBuffer) {

        if (CmdSequencerComponentImpl::END_OF_SEQUENCE != desc) {
            // compute record size
            U32 recSize = sizeof(FwPacketDescriptorType) +
                    buff.getBuffLength(); // command buffer

            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize((U8 )desc));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(time.getSeconds()));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(time.getUSeconds()));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(recSize));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize((FwPacketDescriptorType)Fw::ComPacket::FW_PACKET_COMMAND));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize(buff.getBuffAddr(),buff.getBuffLength(),true));
        } else {
            ASSERT_EQ(Fw::FW_SERIALIZE_OK,destBuffer.serialize((U8 )desc));
        }

    }

    void Tester::serializeCRC(Fw::SerializeBufferBase& destBuffer) {
        // get CRC
        U32 CRC;
        Svc::CmdSequencerComponentImpl::initCrc(CRC);
        Svc::CmdSequencerComponentImpl::updateCRC(CRC,destBuffer.getBuffAddr(),destBuffer.getBuffLength());
        Svc::CmdSequencerComponentImpl::finalizeCRC(CRC);
        ASSERT_EQ(destBuffer.serialize(CRC),Fw::FW_SERIALIZE_OK);

    }

    void Tester::clearAndDispatch(void) {
        this->clearHistory();
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->component.doDispatch());
    }

    // borrowed from PrmDb test

    bool Tester::OpenIntercepter(Os::File::Status& stat, const char* fileName, Os::File::Mode mode, void* ptr) {
        EXPECT_TRUE(ptr);
        Tester* compPtr = static_cast<Tester*>(ptr);
        stat = compPtr->m_testOpenStatus;
        return false;
    }

    bool Tester::ReadIntercepter(Os::File::Status& stat, void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr) {
        EXPECT_TRUE(ptr);
        Tester* compPtr = static_cast<Tester*>(ptr);
        bool status;
        if (compPtr->m_readTestType == NO_ERRORS) {
            // No errors: return true
            status = true;
        }
        else if (compPtr->m_readsToWait > 0) {
            // Not time to inject an error yet: decrement wait count
            --compPtr->m_readsToWait;
            status = true;
        }
        else {
            // Time to inject an error: check test scenario
            switch (compPtr->m_readTestType) {
                case FILE_READ_READ_ERROR:
                    stat = compPtr->m_testReadStatus;
                    break;
                case FILE_READ_SIZE_ERROR:
                    size = compPtr->m_readSize;
                    stat =  Os::File::OP_OK;
                    break;
                case FILE_READ_DATA_ERROR:
                    memcpy(buffer,compPtr->m_readData,size);
                    stat =  Os::File::OP_OK;
                    break;
                default:
                    EXPECT_TRUE(false);
                    break;
            }
            status = false;
        }
        return status;
    }


} // end namespace Svc
