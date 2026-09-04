/*
 * CommandDispatcherTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef CMDDISP_TEST_UT_TLMCHANIMPLTESTER_HPP_
#define CMDDISP_TEST_UT_TLMCHANIMPLTESTER_HPP_

#include <CommandDispatcherGTestBase.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>

namespace Svc {

class CommandDispatcherTester : public CommandDispatcherGTestBase {
  public:
    CommandDispatcherTester(Svc::CommandDispatcherImpl& inst);
    virtual ~CommandDispatcherTester();

    void runNominalDispatch();
    void runInvalidOpcodeDispatch();
    void runCommandReregister();
    void runFailedCommand();
    void runInvalidCommand();
    void runOverflowCommands();
    void runNopCommands();
    void runClearCommandTracking();
    void runCommandQueueOverflow();
    void runNonZeroPortDispatch();

    void registerBuiltinCommands();

    void setSequenceNumber(U32 sequenceNumber) { this->m_impl.m_seq = sequenceNumber; }

    U32 getSequenceNumber() const { return this->m_impl.m_seq; }

    bool hasSequenceNumberWrapped() const { return this->m_impl.m_seqWrapped; }

    U32 allocateSequenceNumber() { return this->m_impl.allocateSequenceNumber(); }

    bool trackSequenceNumber(U32 sequenceNumber) {
        CommandDispatcherImpl::SequenceTrackerEntry entry = {};
        return this->m_impl.m_sequenceTracker.insert(sequenceNumber, entry) == Fw::Success::SUCCESS;
    }

  private:
    Svc::CommandDispatcherImpl& m_impl;

    void from_compCmdSend_handler(FwIndexType portNum, FwOpcodeType opCode, U32 cmdSeq, Fw::CmdArgBuffer& args);

    void from_pingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
    );                                                   // store port call
    bool m_cmdSendRcvd;
    FwOpcodeType m_cmdSendOpCode;
    U32 m_cmdSendCmdSeq;
    Fw::CmdArgBuffer m_cmdSendArgs;
    FwIndexType m_cmdSendPortNum;

    void from_seqCmdStatus_handler(FwIndexType portNum,
                                   FwOpcodeType opCode,
                                   U32 cmdSeq,
                                   const Fw::CmdResponse& response);

    bool m_seqStatusRcvd;
    FwOpcodeType m_seqStatusOpCode;
    U32 m_seqStatusCmdSeq;
    Fw::CmdResponse m_seqStatusCmdResponse;
    FwIndexType m_seqStatusPortNum;
};

} /* namespace Svc */

#endif /* CMDDISP_TEST_UT_TLMCHANIMPLTESTER_HPP_ */
