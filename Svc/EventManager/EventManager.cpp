/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Svc/EventManager/EventManager.hpp>

namespace Svc {
static_assert(std::numeric_limits<FwSizeType>::max() >= TELEM_ID_FILTER_SIZE,
              "TELEM_ID_FILTER_SIZE must fit within range of FwSizeType");
typedef EventManager_Enabled Enabled;
typedef EventManager_FilterSeverity FilterSeverity;

EventManager::EventManager(const char* name) : EventManagerComponentBase(name), m_severityFilter() {
    // set filter defaults
    this->m_severityFilter.setFilter(Fw::LogSeverity::WARNING_HI, FILTER_WARNING_HI_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::WARNING_LO, FILTER_WARNING_LO_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::COMMAND, FILTER_COMMAND_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::ACTIVITY_HI, FILTER_ACTIVITY_HI_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::ACTIVITY_LO, FILTER_ACTIVITY_LO_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::DIAGNOSTIC, FILTER_DIAGNOSTIC_DEFAULT);
}

EventManager::~EventManager() {}

Fw::LogSeverity EventManager::filterSeverityToLogSeverity(FilterSeverity filterLevel) {
    switch (filterLevel.e) {
        case FilterSeverity::WARNING_HI:
            return Fw::LogSeverity::WARNING_HI;
        case FilterSeverity::WARNING_LO:
            return Fw::LogSeverity::WARNING_LO;
        case FilterSeverity::COMMAND:
            return Fw::LogSeverity::COMMAND;
        case FilterSeverity::ACTIVITY_HI:
            return Fw::LogSeverity::ACTIVITY_HI;
        case FilterSeverity::ACTIVITY_LO:
            return Fw::LogSeverity::ACTIVITY_LO;
        case FilterSeverity::DIAGNOSTIC:
            return Fw::LogSeverity::DIAGNOSTIC;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(filterLevel.e));
            return Fw::LogSeverity::WARNING_HI;
    }
}

void EventManager::LogRecv_handler(FwIndexType portNum,
                                   FwEventIdType id,
                                   Fw::Time& timeTag,
                                   const Fw::LogSeverity& severity,
                                   Fw::LogBuffer& args) {
    // Assert valid severity value
    FW_ASSERT(severity.isValid(), static_cast<FwAssertArgType>(severity.e));

    // Check severity filter (FATAL always passes through)
    if (this->m_severityFilter.isFiltered(severity)) {
        return;
    }

    // check ID filters
    Fw::Success findStatus = m_filteredIDs.find(id);
    if ((findStatus == Fw::Success::SUCCESS) && (severity != Fw::LogSeverity::FATAL)) {
        return;
    }

    // send event to the logger thread
    this->loqQueue_internalInterfaceInvoke(id, timeTag, severity, args);

    // if connected, announce the FATAL
    if (Fw::LogSeverity::FATAL == severity.e) {
        if (this->isConnected_FatalAnnounce_OutputPort(0)) {
            this->FatalAnnounce_out(0, id);
        }
    }
}

void EventManager::loqQueue_internalInterfaceHandler(FwEventIdType id,
                                                     const Fw::Time& timeTag,
                                                     const Fw::LogSeverity& severity,
                                                     const Fw::LogBuffer& args) {
    // Serialize event
    this->m_logPacket.setId(id);
    this->m_logPacket.setTimeTag(timeTag);
    this->m_logPacket.setLogBuffer(args);
    this->m_comBuffer.resetSer();
    Fw::SerializeStatus stat = this->m_logPacket.serializeTo(this->m_comBuffer);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));

    if (this->isConnected_PktSend_OutputPort(0)) {
        this->PktSend_out(0, this->m_comBuffer, 0);
    }
}

void EventManager::SET_EVENT_FILTER_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               FilterSeverity filterLevel,
                                               Enabled filterEnable) {
    Fw::LogSeverity logSeverity = filterSeverityToLogSeverity(filterLevel);
    this->m_severityFilter.setFilter(logSeverity, filterEnable.e == Enabled::ENABLED);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EventManager::SET_ID_FILTER_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                            U32 cmdSeq,           //!< The command sequence number
                                            FwEventIdType ID,
                                            Enabled idEnabled  //!< ID filter state
) {
    if (Enabled::ENABLED == idEnabled.e) {  // add ID
        if (m_filteredIDs.insert(ID) == Fw::Success::SUCCESS) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            this->log_ACTIVITY_HI_ID_FILTER_ENABLED(ID);
        } else {
            // if an empty slot was not found, send an error event
            this->log_WARNING_LO_ID_FILTER_LIST_FULL(ID);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    } else {  // remove ID
        if (m_filteredIDs.remove(ID) == Fw::Success::SUCCESS) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            this->log_ACTIVITY_HI_ID_FILTER_REMOVED(ID);
        } else {
            // Entry wasn't found
            this->log_WARNING_LO_ID_FILTER_NOT_FOUND(ID);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    }
}

void EventManager::DUMP_FILTER_STATE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                                U32 cmdSeq            //!< The command sequence number
) {
    // first, iterate through severity filters
    for (FwEnumStoreType filter = 0; filter < FilterSeverity::NUM_CONSTANTS; filter++) {
        FilterSeverity filterState(static_cast<FilterSeverity::t>(filter));
        Fw::LogSeverity logSeverity = filterSeverityToLogSeverity(filterState);
        this->log_ACTIVITY_LO_SEVERITY_FILTER_STATE(filterState, this->m_severityFilter.isEnabled(logSeverity));
    }

    // iterate through ID filter
    for (FwEventIdType ID : m_filteredIDs) {
        this->log_ACTIVITY_HI_ID_FILTER_ENABLED(ID);
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EventManager::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

}  // namespace Svc
