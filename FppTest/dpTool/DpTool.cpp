// ======================================================================
// \title  DpTool.cpp
// \brief  cpp file for DpTool component implementation class
// ======================================================================

#include <cstdio>

#include "FppTest/dpTool/DpTool.hpp"
#include "Fw/Types/Assert.hpp"
#include "stdio.h"
#include "math.h"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

DpTool ::DpTool(const char* const compName)
    : DpToolComponentBase(compName),
      sendTime(Fw::ZERO_TIME),
      recordTest(U32Record) {}

void DpTool ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    DpToolComponentBase::init(queueDepth, instance);
}

DpTool ::~DpTool() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DpTool::schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
}

// ----------------------------------------------------------------------
// Data product handler implementations
// ----------------------------------------------------------------------


void DpTool ::dpRecv_Container1_handler(DpContainer& container, Fw::Success::T status) {

    if (status == Fw::Success::SUCCESS) {

        auto serializeStatus = Fw::FW_SERIALIZE_OK;
        for (FwSizeType i = 0; i < CONTAINER_1_DATA_SIZE; ++i) {
            
            switch (this->recordTest) {

                case U32Record:
                    serializeStatus = container.serializeRecord_U32Record(rand());
                    break;

                case ComplexRecord: {
                    DpTool_Complex dpTool_Complex;
                    dpTool_Complex.set(FppTest::DpTool_Data(rand() & 0xFFFF), rand()); 
                    serializeStatus = container.serializeRecord_ComplexRecord(dpTool_Complex);
                    break;
                }

                case U8ArrayRecord: {
                    U8 u8Array[RECORD_ARRAY_SIZE];
                    for (U32 i=0; i<RECORD_ARRAY_SIZE; i++) {
                        u8Array[i] = rand() & 0xFF;
                    }
                    serializeStatus = container.serializeRecord_U8ArrayRecord(u8Array, RECORD_ARRAY_SIZE);
                    break;
                }

                case U32ArrayRecord: {
                    U32 u32Array[RECORD_ARRAY_SIZE];
                    for (U32 i=0; i<RECORD_ARRAY_SIZE; i++) {
                        u32Array[i] = rand();
                    }
                    serializeStatus = container.serializeRecord_U32ArrayRecord(u32Array, RECORD_ARRAY_SIZE);
                    break;
                }

                case DataArrayRecord: {
                    DpTool_Data dpTool_Data[RECORD_ARRAY_SIZE];
                    for (U32 i=0; i<RECORD_ARRAY_SIZE; i++) {
                        dpTool_Data[i].setu16Field(rand() & 0xFFFF);
                    }
                    serializeStatus = container.serializeRecord_DataArrayRecord(dpTool_Data, RECORD_ARRAY_SIZE);
                    break;
                }

                case EnumRecord: {
                    DpTool_EnumStruct dpTool_enumStruct;
                    dpTool_enumStruct.setstatus(FppTest::DpTool_Status::T(rand() % 3));
                    serializeStatus = container.serializeRecord_EnumRecord(dpTool_enumStruct);
                    break;
                }

                case F32Record:
                    serializeStatus = container.serializeRecord_F32Record(M_PI);
                    break;

                case F64Record:
                    serializeStatus = container.serializeRecord_F64Record(M_PI);
                    break;

                case BoolRecord:
                    serializeStatus = container.serializeRecord_BoolRecord(rand() % 2);
                    break;

                case I8Record: {
                    I8 value = (rand() % 256) - 128;
                    serializeStatus = container.serializeRecord_I8Record(value);
                    break;
                }

                case I16Record: {
                    I16 value = (rand() % 65536) - 32768;
                    serializeStatus = container.serializeRecord_I16Record(value);
                    break;
                }

                case I32Record: {
                    I32 value = rand() - RAND_MAX/2;
                    serializeStatus = container.serializeRecord_I32Record(value);
                    break;
                }

                case I64Record: {
                    // Generate a random 64-bit integer
                    I32 high_part = (I32)((I32)rand() - RAND_MAX/2);
                    I32 low_part = rand();

                    I64 value = (I64)((U64)high_part << 32) | low_part;

                    serializeStatus = container.serializeRecord_I64Record(value);
                    break;
                }

                case FppArrayRecord: {
                    
                    DpTool_ArrayData::Type_of_arrField fieldValues;
                    for (U32 i=0; i<10; i++) {
                        fieldValues[i] = i;
                    }

                    DpTool_ArrayData value;
                    value.setarrField(fieldValues);

                    serializeStatus = container.serializeRecord_FppArrayRecord(value);
                    break;
                }

            }

            if (serializeStatus == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
                break;
            }
            FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, status);
        }
        // Use the time stamp from the time get port
        this->dpSend(container);
    }
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

void DpTool::checkContainer(const DpContainer& container, FwDpIdType localId, FwSizeType size) const {
    FW_ASSERT(container.getBaseId() == this->getIdBase(), container.getBaseId(), this->getIdBase());
    FW_ASSERT(container.getBuffer().getSize() == size, container.getBuffer().getSize(), size);
}

}  // end namespace FppTest
