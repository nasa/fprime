// ======================================================================
// \title  ActiveTest/test/ut/Tester.cpp
// \author tiffany
// \brief  cpp file for ActiveTest test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include "STest/Pick/Pick.hpp"


// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : ActiveTestGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("ActiveTest"),
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

Tester ::~Tester() {}

void Tester ::initComponents() {
    this->init();
    this->component.init(Tester::TEST_INSTANCE_QUEUE_DEPTH, Tester::TEST_INSTANCE_ID);
}

Fw::ParamValid Tester ::from_prmGetIn_handler(const NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    val.resetSer();

    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case ActiveTestComponentBase::PARAMID_PARAMBOOL:
            status = val.serialize(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMU32:
            status = val.serialize(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMSTRING:
            status = val.serialize(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMENUM:
            status = val.serialize(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMARRAY:
            status = val.serialize(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.serialize(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmGetIn(id, val);

    return prmValid;
}

void Tester ::from_prmSetIn_handler(const NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case ActiveTestComponentBase::PARAMID_PARAMBOOL:
            status = val.deserialize(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMU32:
            status = val.deserialize(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMSTRING:
            status = val.deserialize(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMENUM:
            status = val.deserialize(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMARRAY:
            status = val.deserialize(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case ActiveTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.deserialize(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmSetIn(id, val);
}
