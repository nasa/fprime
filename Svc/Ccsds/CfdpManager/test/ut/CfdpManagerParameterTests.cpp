// ======================================================================
// \title  CfdpManagerParameterTests.cpp
// \author Auto-generated
// \brief  cpp file for CfdpManager component parameter unit tests
//
// ======================================================================

#include "CfdpManagerTester.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Parameter Tests
// ----------------------------------------------------------------------
//
// Each parameter has two tests:
//   *Default : after loadParameters() (called in the tester constructor),
//              component.paramGet_<Name>() returns the FPP-declared default and
//              flags ParamValid::DEFAULT.
//   *SetGet  : paramSet_<Name>() stages a value, paramSend_<Name>() commits it,
//              verified via component.paramGet_<Name>() which then flags VALID.
//
// The tester is a friend of CfdpManager, so component.paramGet_<Name>() is
// accessible directly. Parameter SET opcodes are handled synchronously inside
// the generated cmdIn dispatch (unlike async user commands), so paramSend_*
// applies the value immediately and no doDispatch() is needed afterward.

// ---- LocalEid ----

void CfdpManagerTester::testParamLocalEidDefault() {
    Fw::ParamValid valid;
    const EntityId value = this->component.paramGet_LocalEid(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(42u, value);
}

void CfdpManagerTester::testParamLocalEidSetGet() {
    const EntityId newValue = 77;
    this->paramSet_LocalEid(newValue, Fw::ParamValid::VALID);
    this->paramSend_LocalEid(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_LocalEid(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- OutgoingFileChunkSize ----

void CfdpManagerTester::testParamOutgoingFileChunkSizeDefault() {
    Fw::ParamValid valid;
    const U32 value = this->component.paramGet_OutgoingFileChunkSize(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(992u, value);
}

void CfdpManagerTester::testParamOutgoingFileChunkSizeSetGet() {
    const U32 newValue = 512;
    this->paramSet_OutgoingFileChunkSize(newValue, Fw::ParamValid::VALID);
    this->paramSend_OutgoingFileChunkSize(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_OutgoingFileChunkSize(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- RxCrcCalcBytesPerCycle ----

void CfdpManagerTester::testParamRxCrcCalcBytesPerCycleDefault() {
    Fw::ParamValid valid;
    const U32 value = this->component.paramGet_RxCrcCalcBytesPerCycle(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(65536u, value);
}

void CfdpManagerTester::testParamRxCrcCalcBytesPerCycleSetGet() {
    const U32 newValue = 32768;
    this->paramSet_RxCrcCalcBytesPerCycle(newValue, Fw::ParamValid::VALID);
    this->paramSend_RxCrcCalcBytesPerCycle(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_RxCrcCalcBytesPerCycle(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- FileInDefaultChannel ----

void CfdpManagerTester::testParamFileInDefaultChannelDefault() {
    Fw::ParamValid valid;
    const U8 value = this->component.paramGet_FileInDefaultChannel(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(0u, value);
}

void CfdpManagerTester::testParamFileInDefaultChannelSetGet() {
    const U8 newValue = 1;
    this->paramSet_FileInDefaultChannel(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultChannel(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_FileInDefaultChannel(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- FileInDefaultDestEntityId ----

void CfdpManagerTester::testParamFileInDefaultDestEntityIdDefault() {
    Fw::ParamValid valid;
    const EntityId value = this->component.paramGet_FileInDefaultDestEntityId(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(100u, value);
}

void CfdpManagerTester::testParamFileInDefaultDestEntityIdSetGet() {
    const EntityId newValue = 200;
    this->paramSet_FileInDefaultDestEntityId(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultDestEntityId(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_FileInDefaultDestEntityId(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- FileInDefaultClass ----

void CfdpManagerTester::testParamFileInDefaultClassDefault() {
    Fw::ParamValid valid;
    const Cfdp::Class value = this->component.paramGet_FileInDefaultClass(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(Cfdp::Class::CLASS_2, value);
}

void CfdpManagerTester::testParamFileInDefaultClassSetGet() {
    const Cfdp::Class newValue = Cfdp::Class::CLASS_1;
    this->paramSet_FileInDefaultClass(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultClass(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(Cfdp::Class::CLASS_1, this->component.paramGet_FileInDefaultClass(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- FileInDefaultKeep ----

void CfdpManagerTester::testParamFileInDefaultKeepDefault() {
    Fw::ParamValid valid;
    const Cfdp::Keep value = this->component.paramGet_FileInDefaultKeep(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(Cfdp::Keep::DELETE, value);
}

void CfdpManagerTester::testParamFileInDefaultKeepSetGet() {
    const Cfdp::Keep newValue = Cfdp::Keep::KEEP;
    this->paramSet_FileInDefaultKeep(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultKeep(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(Cfdp::Keep::KEEP, this->component.paramGet_FileInDefaultKeep(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ---- FileInDefaultPriority ----

void CfdpManagerTester::testParamFileInDefaultPriorityDefault() {
    Fw::ParamValid valid;
    const U8 value = this->component.paramGet_FileInDefaultPriority(valid);
    ASSERT_EQ(Fw::ParamValid::DEFAULT, valid.e) << "Loaded default should flag DEFAULT";
    ASSERT_EQ(0u, value);
}

void CfdpManagerTester::testParamFileInDefaultPrioritySetGet() {
    const U8 newValue = 5;
    this->paramSet_FileInDefaultPriority(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultPriority(0, 0);

    Fw::ParamValid valid;
    ASSERT_EQ(newValue, this->component.paramGet_FileInDefaultPriority(valid));
    ASSERT_EQ(Fw::ParamValid::VALID, valid.e);
}

// ----------------------------------------------------------------------
// Parameter Telemetry Tests
// ----------------------------------------------------------------------
//
// Each parameter is mirrored to a telemetry channel from parameterUpdated().
// On load, loadParameters() calls parameterLoaded() for each parameter, which
// in turn calls parameterUpdated(), so every channel emits an initial sample;
// thereafter parameterUpdated() re-emits only the channel for the parameter
// that changed.
//
// Each test verifies both halves of that contract:
//   * On load: loadParameters() runs in the tester constructor, so the channel
//              already holds exactly one sample equal to the FPP-declared
//              default before the test body executes.
//   * On update: after clearHistory(), staging + sending a new value drives
//                parameterUpdated() for that parameter, re-emitting its channel
//                with the updated value. Since parameterUpdated() writes only
//                the changed channel, that channel holds exactly one sample.

// ---- PRM_LOCAL_EID ----

void CfdpManagerTester::testTlmLocalEid() {
    ASSERT_TLM_PRM_LOCAL_EID_SIZE(1);
    ASSERT_TLM_PRM_LOCAL_EID(0, 42u);

    this->clearHistory();
    const EntityId newValue = 77;
    this->paramSet_LocalEid(newValue, Fw::ParamValid::VALID);
    this->paramSend_LocalEid(0, 0);

    ASSERT_TLM_PRM_LOCAL_EID_SIZE(1);
    ASSERT_TLM_PRM_LOCAL_EID(0, newValue);
}

// ---- PRM_OUTGOING_FILE_CHUNK_SIZE ----

void CfdpManagerTester::testTlmOutgoingFileChunkSize() {
    ASSERT_TLM_PRM_OUTGOING_FILE_CHUNK_SIZE_SIZE(1);
    ASSERT_TLM_PRM_OUTGOING_FILE_CHUNK_SIZE(0, 992u);

    this->clearHistory();
    const U32 newValue = 512;
    this->paramSet_OutgoingFileChunkSize(newValue, Fw::ParamValid::VALID);
    this->paramSend_OutgoingFileChunkSize(0, 0);

    ASSERT_TLM_PRM_OUTGOING_FILE_CHUNK_SIZE_SIZE(1);
    ASSERT_TLM_PRM_OUTGOING_FILE_CHUNK_SIZE(0, newValue);
}

// ---- PRM_RX_CRC_CALC_BYTES_PER_CYCLE ----

void CfdpManagerTester::testTlmRxCrcCalcBytesPerCycle() {
    ASSERT_TLM_PRM_RX_CRC_CALC_BYTES_PER_CYCLE_SIZE(1);
    ASSERT_TLM_PRM_RX_CRC_CALC_BYTES_PER_CYCLE(0, 65536u);

    this->clearHistory();
    const U32 newValue = 32768;
    this->paramSet_RxCrcCalcBytesPerCycle(newValue, Fw::ParamValid::VALID);
    this->paramSend_RxCrcCalcBytesPerCycle(0, 0);

    ASSERT_TLM_PRM_RX_CRC_CALC_BYTES_PER_CYCLE_SIZE(1);
    ASSERT_TLM_PRM_RX_CRC_CALC_BYTES_PER_CYCLE(0, newValue);
}

// ---- PRM_POST_INACTIVITY_SEND_RETRIES ----

void CfdpManagerTester::testTlmPostInactivitySendRetries() {
    ASSERT_TLM_PRM_POST_INACTIVITY_SEND_RETRIES_SIZE(1);
    ASSERT_TLM_PRM_POST_INACTIVITY_SEND_RETRIES(0, 3u);

    this->clearHistory();
    const U8 newValue = 7;
    this->paramSet_PostInactivitySendRetries(newValue, Fw::ParamValid::VALID);
    this->paramSend_PostInactivitySendRetries(0, 0);

    ASSERT_TLM_PRM_POST_INACTIVITY_SEND_RETRIES_SIZE(1);
    ASSERT_TLM_PRM_POST_INACTIVITY_SEND_RETRIES(0, newValue);
}

// ---- PRM_FILE_IN_DEFAULT_CHANNEL ----

void CfdpManagerTester::testTlmFileInDefaultChannel() {
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CHANNEL_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CHANNEL(0, 0u);

    this->clearHistory();
    const U8 newValue = 1;
    this->paramSet_FileInDefaultChannel(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultChannel(0, 0);

    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CHANNEL_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CHANNEL(0, newValue);
}

// ---- PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID ----

void CfdpManagerTester::testTlmFileInDefaultDestEntityId() {
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID(0, 100u);

    this->clearHistory();
    const EntityId newValue = 200;
    this->paramSet_FileInDefaultDestEntityId(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultDestEntityId(0, 0);

    ASSERT_TLM_PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID(0, newValue);
}

// ---- PRM_FILE_IN_DEFAULT_CLASS ----

void CfdpManagerTester::testTlmFileInDefaultClass() {
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CLASS_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CLASS(0, Cfdp::Class::CLASS_2);

    this->clearHistory();
    const Cfdp::Class newValue = Cfdp::Class::CLASS_1;
    this->paramSet_FileInDefaultClass(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultClass(0, 0);

    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CLASS_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_CLASS(0, Cfdp::Class::CLASS_1);
}

// ---- PRM_FILE_IN_DEFAULT_KEEP ----

void CfdpManagerTester::testTlmFileInDefaultKeep() {
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_KEEP_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_KEEP(0, Cfdp::Keep::DELETE);

    this->clearHistory();
    const Cfdp::Keep newValue = Cfdp::Keep::KEEP;
    this->paramSet_FileInDefaultKeep(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultKeep(0, 0);

    ASSERT_TLM_PRM_FILE_IN_DEFAULT_KEEP_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_KEEP(0, Cfdp::Keep::KEEP);
}

// ---- PRM_FILE_IN_DEFAULT_PRIORITY ----

void CfdpManagerTester::testTlmFileInDefaultPriority() {
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_PRIORITY_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_PRIORITY(0, 0u);

    this->clearHistory();
    const U8 newValue = 5;
    this->paramSet_FileInDefaultPriority(newValue, Fw::ParamValid::VALID);
    this->paramSend_FileInDefaultPriority(0, 0);

    ASSERT_TLM_PRM_FILE_IN_DEFAULT_PRIORITY_SIZE(1);
    ASSERT_TLM_PRM_FILE_IN_DEFAULT_PRIORITY(0, newValue);
}

// ---- PRM_CHANNEL_CONFIG ----

void CfdpManagerTester::testTlmChannelConfig() {
    Fw::ParamValid valid;
    const ChannelArrayParams loaded = this->component.paramGet_ChannelConfig(valid);

    ASSERT_TLM_PRM_CHANNEL_CONFIG_SIZE(1);
    ASSERT_TLM_PRM_CHANNEL_CONFIG(0, loaded);

    this->clearHistory();
    ChannelArrayParams updated = loaded;
    updated[0].set_ack_limit(static_cast<U8>(loaded[0].get_ack_limit() + 1));
    this->paramSet_ChannelConfig(updated, Fw::ParamValid::VALID);
    this->paramSend_ChannelConfig(0, 0);

    ASSERT_TLM_PRM_CHANNEL_CONFIG_SIZE(1);
    ASSERT_TLM_PRM_CHANNEL_CONFIG(0, updated);
}

// ---- Unknown parameter id ----

void CfdpManagerTester::testParameterUpdatedUnknownIdAsserts() {
    // The default branch guards against a parameter being added to the model
    // without a matching case in parameterUpdated(). An id that maps to no
    // parameter must trip the coding-error assert rather than pass silently.
    ASSERT_DEATH_IF_SUPPORTED(this->component.parameterUpdated(static_cast<FwPrmIdType>(0xFFFFFFFF)),
                              "CfdpManager.cpp");
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
