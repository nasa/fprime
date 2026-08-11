/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Fw/Logger/Logger.hpp>
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

void EventManager::LogRecv_handler(FwIndexType portNum,
                                   FwEventIdType id,
                                   Fw::Time& timeTag,
                                   const Fw::LogSeverity& severity,
                                   Fw::LogBuffer& args) {
    FW_ASSERT(severity.isValid(), static_cast<FwAssertArgType>(severity.e));

    // Check severity filter (FATAL always passes through)
    if (this->m_severityFilter.isFiltered(severity)) {
        return;
    }

    // check ID filters
    this->m_idFilterLock.lock();
    Fw::Success findStatus = m_filteredIDs.find(id);
    this->m_idFilterLock.unLock();
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
    // A maximum-size LogBuffer plus the packet header can exceed the com buffer capacity.
    // Drop the event rather than asserting, since the arguments may arrive from
    // external sources (e.g. a hub bridging another address space).
    if (Fw::FW_SERIALIZE_OK != stat) {
        Fw::Logger::log("[ERROR] EventManager: dropping event 0x%x (serialize status %d)\n", static_cast<U32>(id),
                        static_cast<I32>(stat));
        return;
    }

    if (this->isConnected_PktSend_OutputPort(0)) {
        this->PktSend_out(0, this->m_comBuffer, 0);
    }
}

void EventManager::SET_EVENT_FILTER_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const FilterSeverity& filterLevel,
                                               const Enabled& filterEnable) {
    // Verify FilterSeverity enum values match EventSeverityFilter index ordering
    static_assert(static_cast<FwSizeType>(FilterSeverity::WARNING_HI) == 0, "FilterSeverity ordering mismatch");
    static_assert(static_cast<FwSizeType>(FilterSeverity::WARNING_LO) == 1, "FilterSeverity ordering mismatch");
    static_assert(static_cast<FwSizeType>(FilterSeverity::COMMAND) == 2, "FilterSeverity ordering mismatch");
    static_assert(static_cast<FwSizeType>(FilterSeverity::ACTIVITY_HI) == 3, "FilterSeverity ordering mismatch");
    static_assert(static_cast<FwSizeType>(FilterSeverity::ACTIVITY_LO) == 4, "FilterSeverity ordering mismatch");
    static_assert(static_cast<FwSizeType>(FilterSeverity::DIAGNOSTIC) == 5, "FilterSeverity ordering mismatch");

    Fw::LogSeverity logSeverity;
    Fw::Success status = EventSeverityFilter::fromIndex(static_cast<FwSizeType>(filterLevel.e), logSeverity);
    if (status != Fw::Success::SUCCESS) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }
    this->m_severityFilter.setFilter(logSeverity, filterEnable.e == Enabled::ENABLED);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EventManager::SET_ID_FILTER_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                            U32 cmdSeq,           //!< The command sequence number
                                            FwEventIdType ID,
                                            const Enabled& idEnabled  //!< ID filter state
) {
    if (Enabled::ENABLED == idEnabled.e) {  // add ID
        this->m_idFilterLock.lock();
        const Fw::Success insertStatus = m_filteredIDs.insert(ID);
        this->m_idFilterLock.unLock();
        if (insertStatus == Fw::Success::SUCCESS) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            this->log_ACTIVITY_HI_ID_FILTER_ENABLED(ID);
        } else {
            // if an empty slot was not found, send an error event
            this->log_WARNING_LO_ID_FILTER_LIST_FULL(ID);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    } else {  // remove ID
        this->m_idFilterLock.lock();
        const Fw::Success removeStatus = m_filteredIDs.remove(ID);
        this->m_idFilterLock.unLock();
        if (removeStatus == Fw::Success::SUCCESS) {
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
        Fw::LogSeverity logSeverity;
        Fw::Success status = EventSeverityFilter::fromIndex(static_cast<FwSizeType>(filter), logSeverity);
        FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(filter));
        this->log_ACTIVITY_LO_SEVERITY_FILTER_STATE(filterState, this->m_severityFilter.isEnabled(logSeverity));
    }

    // Snapshot the ID filter under the lock; log after release since LogRecv is
    // a sync input that may re-enter this component and take the same lock
    FwEventIdType filteredIDs[TELEM_ID_FILTER_SIZE];
    FwSizeType numFilteredIDs = 0;
    this->m_idFilterLock.lock();
    for (FwEventIdType ID : m_filteredIDs) {
        FW_ASSERT(numFilteredIDs < TELEM_ID_FILTER_SIZE, static_cast<FwAssertArgType>(numFilteredIDs));
        filteredIDs[numFilteredIDs] = ID;
        numFilteredIDs++;
    }
    this->m_idFilterLock.unLock();
    for (FwSizeType i = 0; i < numFilteredIDs; i++) {
        this->log_ACTIVITY_HI_ID_FILTER_ENABLED(filteredIDs[i]);
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void EventManager::run_handler(FwIndexType portNum, U32 context) {
    this->tlmWrite_EventsDropped(this->getNumMsgsDropped());
}

void EventManager::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

}  // namespace Svc
