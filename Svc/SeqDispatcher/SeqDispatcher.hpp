// ======================================================================
// \title  SeqDispatcher.hpp
// \author zimri.leisher
// \brief  hpp file for SeqDispatcher component implementation class
// ======================================================================

#ifndef SeqDispatcher_HPP
#define SeqDispatcher_HPP

#include "Fw/Types/StringBase.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/SeqDispatcher/SeqDispatcherComponentAc.hpp"
#include "Svc/SeqDispatcher/SeqDispatcher_CmdSequencerStateEnumAc.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

class SeqDispatcher final : public SeqDispatcherComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object SeqDispatcher
    //!
    SeqDispatcher(const char* const compName /*!< The component name*/
    );

    //! Destroy object SeqDispatcher
    //!
    ~SeqDispatcher();

  protected:
    //! Handler for input port seqDoneIn
    void seqDoneIn_handler(FwIndexType portNum,             //!< The port number
                           FwOpcodeType opCode,             //!< Command Op Code
                           U32 cmdSeq,                      //!< Command Sequence
                           const Fw::CmdResponse& response  //!< The command response argument
    );

    //! Handler for input port seqStartIn
    void seqStartIn_handler(FwIndexType portNum,             //!< The port number
                            const Fw::StringBase& fileName,  //!< The sequence file
                            const Svc::SeqArgs& args         //!< Optional sequence arguments
    );

    //! Handler for input port seqRunIn
    void seqRunIn_handler(FwIndexType portNum,             //!< The port number
                          const Fw::StringBase& fileName,  //!< The sequence file
                          const Svc::SeqArgs& args         //!< Optional sequence arguments
    );

  private:
    // number of sequences dispatched (successful or otherwise)
    U32 m_dispatchedCount = 0;
    // number of errors from dispatched sequences (CmdResponse::EXECUTION_ERROR)
    U32 m_errorCount = 0;
    // number of sequencers in state AVAILABLE
    U32 m_sequencersAvailable = SeqDispatcherSequencerPorts;

    struct DispatchEntry {
        FwOpcodeType opCode;  //!< opcode of entry
        U32 cmdSeq;
        // store the state of each sequencer
        SeqDispatcher_CmdSequencerState state;
        // store the sequence currently running for each sequencer
        Fw::String sequenceRunning = "<no seq>";
    } m_entryTable[SeqDispatcherSequencerPorts];  //!< table of dispatch
                                                  //!< entries

    FwIndexType getNextAvailableSequencerIdx();

    void runSequence(FwIndexType sequencerIdx,
                     const Fw::ConstStringBase& fileName,
                     BlockState block,
                     const Svc::SeqArgs& args);

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for RUN command handler
    //!
    void RUN_cmdHandler(const FwOpcodeType opCode,        /*!< The opcode*/
                        const U32 cmdSeq,                 /*!< The command sequence number*/
                        const Fw::CmdStringArg& fileName, /*!< The name of the sequence file*/
                        BlockState block);

    //! Implementation for RUN_ARGS command handler
    //!
    void RUN_ARGS_cmdHandler(const FwOpcodeType opCode,        /*!< The opcode*/
                             const U32 cmdSeq,                 /*!< The command sequence number*/
                             const Fw::CmdStringArg& fileName, /*!< The name of the sequence file*/
                             BlockState block,                 /*!< Return command status when complete or not*/
                             Svc::SeqArgs buffer);             /*!< Arguments to pass to a sequencer*/

    void LOG_STATUS_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                               const U32 cmdSeq);         /*!< The command sequence number*/
};

}  // namespace Svc

#endif
