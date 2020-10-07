/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>

namespace Svc {

    ActiveLoggerImpl::ActiveLoggerImpl(const char* name) : 
        ActiveLoggerComponentBase(name),
        m_fatalHead(0),
        m_warningHiHead(0),
        m_warningLoHead(0),
        m_commandHead(0),
        m_activityHiHead(0),
        m_activityLoHead(0),
        m_diagnosticHead(0)
    {
        // set input filter defaults
        this->m_inFilterState[INPUT_WARNING_HI].enabled =
                INPUT_WARNING_HI_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        this->m_inFilterState[INPUT_WARNING_LO].enabled =
                INPUT_WARNING_LO_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        this->m_inFilterState[INPUT_COMMAND].enabled =
                INPUT_COMMAND_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        this->m_inFilterState[INPUT_ACTIVITY_HI].enabled =
                INPUT_ACTIVITY_HI_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        this->m_inFilterState[INPUT_ACTIVITY_LO].enabled =
                INPUT_ACTIVITY_LO_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        this->m_inFilterState[INPUT_DIAGNOSTIC].enabled =
                INPUT_DIAGNOSTIC_DEFAULT?INPUT_ENABLED:INPUT_DISABLED;
        // set send filter defaults
        this->m_sendFilterState[SEND_WARNING_HI].enabled =
                SEND_WARNING_HI_DEFAULT?SEND_ENABLED:SEND_DISABLED;
        this->m_sendFilterState[SEND_WARNING_LO].enabled =
                SEND_WARNING_LO_DEFAULT?SEND_ENABLED:SEND_DISABLED;
        this->m_sendFilterState[SEND_COMMAND].enabled =
                SEND_COMMAND_DEFAULT?SEND_ENABLED:SEND_DISABLED;
        this->m_sendFilterState[SEND_ACTIVITY_HI].enabled =
                SEND_ACTIVITY_HI_DEFAULT?SEND_ENABLED:SEND_DISABLED;
        this->m_sendFilterState[SEND_ACTIVITY_LO].enabled =
                SEND_ACTIVITY_LO_DEFAULT?SEND_ENABLED:SEND_DISABLED;
        this->m_sendFilterState[SEND_DIAGNOSTIC].enabled =
                SEND_DIAGNOSTIC_DEFAULT?SEND_ENABLED:SEND_DISABLED;

        memset(m_filteredIDs,0,sizeof(m_filteredIDs));

    }

    ActiveLoggerImpl::~ActiveLoggerImpl() {
    }

    void ActiveLoggerImpl::init(
            NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
            NATIVE_INT_TYPE instance /*!< The instance number*/
            ) {
        ActiveLoggerComponentBase::init(queueDepth,instance);
    }

    void ActiveLoggerImpl::LogRecv_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::LogSeverity severity, Fw::LogBuffer &args) {

        // make sure ID is not zero. Zero is reserved for ID filter.
        FW_ASSERT(id != 0);

        switch (severity) {
            case Fw::LOG_FATAL: // always pass FATAL
                break;
            case Fw::LOG_WARNING_HI:
                if (this->m_inFilterState[INPUT_WARNING_HI].enabled == INPUT_DISABLED) {
                   return;
                }
                break;
            case Fw::LOG_WARNING_LO:
                if (this->m_inFilterState[INPUT_WARNING_LO].enabled == INPUT_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_COMMAND:
                if (this->m_inFilterState[INPUT_COMMAND].enabled == INPUT_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_ACTIVITY_HI:
                if (this->m_inFilterState[INPUT_ACTIVITY_HI].enabled == INPUT_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_ACTIVITY_LO:
                if (this->m_inFilterState[INPUT_ACTIVITY_LO].enabled == INPUT_DISABLED) {
                    return;
                }
                break;
            case Fw::LOG_DIAGNOSTIC:
                if (this->m_inFilterState[INPUT_DIAGNOSTIC].enabled == INPUT_DISABLED) {
                    return;
                }
                break;
            default:
                FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(severity));
                return;
        }

        // check ID filters
        for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
            if (
              (m_filteredIDs[entry] == id) &&
              (severity != Fw::LOG_FATAL)
              ) {
                return;
            }
        }

        // send event to the logger thread
        this->loqQueue_internalInterfaceInvoke(id,timeTag,static_cast<QueueLogSeverity>(severity),args);

        // if connected, announce the FATAL
        if (Fw::LOG_FATAL == severity) {
            if (this->isConnected_FatalAnnounce_OutputPort(0)) {
                this->FatalAnnounce_out(0,id);
            }
        }
    }

    void ActiveLoggerImpl::loqQueue_internalInterfaceHandler(FwEventIdType id, Fw::Time &timeTag, QueueLogSeverity severity, Fw::LogBuffer &args) {

        // Serialize event
        this->m_logPacket.setId(id);
        this->m_logPacket.setTimeTag(timeTag);
        this->m_logPacket.setLogBuffer(args);
        this->m_comBuffer.resetSer();
        Fw::SerializeStatus stat = this->m_logPacket.serialize(this->m_comBuffer);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));

        switch (severity) {
            case QUEUE_LOG_FATAL: // always pass FATAL
                this->m_fatalCb[this->m_fatalHead] = this->m_comBuffer;
                this->m_fatalHead = (this->m_fatalHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_fatalCb);
                break;
            case QUEUE_LOG_WARNING_HI:
                this->m_warningHiCb[this->m_warningHiHead] = this->m_comBuffer;
                this->m_warningHiHead = (this->m_warningHiHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_warningHiCb);
                if (this->m_sendFilterState[SEND_WARNING_HI].enabled == SEND_DISABLED) {
                   return;
                }
                break;
            case QUEUE_LOG_WARNING_LO:
                this->m_warningLoCb[this->m_warningLoHead] = this->m_comBuffer;
                this->m_warningLoHead = (this->m_warningLoHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_warningLoCb);
                if (this->m_sendFilterState[SEND_WARNING_LO].enabled == SEND_DISABLED) {
                    return;
                }
                break;
            case QUEUE_LOG_COMMAND:
                this->m_commandCb[this->m_commandHead] = this->m_comBuffer;
                this->m_commandHead = (this->m_commandHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_commandCb);
                if (this->m_sendFilterState[SEND_COMMAND].enabled == SEND_DISABLED) {
                    return;
                }
                break;
            case QUEUE_LOG_ACTIVITY_HI:
                this->m_activityHiCb[this->m_activityHiHead] = this->m_comBuffer;
                this->m_activityHiHead = (this->m_activityHiHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_activityHiCb);
                if (this->m_sendFilterState[SEND_ACTIVITY_HI].enabled == SEND_DISABLED) {
                    return;
                }
                break;
            case QUEUE_LOG_ACTIVITY_LO:
                this->m_activityLoCb[this->m_activityLoHead] = this->m_comBuffer;
                this->m_activityLoHead = (this->m_activityLoHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_activityLoCb);
                if (this->m_sendFilterState[SEND_ACTIVITY_LO].enabled == SEND_DISABLED) {
                    return;
                }
                break;
            case QUEUE_LOG_DIAGNOSTIC:
                this->m_diagnosticCb[this->m_diagnosticHead] = this->m_comBuffer;
                this->m_diagnosticHead = (this->m_diagnosticHead + 1)%FW_NUM_ARRAY_ELEMENTS(this->m_diagnosticCb);
                if (this->m_sendFilterState[SEND_DIAGNOSTIC].enabled == SEND_DISABLED) {
                    return;
                }
                break;
            default:
                FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(severity));
                return;
        }

        if (this->isConnected_PktSend_OutputPort(0)) {
            this->PktSend_out(0, this->m_comBuffer,0);
        }
    }

    void ActiveLoggerImpl::ALOG_SET_EVENT_REPORT_FILTER_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, InputFilterLevel FilterLevel, InputFilterEnabled FilterEnable) {
        if (  (FilterLevel > INPUT_DIAGNOSTIC) or
              (FilterLevel < INPUT_WARNING_HI) or
              (FilterEnable < INPUT_ENABLED) or
              (FilterEnable > INPUT_DISABLED)) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
            return;
        }
        this->m_inFilterState[FilterLevel].enabled = FilterEnable;
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::ALOG_SET_EVENT_SEND_FILTER_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, SendFilterLevel FilterLevel, SendFilterEnabled FilterEnable) {
        if (  (FilterLevel > SEND_DIAGNOSTIC) or
              (FilterLevel < SEND_WARNING_HI) or
              (FilterEnable < SEND_ENABLED) or
              (FilterEnable > SEND_DISABLED)) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
            return;
        }
        this->m_sendFilterState[FilterLevel].enabled = FilterEnable;
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::ALOG_DUMP_EVENT_LOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& filename) {
        // Walk through each event type and write it to the file

        static const BYTE delimiter = 0xA5;
        // open the file
        Os::File file;
        Os::File::Status stat = file.open(filename.toChar(),Os::File::OPEN_WRITE);
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_OPEN,(I32)stat);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        NATIVE_UINT_TYPE numRecords = 0;

        NATIVE_INT_TYPE fileWriteSize;
        // write FATAL
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_fatalCb); entry++) {

            // if there is data, write it
            if (this->m_fatalCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_FATAL_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_fatalCb[entry].getBuffLength();
                stat = file.write(this->m_fatalCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_FATAL_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write WARNING HI
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_warningHiCb); entry++) {
            // if there is data, write it
            if (this->m_warningHiCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_WARNING_HI_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_warningHiCb[entry].getBuffLength();
                stat = file.write(this->m_warningHiCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_WARNING_HI_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write WARNING LO
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_warningLoCb); entry++) {
            // if there is data, write it
            if (this->m_warningLoCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_WARNING_LO_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_warningLoCb[entry].getBuffLength();
                stat = file.write(this->m_warningLoCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_WARNING_LO_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write COMMAND
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_commandCb); entry++) {
            // if there is data, write it
            if (this->m_commandCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_COMMAND_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_commandCb[entry].getBuffLength();
                stat = file.write(this->m_commandCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_COMMAND_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write ACTIVITY HI
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_activityHiCb); entry++) {
            // if there is data, write it
            if (this->m_activityHiCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_ACTIVITY_HI_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_activityHiCb[entry].getBuffLength();
                stat = file.write(this->m_activityHiCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_ACTIVITY_HI_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write ACTIVITY LO
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_activityLoCb); entry++) {
            // if there is data, write it
            if (this->m_activityLoCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_ACTIVITY_LO_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_activityLoCb[entry].getBuffLength();
                stat = file.write(this->m_activityLoCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_ACTIVITY_LO_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        // write DIAGNOSTIC
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_diagnosticCb); entry++) {
            // if there is data, write it
            if (this->m_diagnosticCb[entry].getBuffLength() != 0) {
                // write delimiter
                fileWriteSize = sizeof(delimiter);
                stat = file.write(&delimiter,fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_DIAGNOSTIC_DELIMETER,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }

                // write event record
                fileWriteSize = this->m_diagnosticCb[entry].getBuffLength();
                stat = file.write(this->m_diagnosticCb[entry].getBuffAddr(),fileWriteSize);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_ALOG_FILE_WRITE_ERR(LOG_WRITE_DIAGNOSTIC_RECORD,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    file.close();
                    return;
                }
                numRecords++;
            } else {
                // if the entry is empty, that means it is only a partially filled list, so quit writing
                break;
            }
        }

        file.close();

        this->log_ACTIVITY_HI_ALOG_FILE_WRITE_COMPLETE(numRecords);

        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::ALOG_SET_ID_FILTER_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq, //!< The command sequence number
            U32 ID,
            IdFilterEnabled IdFilterEnable //!< ID filter state
        ) {

        // check parameter
        switch (IdFilterEnable) {
            case ID_ENABLED:
            case ID_DISABLED:
                break;
            default:
                this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
                return;
        }

        if (ID_ENABLED == IdFilterEnable) { // add ID
            // search list for existing entry
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == ID) {
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(ID);
                    return;
                }
            }
            // if not already a match, search for an open slot
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == 0) {
                    this->m_filteredIDs[entry] = ID;
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(ID);
                    return;
                }
            }
            // if an empty slot was not found, send an error event
            this->log_WARNING_LO_ALOG_ID_FILTER_LIST_FULL(ID);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        } else { // remove ID
            // search list for existing entry
            for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
                if (this->m_filteredIDs[entry] == ID) {
                    this->m_filteredIDs[entry] = 0; // zero entry
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
                    this->log_ACTIVITY_HI_ALOG_ID_FILTER_REMOVED(ID);
                    return;
                }
            }
            // if it gets here, wasn't found
            this->log_WARNING_LO_ALOG_ID_FILTER_NOT_FOUND(ID);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
        }

    }

    void ActiveLoggerImpl::ALOG_DUMP_FILTER_STATE_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq //!< The command sequence number
        ) {

        // first, iterate through severity filters
        for (NATIVE_INT_TYPE filter = 0; filter < InputFilterLevel_MAX; filter++) {
            this->log_ACTIVITY_LO_ALOG_SEVERITY_FILTER_STATE(
                    static_cast<EventFilterState>(filter),
                    INPUT_ENABLED == this->m_inFilterState[filter].enabled,
                    SEND_ENABLED == this->m_sendFilterState[filter].enabled
                    );
        }

        // iterate through ID filter
        for (NATIVE_INT_TYPE entry = 0; entry < TELEM_ID_FILTER_SIZE; entry++) {
            if (this->m_filteredIDs[entry] != 0) {
                this->log_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(this->m_filteredIDs[entry]);
            }
        }

        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
    }

    void ActiveLoggerImpl::pingIn_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
        // return key
        this->pingOut_out(0,key);
    }

} // namespace Svc
