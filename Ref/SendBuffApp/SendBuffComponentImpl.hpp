#ifndef REF_LPS_ATM_IMPL_HPP
#define REF_LPS_ATM_IMPL_HPP

#include <Ref/SendBuffApp/SendBuffComponentAc.hpp>

namespace Ref {

    /// This component sends a data buffer to a driver each time it is invoked by a scheduler

    class SendBuffImpl : public SendBuffComponentBase {
        public:

            // Only called by derived class
            SendBuffImpl(const char* compName); //!< constructor
            ~SendBuffImpl(); //!< destructor

        private:

            void SchedIn_handler(NATIVE_INT_TYPE portNum, U32 context); //!< downcall for input port
            void SB_START_PKTS_cmdHandler(FwOpcodeType opcode, U32 cmdSeq); //!< START_PKTS command handler
            void SB_INJECT_PKT_ERROR_cmdHandler(FwOpcodeType opcode, U32 cmdSeq); //!< START_PKTS command handler
            void SB_GEN_FATAL_cmdHandler(
                    FwOpcodeType opCode, /*!< The opcode*/
                    U32 cmdSeq, /*!< The command sequence number*/
                    U32 arg1, /*!< First FATAL Argument*/
                    U32 arg2, /*!< Second FATAL Argument*/
                    U32 arg3 /*!< Third FATAL Argument*/
                );

                //! Handler for command SB_GEN_ASSERT
                /* Generate an ASSERT */
             void SB_GEN_ASSERT_cmdHandler(
                    FwOpcodeType opCode, /*!< The opcode*/
                    U32 cmdSeq, /*!< The command sequence number*/
                    U32 arg1, /*!< First ASSERT Argument*/
                    U32 arg2, /*!< Second ASSERT Argument*/
                    U32 arg3, /*!< Third ASSERT Argument*/
                    U32 arg4, /*!< Fourth ASSERT Argument*/
                    U32 arg5, /*!< Fifth ASSERT Argument*/
                    U32 arg6 /*!< Sixth ASSERT Argument*/
                );
            U32 m_invocations; //!< number of times component has been called by the scheduler
            U32 m_buffsSent; //!< number of buffers sent
            U32 m_errorsInjected; //!< number of errors injected
            bool m_injectError; //!< flag to inject error on next packet
            bool m_sendPackets; //!< If true, send packets
            U32 m_currPacketId; //!< current packet ID to be sent
            bool m_firstPacketSent; //!< set if first packet

            void toString(char* str, I32 buffer_size); //!< writes a string representation of the object

            // buffer data
            Drv::DataBuffer m_testBuff; //!< data buffer to send

            // parameter update notification
            void parameterUpdated(FwPrmIdType id); //!< Notification function for changed parameters

            // send state
            SendBuff_ActiveState m_state;
};

}

#endif
