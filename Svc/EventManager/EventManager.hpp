/*
 * EventManager.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef Svc_EventManager_HPP_
#define Svc_EventManager_HPP_

#include <Fw/Log/LogPacket.hpp>
#include <Svc/EventManager/EventManagerComponentAc.hpp>
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
                         Fw::LogBuffer& args);

    void loqQueue_internalInterfaceHandler(FwEventIdType id,
                                           const Fw::Time& timeTag,
                                           const Fw::LogSeverity& severity,
                                           const Fw::LogBuffer& args);

    void SET_EVENT_FILTER_cmdHandler(FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     EventManager_FilterSeverity filterLevel,
                                     EventManager_Enabled filterEnabled);

    void SET_ID_FILTER_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                  U32 cmdSeq,           //!< The command sequence number
                                  FwEventIdType ID,
                                  EventManager_Enabled idFilterEnabled  //!< ID filter state
    );

    void DUMP_FILTER_STATE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                      U32 cmdSeq            //!< The command sequence number
    );

    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
    );

    // Filter state
    struct t_filterState {
        EventManager_Enabled enabled;  //<! filter is enabled
    } m_filterState[EventManager_FilterSeverity::NUM_CONSTANTS];

    // Working members
    Fw::LogPacket m_logPacket;  //!< packet buffer for assembling log packets
    Fw::ComBuffer m_comBuffer;  //!< com buffer for sending event buffers

    // array of filtered event IDs.
    // value of 0 means no entry
    FwEventIdType m_filteredIDs[TELEM_ID_FILTER_SIZE];
};

}  // namespace Svc
#endif /* Svc_EventManager_HPP_ */
