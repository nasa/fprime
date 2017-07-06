// ====================================================================== 
// \title  CmdSequencerImpl.cpp
// \author Bocchino/Canham
// \brief  cpp file for CmdDispatcherComponentBase component implementation class
//
// \copyright
// Copyright (C) 2009-2016 California Institute of Technology.
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

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Svc/CmdSequencer/CmdSequencerImpl.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Types/Serializable.hpp>
extern "C" {
  #include <Utils/Hash/libcrc/lib_crc.h>
}

namespace Svc {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    CmdSequencerComponentImpl::
#if FW_OBJECT_NAMES == 1
    CmdSequencerComponentImpl(const char* name) :
            CmdSequencerComponentBase(name),
#else
            CmdSequencerComponentImpl(void) :
            CmdSequencerComponentBase(),
#endif
            m_numRecords(0),
            m_timeBase(TB_DONT_CARE),
            m_timeContext(FW_CONTEXT_DONT_CARE),
            m_fileCRC(0),
            m_allocatorId(0),
            m_loadCmdCount(0),
            m_cancelCmdCount(0),
            m_errorCount(0),
            m_queueDepth(0),
            m_runMode(STOPPED),
            m_stepMode(AUTO),
            m_executedCount(0),
            m_totalExecutedCount(0),
            m_sequencesCompletedCount(0),
            m_timeout(0)
    {

    }

    void CmdSequencerComponentImpl::init(const NATIVE_INT_TYPE queueDepth,
            const NATIVE_INT_TYPE instance) {
        CmdSequencerComponentBase::init(queueDepth, instance);
    }

    void CmdSequencerComponentImpl::setTimeout(NATIVE_UINT_TYPE timeout) {
        this->m_timeout = timeout;
    }

    void CmdSequencerComponentImpl::allocateBuffer(NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator, NATIVE_UINT_TYPE bytes) {
        // has to be at least as big as a header
        FW_ASSERT(bytes >= SEQ_FILE_HEADER_SIZE);
        this->m_allocatorId = identifier;
        this->m_seqBuffer.setExtBuffer(static_cast<U8*>(allocator.allocate(identifier,bytes)),bytes);
    }

    void CmdSequencerComponentImpl::deallocateBuffer(Fw::MemAllocator& allocator) {
        allocator.deallocate(this->m_allocatorId,this->m_seqBuffer.getBuffAddr());
        this->m_seqBuffer.clear();
    }

    CmdSequencerComponentImpl::~CmdSequencerComponentImpl(void) {

    }

    // ----------------------------------------------------------------------
    // Handler implementations
    // ----------------------------------------------------------------------

    void CmdSequencerComponentImpl::CS_Run_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq,
            const Fw::CmdStringArg& fileName) {

        if (!this->requireRunMode(STOPPED)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // load commands
        if (not this->performCmd_Load(fileName)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        this->m_executedCount = 0;

        // Check the step mode. If it is auto, start the sequence
        if (AUTO == this->m_stepMode) {
            this->m_runMode = RUNNING;
            this->performCmd_Step();
        }

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    void CmdSequencerComponentImpl::CS_Validate_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq, /*!< The command sequence number*/
            const Fw::CmdStringArg& fileName) { /*!< The name of the sequence file*/

        if (!this->requireRunMode(STOPPED)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // load commands
        if (not this->performCmd_Load(fileName)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // reset buffer
        this->m_seqBuffer.resetSer();

        this->log_ACTIVITY_HI_CS_SequenceValid(this->m_logFileName);

        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);

    }

    //! Handler for input port seqRunIn
    void CmdSequencerComponentImpl::seqRunIn_handler(
           NATIVE_INT_TYPE portNum, /*!< The port number*/
           Fw::EightyCharString &filename /*!< The sequence file*/
       ) {

        if (!this->requireRunMode(STOPPED)) {
            this->seqDone_out(0,0,0,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        // load commands
        Fw::CmdStringArg cmdStr(filename);
        if (not this->performCmd_Load(cmdStr)) {
            this->seqDone_out(0,0,0,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        this->m_executedCount = 0;

        // Check the step mode. If it is auto, start the sequence
        if (AUTO == this->m_stepMode) {
            this->m_runMode = RUNNING;
            this->performCmd_Step();
        }

        this->log_ACTIVITY_HI_CS_PortSequenceStarted(this->m_logFileName);
    }

    void CmdSequencerComponentImpl::CS_Cancel_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq) {
        if (RUNNING == this->m_runMode) {
            this->performCmd_Cancel();
            Fw::LogStringArg logStringArg(this->m_fileName);
            this->log_ACTIVITY_HI_CS_SequenceCanceled(logStringArg);
            ++this->m_cancelCmdCount;
            this->tlmWrite_CS_CancelCommands(this->m_cancelCmdCount);
        } else {
            this->log_WARNING_LO_CS_NoSequenceActive();
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }

    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    void CmdSequencerComponentImpl::error(void) {
        ++this->m_errorCount;
        this->m_fileName = "";
        this->m_logFileName = "";
        this->tlmWrite_CS_Errors(m_errorCount);
    }

    void CmdSequencerComponentImpl::eventFileInvalid(FileReadStage stage, I32 error) {
        this->log_WARNING_HI_CS_FileInvalid(this->m_logFileName,stage,error);
        this->error();
    }

    void CmdSequencerComponentImpl::eventTimeBaseMismatch(U32 currTimeBase, U32 seqTimeBase) {
        this->log_WARNING_HI_CS_TimeBaseMismatch(this->m_logFileName,currTimeBase,seqTimeBase);
        this->error();
    }

    void CmdSequencerComponentImpl::eventTimeContextMismatch(U32 currTimeContext, U32 seqTimeContext) {
        this->log_WARNING_HI_CS_TimeContextMismatch(this->m_logFileName,currTimeContext,seqTimeContext);
        this->error();
    }

    void CmdSequencerComponentImpl::eventFileReadError(void) {
        this->log_WARNING_HI_CS_FileReadError(this->m_logFileName);
        this->error();
    }

    void CmdSequencerComponentImpl::eventFileSizeError(U32 size) {
        this->log_WARNING_HI_CS_FileSizeError(this->m_logFileName,size);
        this->error();
    }

    void CmdSequencerComponentImpl::eventRecordInvalid(U32 n, I32 error) {
        this->log_WARNING_HI_CS_RecordInvalid(this->m_logFileName, n, error);
        this->error();
    }

    void CmdSequencerComponentImpl::performCmd_Cancel(void) {
        this->m_seqBuffer.resetDeser();
        this->m_runMode = STOPPED;
        this->m_cmdTimer.clear();
        this->m_cmdTimeoutTimer.clear();
        this->m_executedCount = 0;
        // write sequence done port with error, if connected
        if (this->isConnected_seqDone_OutputPort(0)) {
            this->seqDone_out(0,0,0,Fw::COMMAND_EXECUTION_ERROR);
        }

    }

    bool CmdSequencerComponentImpl::performCmd_Load(const Fw::CmdStringArg& fileName) {

        // make sure there is a buffer allocated
        FW_ASSERT(this->m_seqBuffer.getBuffAddr());

        this->m_fileName = fileName;
        // copy for events
        this->m_logFileName = fileName;

        if (!this->readSequenceFile()) {
            return false;
        }

        if (!this->validateBuffer()) {
            return false;
        }

        this->log_ACTIVITY_LO_CS_SequenceLoaded(this->m_logFileName);
        ++this->m_loadCmdCount;
        this->tlmWrite_CS_LoadCommands(this->m_loadCmdCount);

        return true;

    }

    bool CmdSequencerComponentImpl::readSequenceFile(void) {

        bool result;

        Os::File::Status status = this->m_sequenceFile.open(
                this->m_fileName.toChar(), Os::File::OPEN_READ);

        if (status == Os::File::OP_OK) {
            result = this->readOpenSequenceFile();
        } else if (status == Os::File::DOESNT_EXIST) {
            this->log_WARNING_HI_CS_FileNotFound(this->m_logFileName);
            this->error();
            result = false;
        } else {
            this->eventFileReadError();
            result = false;
        }

        this->m_sequenceFile.close();
        return result;

    }

    bool CmdSequencerComponentImpl::readOpenSequenceFile(void) {

        U32 fileSize;

        NATIVE_INT_TYPE readLen = SEQ_FILE_HEADER_SIZE;
        // temporarily borrow sequence buffer for header
        Os::File::Status status = this->m_sequenceFile.read(this->m_seqBuffer.getBuffAddr(),readLen);

        // check read status
        if (status != Os::File::OP_OK) {
            this->eventFileInvalid(SEQ_READ_HEADER,this->m_sequenceFile.getLastError());
            return false;
        }

        // check read size
        if (SEQ_FILE_HEADER_SIZE != readLen) {
            this->eventFileInvalid(SEQ_READ_HEADER_SIZE,readLen);
            return false;
        }

        // set buffer length to read size
        FW_ASSERT(Fw::FW_SERIALIZE_OK == this->m_seqBuffer.setBuffLen(readLen));

        // deserialize size of file
        Fw::SerializeStatus stat = this->m_seqBuffer.deserialize(fileSize);
        if (stat != Fw::FW_SERIALIZE_OK) {
            this->eventFileInvalid(SEQ_DESER_SIZE,stat);
            return false;
        }

        if (fileSize > this->m_seqBuffer.getBuffCapacity()) {
            this->eventFileSizeError(fileSize);
            return false;
        }

        // deserialize number of records
        stat = this->m_seqBuffer.deserialize(this->m_numRecords);
        if (stat != Fw::FW_SERIALIZE_OK) {
            this->eventFileInvalid(SEQ_DESER_NUM_RECORDS,stat);
            return false;
        }

        // deserialize time base of sequence
        FwTimeBaseStoreType tbase;
        stat = this->m_seqBuffer.deserialize(tbase);
        if (stat != Fw::FW_SERIALIZE_OK) {
            this->eventFileInvalid(SEQ_DESER_TIME_BASE,stat);
            return false;
        }

        this->m_timeBase = static_cast<TimeBase>(tbase);

        // check time base
        Fw::Time check = this->getTime();
        if (
                (check.getTimeBase() != this->m_timeBase) and
                (this->m_timeBase != TB_DONT_CARE)
            ) {
            this->eventTimeBaseMismatch(check.getTimeBase(),this->m_timeBase);
            return false;
        }
        // set to actual time base for later
        this->m_timeBase = check.getTimeBase();

        // deserialize time context
        stat = this->m_seqBuffer.deserialize(this->m_timeContext);
        if (stat != Fw::FW_SERIALIZE_OK) {
            this->eventFileInvalid(SEQ_DESER_TIME_CONTEXT,stat);
            return false;
        }

        // check time context
        if (
                (check.getContext() != this->m_timeContext) and
                (this->m_timeContext != FW_CONTEXT_DONT_CARE)
           ) {
            this->eventTimeContextMismatch(check.getContext(),this->m_timeContext);
            return false;
        }

        // set to actual time context for later
        this->m_timeContext = check.getContext();

        // compute CRC over header
        this->initCrc(this->m_fileCRC);

        this->updateCRC(this->m_fileCRC,this->m_seqBuffer.getBuffAddr(),SEQ_FILE_HEADER_SIZE);

        // read sequence data

        readLen = fileSize;
        status = this->m_sequenceFile.read(this->m_seqBuffer.getBuffAddr(),readLen);
        // check read status
        if (status != Os::File::OP_OK) {
            this->eventFileInvalid(SEQ_READ_SEQ_DATA,this->m_sequenceFile.getLastError());
            return false;
        }

        // check read size
        if ((NATIVE_INT_TYPE)fileSize != readLen) {
            this->eventFileInvalid(SEQ_READ_SEQ_DATA_SIZE,readLen);
            return false;
        }

        // set sequence buffer size
        FW_ASSERT(Fw::FW_SERIALIZE_OK == this->m_seqBuffer.setBuffLen(fileSize));

        // compute CRC over rest of file. Will be checked in validateBuffer()

        // buffer should be at least as large as a CRC
        if (this->m_seqBuffer.getBuffLength() < sizeof(this->m_fileCRC)) {
            this->eventFileInvalid(SEQ_READ_SEQ_CRC,this->m_seqBuffer.getBuffLength());
            return false;
        } else {
            this->updateCRC(this->m_fileCRC,
                    this->m_seqBuffer.getBuffAddr(),
                    fileSize-sizeof(this->m_fileCRC));

            // finalize CRC
            this->finalizeCRC(this->m_fileCRC);
            return true;
        }


    }

    bool CmdSequencerComponentImpl::validateBuffer(void) {

        // validate CRC

        // Get CRC from the data file
        U32 storedCrc;

        // point at CRC bytes
        Fw::ExternalSerializeBuffer crcBuff(&this->m_seqBuffer.getBuffAddr()[this->m_seqBuffer.getBuffLength()-sizeof(storedCrc)],
                sizeof(storedCrc));
        // set buffer size to length of CRC
        FW_ASSERT(Fw::FW_SERIALIZE_OK == crcBuff.setBuffLen(sizeof(storedCrc)));

        // deserialize CRC value
        Fw::SerializeStatus stat = crcBuff.deserialize(storedCrc);
        FW_ASSERT(stat == Fw::FW_SERIALIZE_OK,stat);

        // compare CRC
        if (storedCrc != this->m_fileCRC) {
            this->log_WARNING_HI_CS_FileCrcFailure(this->m_logFileName,storedCrc,this->m_fileCRC);
            this->error();
            return false;
        }

        CmdRecord rec;

        for (NATIVE_UINT_TYPE record = 0; record < this->m_numRecords; record++) {
            Fw::SerializeStatus stat = rec.deserialize(this->m_seqBuffer);
            if (stat != Fw::FW_SERIALIZE_OK) {
                this->eventRecordInvalid(record,stat);
                return false;
            }
        }

        // there should only be the CRC left
        if (this->m_seqBuffer.getBuffLeft() != sizeof(this->m_fileCRC)) {
            this->log_WARNING_HI_CS_RecordMismatch(this->m_logFileName,this->m_numRecords,this->m_seqBuffer.getBuffLeft()-sizeof(this->m_fileCRC));
            return false;
        }

        // rewind deserialization
        this->m_seqBuffer.resetDeser();
        // set buffer to size minus CRC now that it's validated
        FW_ASSERT(Fw::FW_SERIALIZE_OK == this->m_seqBuffer.setBuffLen(this->m_seqBuffer.getBuffLength()-sizeof(this->m_fileCRC)));

        return true;

    }

    void CmdSequencerComponentImpl::cmdResponseIn_handler(
            NATIVE_INT_TYPE portNum,
            FwOpcodeType opcode, U32 cmdSeq, Fw::CommandResponse response) {
        // check to see if sequencer is running

        if (this->m_runMode == STOPPED) {
            this->log_WARNING_HI_CS_UnexpectedCompletion(opcode);
        } else {
            // clear command timeout
            this->m_cmdTimeoutTimer.clear();
            if (response != Fw::COMMAND_OK) {
                this->commandError(this->m_executedCount, opcode, response);
                this->performCmd_Cancel();
            } else if (this->m_runMode == RUNNING && this->m_stepMode == AUTO) {
                this->commandComplete(opcode);
                // check to see if there is any data left
                if (this->m_seqBuffer.getBuffLeft() == 0) {
                    this->m_runMode = STOPPED;
                    this->sequenceComplete();
                } else {
                    this->performCmd_Step();
                }
            } else { // manual step mode
                this->commandComplete(opcode);
                if (this->m_seqBuffer.getBuffLeft() == 0) {
                    this->m_runMode = STOPPED;
                    this->sequenceComplete();
                }
            }
        }
    }

    void CmdSequencerComponentImpl::schedIn_handler(NATIVE_INT_TYPE portNum,
            NATIVE_UINT_TYPE order) {

        Fw::Time currTime = this->getTime();
        // check to see if a command time is pending
        if (this->m_cmdTimer.isExpiredAt(currTime)) {
            this->comCmdOut_out(0, m_cmdRecord.m_command, 0);
            this->m_cmdTimer.clear();
            // start command timeout timer
            this->setCmdTimeout(currTime);
        } else if (this->m_cmdTimeoutTimer.isExpiredAt(this->getTime())) { // check for command timeout
            this->log_WARNING_HI_CS_SequenceTimeout(m_logFileName,this->m_executedCount);
            // If there is a command timeout, cancel the sequence
            this->performCmd_Cancel();
        }
    }

    void CmdSequencerComponentImpl::CS_Start_cmdHandler(
            FwOpcodeType opcode, U32 cmdSeq) {

        // make sure a sequence is loaded
        if (this->m_seqBuffer.getBuffLength() == 0) {
            this->log_WARNING_LO_CS_NoSequenceActive();
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        if (!this->requireRunMode(STOPPED)) {
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
            return;
        }
        this->m_runMode = RUNNING;
        this->performCmd_Step();
        this->log_ACTIVITY_HI_CS_CmdStarted(this->m_logFileName);
        this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_OK);
    }

    void CmdSequencerComponentImpl::CS_Step_cmdHandler(
            FwOpcodeType opcode, U32 cmdSeq) {
        if (this->requireRunMode(RUNNING)) {
            this->performCmd_Step();
            // check for special case where end of sequence entry was encountered
            if (this->m_runMode != STOPPED) {
                this->log_ACTIVITY_HI_CS_CmdStepped(this->m_logFileName,this->m_executedCount);
            }
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        }
    }

    void CmdSequencerComponentImpl::CS_Auto_cmdHandler(
            FwOpcodeType opcode, U32 cmdSeq) {
        if (this->requireRunMode(STOPPED)) {
            this->m_stepMode = AUTO;
            this->log_ACTIVITY_HI_CS_ModeSwitched(SEQ_AUTO_MODE);
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        }
    }

    void CmdSequencerComponentImpl::CS_Manual_cmdHandler(
            FwOpcodeType opcode, U32 cmdSeq) {
        if (this->requireRunMode(STOPPED)) {
            this->m_stepMode = MANUAL;
            this->log_ACTIVITY_HI_CS_ModeSwitched(SEQ_STEP_MODE);
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        }
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    bool CmdSequencerComponentImpl::requireRunMode(RunMode mode) {
        if (this->m_runMode == mode) {
            return true;
        } else {
            this->log_WARNING_HI_CS_InvalidMode();
            return false;
        }
    }

    void CmdSequencerComponentImpl::commandError(const U32 number,
            const U32 opCode, const U32 error) {
        Fw::LogStringArg fileStr(this->m_fileName);
        this->log_WARNING_HI_CS_CommandError(fileStr,number, opCode, error);
        this->error();
    }

    void CmdSequencerComponentImpl::performCmd_Step(void) {

        // deserialize next record. The buffer was previously validated,
        // so a failure here should be asserted

        Fw::SerializeStatus stat = this->m_cmdRecord.deserialize(this->m_seqBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,stat);
        // set clock time base and context from value set when sequence was loaded
        this->m_cmdRecord.m_timeTag.setTimeBase(this->m_timeBase);
        this->m_cmdRecord.m_timeTag.setTimeContext(this->m_timeContext);

        Fw::Time currentTime = this->getTime();
        switch (this->m_cmdRecord.m_descriptor) {
            case END_OF_SEQUENCE:
                this->m_runMode = STOPPED;
                this->sequenceComplete();
                break;
            case RELATIVE:
                this->performCmd_Step_RELATIVE(currentTime);
                break;
            case ABSOLUTE:
                this->performCmd_Step_ABSOLUTE(currentTime);
                break;
            default:
                FW_ASSERT(0, m_cmdRecord.m_descriptor);
        }
    }

    void CmdSequencerComponentImpl::sequenceComplete(void) {
        ++this->m_sequencesCompletedCount;
        // reset buffer
        this->m_seqBuffer.resetSer();
        Fw::LogStringArg fileStr(this->m_fileName);
        this->log_ACTIVITY_HI_CS_SequenceComplete(fileStr);
        this->tlmWrite_CS_SequencesCompleted(this->m_sequencesCompletedCount);
        this->m_executedCount = 0;
        // write sequence done port, if connected
        if (this->isConnected_seqDone_OutputPort(0)) {
            this->seqDone_out(0,0,0,Fw::COMMAND_OK);
        }
    }

    void CmdSequencerComponentImpl::commandComplete(const U32 opcode) {
        Fw::LogStringArg fileStr(this->m_fileName);
        this->log_ACTIVITY_LO_CS_CommandComplete(fileStr,this->m_executedCount, opcode);
        ++this->m_executedCount;
        ++this->m_totalExecutedCount;
        this->tlmWrite_CS_CommandsExecuted(this->m_totalExecutedCount);
    }

    void CmdSequencerComponentImpl::performCmd_Step_RELATIVE(
            Fw::Time& currentTime) {

        this->m_cmdRecord.m_timeTag.add(currentTime.getSeconds(),currentTime.getUSeconds());
        this->performCmd_Step_ABSOLUTE(currentTime);
    }

    void CmdSequencerComponentImpl::performCmd_Step_ABSOLUTE(
            Fw::Time& currentTime) {

        if (currentTime >= this->m_cmdRecord.m_timeTag) {
            this->comCmdOut_out(0, m_cmdRecord.m_command, 0);
            this->setCmdTimeout(currentTime);
        } else {
            this->m_cmdTimer.set(this->m_cmdRecord.m_timeTag);
        }
    }

    void CmdSequencerComponentImpl::preamble(void){
        // make sure a buffer has been allocated
        FW_ASSERT(this->m_seqBuffer.getBuffAddr());
        FW_ASSERT(this->m_seqBuffer.getBuffCapacity());
    }

    Fw::SerializeStatus CmdSequencerComponentImpl::CmdRecord::serialize(Fw::SerializeBufferBase& buffer) const {

        // shouldn't ever be serialized
        FW_ASSERT(0);
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    Fw::SerializeStatus CmdSequencerComponentImpl::CmdRecord::deserialize(Fw::SerializeBufferBase& buffer) {

        U8 descEntry;

        // read the descriptor
        Fw::SerializeStatus stat = buffer.deserialize(descEntry);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        // check the descriptor value
        switch (descEntry) {
            case ABSOLUTE...END_OF_SEQUENCE: // okay
                break;
            default:
                return Fw::FW_DESERIALIZE_FORMAT_ERROR;
        }
        // cast it
        this->m_descriptor = static_cast<CmdRecordDescriptor>(descEntry);
        // if the descriptor is END_OF_SEQUENCE, quit
        if (END_OF_SEQUENCE == this->m_descriptor) {
            return Fw::FW_SERIALIZE_OK;
        }
        // next is time field

        U32 seconds;
        stat = buffer.deserialize(seconds);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }

        U32 useconds;

        stat = buffer.deserialize(useconds);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }

        // set time value for record
        this->m_timeTag.set(seconds,useconds);

        // read the command buffer record size
        U32 recordSize;
        stat = buffer.deserialize(recordSize);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        // validate the record size. It shouldn't be longer than the remaining buffer
        // or longer than destination ComBuffer

        if (
                (recordSize > buffer.getBuffLeft()) ||
                (recordSize > Fw::ComBuffer::SERIALIZED_SIZE - sizeof(FwPacketDescriptorType))

            ) {

            return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
        }

        // next is command buffer
        this->m_command.resetSer();

        stat = buffer.deserialize(this->m_command.getBuffAddr(),recordSize,true);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        // set length of command buffer
        FW_ASSERT(Fw::FW_SERIALIZE_OK == this->m_command.setBuffLen(recordSize));

        return Fw::FW_SERIALIZE_OK;
    }

    void CmdSequencerComponentImpl::pingIn_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 key /*!< Value to return to pinger*/
        ) {
        // send ping response
        this->pingOut_out(0,key);
    }

    //! initialize CRC
    void CmdSequencerComponentImpl::initCrc(U32 &crc) {
        crc = 0xFFFFFFFF;
    }

    //! update CRC
    void CmdSequencerComponentImpl::updateCRC(U32 &crc, const BYTE* buffer, NATIVE_UINT_TYPE bufferSize) {
        FW_ASSERT(buffer);
        for(NATIVE_UINT_TYPE index = 0; index < bufferSize; index++) {
            crc = update_crc_32(crc, buffer[index]);
        }
    }

    //! finalize CRC
    void CmdSequencerComponentImpl::finalizeCRC(U32 &crc) {
        crc = ~crc;
    }

    void CmdSequencerComponentImpl::setCmdTimeout(const Fw::Time &currentTime) {
        // start timeout timer if enabled and not in step mode
        if ((this->m_timeout > 0) and (AUTO == this->m_stepMode)) {
            Fw::Time expTime = currentTime;
            expTime.add(this->m_timeout,0);
            this->m_cmdTimeoutTimer.set(expTime);
        }
    }


}

