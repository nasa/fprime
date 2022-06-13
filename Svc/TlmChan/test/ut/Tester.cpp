// ======================================================================
// \title  TlmChan.hpp
// \author tcanham
// \brief  cpp file for TlmChan test harness implementation class
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc
{

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    Tester ::
        Tester() : TlmChanGTestBase("Tester", MAX_HISTORY_SIZE),
                   component("TlmChan")
    {
        this->initComponents();
        this->connectPorts();
    }

    Tester ::
        ~Tester()
    {
    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void TlmChanImplTester::runNominalChannel()
    {

        this->clearBuffs();
        // send first buffer
        this->sendBuff(27, 10, 0);
        this->doRun(true);
        this->checkBuff(27, 10, 0);

        this->clearBuffs();
        // send again to other buffer
        this->sendBuff(27, 10, 0);

        static bool tlc003 = false;

        if (not tlc003)
        {
            REQUIREMENT("TLC-003");
            tlc003 = true;
        }

        this->doRun(true);
        this->checkBuff(27, 10, 0);

        // do an update to make sure it gets updated and returned correctly
        this->clearBuffs();
        this->sendBuff(27, 20, 0);
    }

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    void Tester ::
        from_PktSend_handler(
            const NATIVE_INT_TYPE portNum,
            Fw::ComBuffer &data,
            U32 context)
    {
        this->pushFromPortEntry_PktSend(data, context);
    }

    void Tester ::
        from_pingOut_handler(
            const NATIVE_INT_TYPE portNum,
            U32 key)
    {
        this->pushFromPortEntry_pingOut(key);
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void Tester ::
        connectPorts()
    {

        // Run
        this->connect_to_Run(
            0,
            this->component.get_Run_InputPort(0));

        // TlmGet
        this->connect_to_TlmGet(
            0,
            this->component.get_TlmGet_InputPort(0));

        // TlmRecv
        this->connect_to_TlmRecv(
            0,
            this->component.get_TlmRecv_InputPort(0));

        // pingIn
        this->connect_to_pingIn(
            0,
            this->component.get_pingIn_InputPort(0));

        // PktSend
        this->component.set_PktSend_OutputPort(
            0,
            this->get_from_PktSend(0));

        // pingOut
        this->component.set_pingOut_OutputPort(
            0,
            this->get_from_pingOut(0));
    }

    void Tester ::
        initComponents()
    {
        this->init();
        this->component.init(
            QUEUE_DEPTH, INSTANCE);
    }

} // end namespace Svc
