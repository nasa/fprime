// ======================================================================
// \title  CmdSplitter/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for CmdSplitter test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "CmdSplitterGTestBase.hpp"
#include "Svc/CmdSplitter/CmdSplitter.hpp"

namespace Svc {

class CmdSplitterTester : public CmdSplitterGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

    //! Construct object CmdSplitterTester
    //!
    CmdSplitterTester();

    //! Destroy object CmdSplitterTester
    //!
    ~CmdSplitterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test that commands under a limit route locally
    //!
    void test_local_routing();

    //! Test the commands above the limit route remotely
    //!
    void test_remote_routing();

    //! Test that errored command route locally
    //!
    void test_error_routing();

    //! Test that command response forwarding works
    //!
    void test_response_forwarding();

  private:
    //! Helper to build a com buffer given an opcode
    //!
    Fw::ComBuffer build_command_around_opcode(FwOpcodeType opcode);

    //! Helper to set opcode base and select a valid opcode
    //!
    FwOpcodeType setup_and_pick_valid_opcode(bool for_local /*!< Local command testing*/);

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_LocalCmd
    //!
    void from_LocalCmd_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                               Fw::ComBuffer& data,           /*!< Buffer containing packet data */
                               U32 context                    /*!< Call context value; meaning chosen by user */
    );

    //! Handler for from_RemoteCmd
    //!
    void from_RemoteCmd_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                Fw::ComBuffer& data,           /*!< Buffer containing packet data */
                                U32 context                    /*!< Call context value; meaning chosen by user */
    );

    //! Handler for from_forwardSeqCmdStatus
    //!
    void from_forwardSeqCmdStatus_handler(const NATIVE_INT_TYPE portNum,  /*!< The port number*/
                                          FwOpcodeType opCode,            /*!< Command Op Code */
                                          U32 cmdSeq,                     /*!< Command Sequence */
                                          const Fw::CmdResponse& response /*!< The command response argument */
    );

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    CmdSplitter component;
    NATIVE_INT_TYPE active_command_source;
};

}  // end namespace Svc

#endif
