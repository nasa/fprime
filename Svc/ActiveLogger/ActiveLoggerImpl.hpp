/*
 * ActiveLoggerImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef ACTIVELOGGERIMPL_HPP_
#define ACTIVELOGGERIMPL_HPP_

#include <Svc/ActiveLogger/ActiveLoggerComponentAc.hpp>
#include <Fw/Log/LogPacket.hpp>
#include <ActiveLoggerImplCfg.hpp>

namespace Svc {

    class ActiveLoggerImpl: public ActiveLoggerComponentBase {
        public:
            ActiveLoggerImpl(const char* compName); //!< constructor
            virtual ~ActiveLoggerImpl(); //!< destructor
        PROTECTED:
        PRIVATE:
            void LogRecv_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity& severity, Fw::LogBuffer &args);
            void loqQueue_internalInterfaceHandler(FwEventIdType id, const Fw::Time &timeTag, const Fw::LogSeverity& severity, const Fw::LogBuffer &args);

            void SET_EVENT_FILTER_cmdHandler(
                    FwOpcodeType opCode,
                    U32 cmdSeq,
                    ActiveLogger_FilterSeverity filterLevel,
                    ActiveLogger_Enabled filterEnabled);

            void SET_ID_FILTER_cmdHandler(
                    FwOpcodeType opCode, //!< The opcode
                    U32 cmdSeq, //!< The command sequence number
                    U32 ID,
                    ActiveLogger_Enabled idFilterEnabled //!< ID filter state
                );

            void DUMP_FILTER_STATE_cmdHandler(
                    FwOpcodeType opCode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                );

            //! Handler implementation for pingIn
            //!
            void pingIn_handler(
                const NATIVE_INT_TYPE portNum, /*!< The port number*/
                U32 key /*!< Value to return to pinger*/
            );

            // Filter state
            struct t_filterState {
                ActiveLogger_Enabled enabled; //<! filter is enabled
            } m_filterState[ActiveLogger_FilterSeverity::NUM_CONSTANTS];

            // Working members
            Fw::LogPacket m_logPacket; //!< packet buffer for assembling log packets
            Fw::ComBuffer m_comBuffer; //!< com buffer for sending event buffers

            // array of filtered event IDs.
            // value of 0 means no entry
            FwEventIdType m_filteredIDs[TELEM_ID_FILTER_SIZE];

    };

}
#endif /* ACTIVELOGGERIMPL_HPP_ */
