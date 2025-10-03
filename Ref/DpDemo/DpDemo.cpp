// ======================================================================
// \title  DpDemo.cpp
// \author jawest
// \brief  cpp file for DpDemo component implementation class
// ======================================================================

#include "Ref/DpDemo/DpDemo.hpp"

namespace Ref {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DpDemo ::DpDemo(const char* const compName) : DpDemoComponentBase(compName) {
    this->selectedColor = DpDemo_ColorEnum::RED;
    this->numRecords = 0;
    this->dpPriority = 0;
}

DpDemo ::~DpDemo() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void DpDemo ::run_handler(FwIndexType portNum, U32 context) {
    // If a Data product is being generated, store records
    if (this->dpInProgress) {
        this->dpContainer.serializeRecord_StringRecord(Fw::String("Test string"));
        this->dpContainer.serializeRecord_BooleanRecord(true);
        this->dpContainer.serializeRecord_I32Record(-100);
        this->dpContainer.serializeRecord_F64Record(1.25);
        this->dpContainer.serializeRecord_U32ArrayRecord(DpDemo_U32Array({1, 2, 3, 4, 5}));
        this->dpContainer.serializeRecord_F32ArrayRecord(DpDemo_F32Array({1.1f, 2.2f, 3.3f}));
        this->dpContainer.serializeRecord_BooleanArrayRecord(DpDemo_BooleanArray({true, false}));
        // Array Records
        // Array record of strings
        Fw::String str0("String array element 0");
        Fw::String str1("String array element 1");
        Fw::String str2("String array element 2");
        const Fw::StringBase* strings[3] = {&str0, &str1, &str2};
        this->dpContainer.serializeRecord_StringArrayRecord(strings, 3);
        // Array record of arrays
        const DpDemo_StringArray arrayArray[1] = {DpDemo_StringArray(
            {Fw::String("0 - String array record element 0"), Fw::String("0 - String array record element 1")})};
        this->dpContainer.serializeRecord_ArrayArrayRecord(arrayArray, 1);
        // Array record of structs
        const DpDemo_StructWithStringMembers structArray[2] = {
            DpDemo_StructWithStringMembers(Fw::String("0 - String member"),
                                           DpDemo_StringArray({Fw::String("0 - String array element 0"),
                                                               Fw::String("0 - String array element 1")})),
            DpDemo_StructWithStringMembers(Fw::String("1 - String member"),
                                           DpDemo_StringArray({Fw::String("1 - String array element 0"),
                                                               Fw::String("1 - String array element 1")}))};
        this->dpContainer.serializeRecord_StructArrayRecord(structArray, 2);
        this->dpContainer.serializeRecord_ArrayOfStringArrayRecord(DpDemo_ArrayOfStringArray(
            {DpDemo_StringArray({Fw::String("0 - String array element 0"), Fw::String("0 - String array element 1")}),
             DpDemo_StringArray({Fw::String("1 - String array element 0"), Fw::String("1 - String array element 1")}),
             DpDemo_StringArray(
                 {Fw::String("2 - String array element 0"), Fw::String("2 - String array element 1")})}));
        this->dpContainer.serializeRecord_ArrayOfStructsRecord(DpDemo_ArrayOfStructs(
            {DpDemo_StructWithStringMembers(Fw::String("0 - String member"),
                                            DpDemo_StringArray({Fw::String("0 - String array element 0"),
                                                                Fw::String("0 - String array element 1")})),
             DpDemo_StructWithStringMembers(Fw::String("1 - String member"),
                                            DpDemo_StringArray({Fw::String("1 - String array element 0"),
                                                                Fw::String("1 - String array element 1")})),
             DpDemo_StructWithStringMembers(Fw::String("2 - String member"),
                                            DpDemo_StringArray({Fw::String("2 - String array element 0"),
                                                                Fw::String("2 - String array element 1")}))}));
        this->dpContainer.serializeRecord_EnumArrayRecord(
            DpDemo_EnumArray({DpDemo_ColorEnum::RED, DpDemo_ColorEnum::GREEN, DpDemo_ColorEnum::BLUE}));
        this->dpContainer.serializeRecord_StructWithEverythingRecord(DpDemo_StructWithEverything(
            -1, 2.5, Fw::String("String Member"), false, this->selectedColor,
            {DpDemo_U32Array({1, 2, 3, 4, 5}), DpDemo_U32Array({6, 7, 8, 9, 10})}, DpDemo_F32Array({4.4f, 5.5f, 6.6f}),
            DpDemo_U32Array({6, 7, 8, 9, 10}),
            DpDemo_EnumArray({DpDemo_ColorEnum::RED, DpDemo_ColorEnum::GREEN, DpDemo_ColorEnum::BLUE}),
            DpDemo_StringArray({Fw::String("String array element 0"), Fw::String("String array element 1")}),
            DpDemo_BooleanArray({true, false}),
            DpDemo_StructWithStringMembers(
                Fw::String("String member"),
                DpDemo_StringArray({Fw::String("String array element 0"), Fw::String("String array element 1")})),
            DpDemo_ArrayOfStringArray({DpDemo_StringArray({Fw::String("0 - String array element 0"),
                                                           Fw::String("0 - String array element 1")}),
                                       DpDemo_StringArray({Fw::String("1 - String array element 0"),
                                                           Fw::String("1 - String array element 1")}),
                                       DpDemo_StringArray({Fw::String("2 - String array element 0"),
                                                           Fw::String("2 - String array element 1")})})));
        this->log_ACTIVITY_LO_DpComplete(this->numRecords);
        this->cleanupAndSendDp();
    }
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void DpDemo ::SelectColor_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, Ref::DpDemo_ColorEnum color) {
    this->selectedColor = color;
    log_ACTIVITY_HI_ColorSelected(color);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void DpDemo ::Dp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, DpDemo_DpReqType reqType, U32 priority) {
    // make sure DPs are available
    if (!this->isConnected_productGetOut_OutputPort(0) || !this->isConnected_productRequestOut_OutputPort(0)) {
        this->log_WARNING_HI_DpsNotConnected();
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->numRecords = 15;  // 15 records in current demo
    FwSizeType dpSize = DpDemo_StringAlias::SERIALIZED_SIZE + sizeof(DpDemo_BoolAlias) + sizeof(DpDemo_I32Alias) +
                        sizeof(DpDemo_F64Alias) + DpDemo_U32Array::SERIALIZED_SIZE + DpDemo_F32Array::SERIALIZED_SIZE +
                        DpDemo_BooleanArray::SERIALIZED_SIZE + DpDemo_EnumArray::SERIALIZED_SIZE +
                        DpDemo_StringArray::SERIALIZED_SIZE + DpDemo_StructWithEverything::SERIALIZED_SIZE +
                        DpDemo_StructWithStringMembers::SERIALIZED_SIZE + (DpDemo_StringArray::SERIALIZED_SIZE * 3) +
                        (DpDemo_StringArray::SERIALIZED_SIZE * 1) +
                        (DpDemo_StructWithStringMembers::SERIALIZED_SIZE * 2) +
                        DpDemo_ArrayOfStringArray::SERIALIZED_SIZE + (numRecords * sizeof(FwDpIdType));

    this->dpPriority = static_cast<FwDpPriorityType>(priority);
    this->log_ACTIVITY_LO_DpMemRequested(dpSize);
    if (reqType == DpDemo_DpReqType::IMMEDIATE) {
        Fw::Success stat = this->dpGet_DpDemoContainer(dpSize, this->dpContainer);
        // make sure we got the memory we wanted
        if (Fw::Success::FAILURE == stat) {
            this->log_WARNING_HI_DpMemoryFail();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        } else {
            this->dpInProgress = true;
            this->log_ACTIVITY_LO_DpStarted(numRecords);
            this->log_ACTIVITY_LO_DpMemReceived(this->dpContainer.getBuffer().getSize());
            // override priority with requested priority
            this->dpContainer.setPriority(priority);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        }
    } else if (reqType == DpDemo_DpReqType::ASYNC) {
        this->dpRequest_DpDemoContainer(dpSize);
    } else {
        // should never get here
        FW_ASSERT(0, reqType.e);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for data products
// ----------------------------------------------------------------------

void DpDemo ::dpRecv_DpDemoContainer_handler(DpContainer& container, Fw::Success::T status) {
    // Make sure we got the buffer we wanted or quit
    if (Fw::Success::SUCCESS == status) {
        this->dpContainer = container;
        this->dpInProgress = true;
        // set previously requested priority
        this->dpContainer.setPriority(this->dpPriority);
        this->log_ACTIVITY_LO_DpStarted(this->numRecords);
    } else {
        this->log_WARNING_HI_DpMemoryFail();
        // cleanup
        this->dpInProgress = false;
        this->numRecords = 0;
    }
}

void DpDemo ::cleanupAndSendDp() {
    this->dpSend(this->dpContainer);
    this->dpInProgress = false;
    this->numRecords = 0;
}

}  // namespace Ref
