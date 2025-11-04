// ======================================================================
// \title  PassiveTest/test/ut/PassiveTestTester.cpp
// \author tiffany
// \brief  cpp file for PassiveTest test harness implementation class
// ======================================================================

#include "PassiveTestTester.hpp"
#include "STest/Pick/Pick.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

PassiveTestTester ::PassiveTestTester()
    : PassiveTestGTestBase("PassiveTestTester", PassiveTestTester::MAX_HISTORY_SIZE),
      component("PassiveTest"),
      primitiveBuf(primitiveData, sizeof(primitiveData)),
      stringBuf(stringData, sizeof(stringData)),
      enumBuf(enumData, sizeof(enumData)),
      arrayBuf(arrayData, sizeof(arrayData)),
      structBuf(structData, sizeof(structData)),
      serialBuf(serialData, sizeof(serialData)),
      time(STest::Pick::any(), STest::Pick::lowerUpper(0, 999999)) {
    this->initComponents();
    this->connectPorts();
    this->component.registerExternalParameters(&this->paramTesterDelegate);
}

PassiveTestTester ::~PassiveTestTester() {}

void PassiveTestTester ::initComponents() {
    this->init();
    this->component.init(PassiveTestTester::TEST_INSTANCE_ID);
}

Fw::ParamValid PassiveTestTester ::from_prmGetIn_handler(const FwIndexType portNum,
                                                         FwPrmIdType id,
                                                         Fw::ParamBuffer& val) {
    val.resetSer();

    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case PassiveTestComponentBase::PARAMID_PARAMBOOL:
            status = val.serializeFrom(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMU32:
            status = val.serializeFrom(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMSTRING:
            status = val.serializeFrom(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMENUM:
            status = val.serializeFrom(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMARRAY:
            status = val.serializeFrom(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.serializeFrom(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmGetIn(id, val);

    return prmValid;
}

void PassiveTestTester ::from_prmSetIn_handler(const FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    Fw::SerializeStatus status;
    U32 id_base = component.getIdBase();

    FW_ASSERT(id >= id_base);

    switch (id - id_base) {
        case PassiveTestComponentBase::PARAMID_PARAMBOOL:
            status = val.deserializeTo(boolPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMU32:
            status = val.deserializeTo(u32Prm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMSTRING:
            status = val.deserializeTo(stringPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMENUM:
            status = val.deserializeTo(enumPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMARRAY:
            status = val.deserializeTo(arrayPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;

        case PassiveTestComponentBase::PARAMID_PARAMSTRUCT:
            status = val.deserializeTo(structPrm.args.val);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
            break;
    }

    this->pushFromPortEntry_prmSetIn(id, val);
}

// ----------------------------------------------------------------------
// Unit test implementation of external parameter delegate serialization/deserialization
// ----------------------------------------------------------------------

Fw::SerializeStatus PassiveTestTester::PassiveTestComponentBaseParamExternalDelegate ::deserializeParam(
    const FwPrmIdType base_id,
    const FwPrmIdType local_id,
    const Fw::ParamValid prmStat,
    Fw::SerialBufferBase& buff) {
    Fw::SerializeStatus stat;
    (void)base_id;

    // Serialize the parameter based on ID
    switch (local_id) {
        // ParamBoolExternal
        case PassiveTestComponentBase::PARAMID_PARAMBOOLEXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamBoolExternal);
            break;
        // ParamI32External
        case PassiveTestComponentBase::PARAMID_PARAMI32EXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamI32External);
            break;
        // ParamStringExternal
        case PassiveTestComponentBase::PARAMID_PARAMSTRINGEXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamStringExternal);
            break;
        // ParamEnumExternal
        case PassiveTestComponentBase::PARAMID_PARAMENUMEXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamEnumExternal);
            break;
        // ParamArrayExternal
        case PassiveTestComponentBase::PARAMID_PARAMARRAYEXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamArrayExternal);
            break;
        // ParamStructExternal
        case PassiveTestComponentBase::PARAMID_PARAMSTRUCTEXTERNAL:
            stat = buff.deserializeTo(this->m_param_ParamStructExternal);
            break;
        default:
            // Unknown ID should not have gotten here
            FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
    }

    return stat;
}

Fw::SerializeStatus PassiveTestTester::PassiveTestComponentBaseParamExternalDelegate ::serializeParam(
    const FwPrmIdType base_id,
    const FwPrmIdType local_id,
    Fw::SerialBufferBase& buff) const {
    Fw::SerializeStatus stat;
    (void)base_id;

    // Serialize the parameter based on ID
    switch (local_id) {
        // ParamBoolExternal
        case PassiveTestComponentBase::PARAMID_PARAMBOOLEXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamBoolExternal);
            break;
        // ParamI32External
        case PassiveTestComponentBase::PARAMID_PARAMI32EXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamI32External);
            break;
        // ParamStringExternal
        case PassiveTestComponentBase::PARAMID_PARAMSTRINGEXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamStringExternal);
            break;
        // ParamEnumExternal
        case PassiveTestComponentBase::PARAMID_PARAMENUMEXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamEnumExternal);
            break;
        // ParamArrayExternal
        case PassiveTestComponentBase::PARAMID_PARAMARRAYEXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamArrayExternal);
            break;
        // ParamStructExternal
        case PassiveTestComponentBase::PARAMID_PARAMSTRUCTEXTERNAL:
            stat = buff.serializeFrom(this->m_param_ParamStructExternal);
            break;
        default:
            // Unknown ID should not have gotten here
            FW_ASSERT(false, static_cast<FwAssertArgType>(local_id));
    }

    return stat;
}
