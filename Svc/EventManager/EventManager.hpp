/*
 * EventManager.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef Svc_EventManager_HPP_
#define Svc_EventManager_HPP_

#include <Fw/DataStructures/ArraySet.hpp>
#include <Fw/Log/LogPacket.hpp>
#include <Os/Mutex.hpp>
#include <Svc/EventManager/EventManagerComponentAc.hpp>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>
#include <atomic>
#include <config/EventManagerCfg.hpp>

namespace Svc {

class EventManager final : public EventManagerComponentBase {
  public:
    EventManager(const char* compName);  //!< constructor
    virtual ~EventManager();             //!< destructor

  private:
    void LogRecv_handler(FwIndexType portNum,
                         FwEventIdType id,
                         Fw::Time& timeTag,
                         const Fw::LogSeverity& severity,
                         Fw::LogBuffer& args) override;

    void loqQueue_internalInterfaceHandler(FwEventIdType id,
                                           const Fw::Time& timeTag,
                                           const Fw::LogSeverity& severity,
                                           const Fw::LogBuffer& args) override;

    void SET_EVENT_FILTER_cmdHandler(FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     const EventManager_FilterSeverity& filterLevel,
                                     const EventManager_Enabled& filterEnabled) override;

    void SET_ID_FILTER_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                  U32 cmdSeq,           //!< The command sequence number
                                  FwEventIdType ID,
                                  const EventManager_Enabled& idFilterEnabled  //!< ID filter state
                                  ) override;

    void DUMP_FILTER_STATE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                      U32 cmdSeq            //!< The command sequence number
                                      ) override;

    //! Handler for rate group port — writes dropped-event telemetry
    void run_handler(FwIndexType portNum, U32 context) override;

    //! Internal interface handler for runInternal
    void runInternal_internalInterfaceHandler() override;

    //! Overflow hook for runInternal
    void runInternal_overflowHook() override;

    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
                        ) override;

    // Severity filter state (shared implementation)
    EventSeverityFilter m_severityFilter;

    // Working members
    Fw::LogPacket m_logPacket;  //!< packet buffer for assembling log packets
    Fw::ComBuffer m_comBuffer;  //!< com buffer for sending event buffers

    // Set of filtered event IDs.
    Fw::ArraySet<FwEventIdType, TELEM_ID_FILTER_SIZE> m_filteredIDs;

    // Guards m_filteredIDs: read on the sync LogRecv path, mutated on the command thread
    Os::Mutex m_idFilterLock;

    // True while a runInternal message is on the queue
    std::atomic<bool> m_runQueued;
};

}  // namespace Svc
#endif /* Svc_EventManager_HPP_ */
