// ======================================================================
// \title  ActiveTest/test/ut/ActiveTestActiveTestTester.cpp
// \author tiffany
// \brief  cpp file for ActiveTest test harness implementation class
// ======================================================================

#include "ActiveTestTester.hpp"
#include "STest/Pick/Pick.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ActiveTestTester ::ActiveTestTester()
    : ActiveTestGTestBase("ActiveTestTester", ActiveTestTester::MAX_HISTORY_SIZE),
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
    this->component.registerExternalParameters(&this->paramTesterDelegate);
}

ActiveTestTester ::~ActiveTestTester() {}

void ActiveTestTester ::initComponents() {
    this->init();
    this->component.init(ActiveTestTester::TEST_INSTANCE_QUEUE_DEPTH, ActiveTestTester::TEST_INSTANCE_ID);
}

Fw::ParamValid ActiveTestTester ::from_prmGetIn_handler(const FwIndexType portNum,
                                                        FwPrmIdType id,
                                                        Fw::ParamBuffer& val) {
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

void ActiveTestTester ::from_prmSetIn_handler(const FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
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

// ----------------------------------------------------------------------
// Unit test implementation of external parameter delegate serialization/deserialization
// ----------------------------------------------------------------------

Fw::SerializeStatus ActiveTestTester::ActiveTestComponentBaseParamExternalDelegate ::deserializeParam(
    const FwPrmIdType base_id,
    const FwPrmIdType local_id,
    const Fw::ParamValid prmStat,
    Fw::SerializeBufferBase& buff) {
    Fw::SerializeStatus stat;
    (void)base_id;

    // Serialize the parameter based on ID
    switch (local_id) {
        // ParamBoolExternal
        case ActiveTestComponentBase::PARAMID_PARAMBOOLEXTERNAL:
            stat = buff.deserialize(this->m_param_ParamBoolExternal);
            break;
        // ParamI32External
        case ActiveTestComponentBase::PARAMID_PARAMI32EXTERNAL:
            stat = buff.deserialize(this->m_param_ParamI32External);
            break;
        // ParamStringExternal
        case ActiveTestComponentBase::PARAMID_PARAMSTRINGEXTERNAL:
            stat = buff.deserialize(this->m_param_ParamStringExternal);
            break;
        // ParamEnumExternal
        case ActiveTestComponentBase::PARAMID_PARAMENUMEXTERNAL:
            stat = buff.deserialize(this->m_param_ParamEnumExternal);
            break;
        // ParamArrayExternal
        case ActiveTestComponentBase::PARAMID_PARAMARRAYEXTERNAL:
            stat = buff.deserialize(this->m_param_ParamArrayExternal);
            break;
        // ParamStructExternal
        case ActiveTestComponentBase::PARAMID_PARAMSTRUCTEXTERNAL:
            stat = buff.deserialize(this->m_param_ParamStructExternal);
            break;
        default:
            // Unknown ID should not have gotten here
            FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
    }

    return stat;
}

Fw::SerializeStatus ActiveTestTester::ActiveTestComponentBaseParamExternalDelegate ::serializeParam(
    const FwPrmIdType base_id,
    const FwPrmIdType local_id,
    Fw::SerializeBufferBase& buff) const {
    Fw::SerializeStatus stat;
    (void)base_id;

    // Serialize the parameter based on ID
    switch (local_id) {
        // ParamBoolExternal
        case ActiveTestComponentBase::PARAMID_PARAMBOOLEXTERNAL:
            stat = buff.serialize(this->m_param_ParamBoolExternal);
            break;
        // ParamI32External
        case ActiveTestComponentBase::PARAMID_PARAMI32EXTERNAL:
            stat = buff.serialize(this->m_param_ParamI32External);
            break;
        // ParamStringExternal
        case ActiveTestComponentBase::PARAMID_PARAMSTRINGEXTERNAL:
            stat = buff.serialize(this->m_param_ParamStringExternal);
            break;
        // ParamEnumExternal
        case ActiveTestComponentBase::PARAMID_PARAMENUMEXTERNAL:
            stat = buff.serialize(this->m_param_ParamEnumExternal);
            break;
        // ParamArrayExternal
        case ActiveTestComponentBase::PARAMID_PARAMARRAYEXTERNAL:
            stat = buff.serialize(this->m_param_ParamArrayExternal);
            break;
        // ParamStructExternal
        case ActiveTestComponentBase::PARAMID_PARAMSTRUCTEXTERNAL:
            stat = buff.serialize(this->m_param_ParamStructExternal);
            break;
        default:
            // Unknown ID should not have gotten here
            FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
    }

    return stat;
}
