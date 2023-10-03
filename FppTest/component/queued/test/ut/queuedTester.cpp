// ======================================================================
// \title  QueuedTest/test/ut/Tester.cpp
// \author tiffany
// \brief  cpp file for QueuedTest test harness implementation class
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "queuedTester.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

queuedTester ::queuedTester()
    : QueuedTestGTestBase("Tester", queuedTester::MAX_HISTORY_SIZE),
      component("QueuedTest"),
      primitiveBuf(primitiveData, sizeof(primitiveData)),
      stringBuf(stringData, sizeof(stringData)),
      enumBuf(enumData, sizeof(enumData)),
      arrayBuf(arrayData, sizeof(arrayData)),
      structBuf(structData, sizeof(structData)),
      serialBuf(serialData, sizeof(serialData)),
      time(STest::Pick::any(), STest::Pick::any()) {
    this->initComponents();
    this->connectPorts();
    this->connectAsyncPorts();
}

queuedTester ::~queuedTester() {}

void queuedTester ::initComponents() {
    this->init();
    this->component.init(queuedTester::TEST_INSTANCE_QUEUE_DEPTH, queuedTester::TEST_INSTANCE_ID);
}

Fw::ParamValid queuedTester ::from_prmGetIn_handler(const NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    val.resetSer();

    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case QueuedTestComponentBase::PARAMID_PARAMBOOL:
            status = val.serialize(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMU32:
            status = val.serialize(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMSTRING:
            status = val.serialize(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMENUM:
            status = val.serialize(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMARRAY:
            status = val.serialize(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.serialize(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmGetIn(id, val);

    return prmValid;
}

void queuedTester ::from_prmSetIn_handler(const NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case QueuedTestComponentBase::PARAMID_PARAMBOOL:
            status = val.deserialize(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMU32:
            status = val.deserialize(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMSTRING:
            status = val.deserialize(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMENUM:
            status = val.deserialize(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMARRAY:
            status = val.deserialize(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case QueuedTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.deserialize(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmSetIn(id, val);
}