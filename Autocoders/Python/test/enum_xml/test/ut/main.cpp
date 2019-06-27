#include <Autocoders/Python/test/enum_xml/Component1ComponentAc.hpp>
#include <Autocoders/Python/test/enum_xml/Enum1EnumAc.hpp>
#include <Autocoders/Python/test/enum_xml/Port1PortAc.hpp>
#include <Autocoders/Python/test/enum_xml/Serial1SerializableAc.hpp>

#include <Autocoders/Python/test/enum_xml/ExampleEnumImpl.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Assert.hpp>

#include <bitset>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleReg_ptr = 0;

// Component instance pointers
Example::ExampleEnumImpl* inst1 = 0;
Example::ExampleEnumImpl* inst2 = 0;

extern "C" {
    void dumparch(void);
    void dumpobj(const char* objName);
}

#ifdef TGT_OS_TYPE_LINUX
extern "C" {
    int main(int argc, char* argv[]);
};
#endif

void dumparch(void) {
    simpleReg_ptr->dump();
}

void dumpobj(const char* objName) {
    simpleReg_ptr->dump(objName);
}

void constructArchitecture(void) {
    Fw::PortBase::setTrace(true);
    
    simpleReg_ptr = new Fw::SimpleObjRegistry();
    
    // Instantiate the inst1 and inst2
    inst1   = new Example::ExampleEnumImpl("inst1");
    inst2   = new Example::ExampleEnumImpl("inst2");
    
    // Connect inst1 to inst2
    inst1->set_ExEnumOut_OutputPort(0, inst2->get_ExEnumIn_InputPort(0));
    
    // Connect inst2 to inst1
    inst2->set_ExEnumOut_OutputPort(0, inst1->get_ExEnumIn_InputPort(0));
    
    // Instantiate components
    inst1->init(100);
    inst2->init(100);
    
    dumparch();
}

int main(int argc, char* argv[]) {
    // Construct the topology here.
    constructArchitecture();

    int serial_arg1 = 1;
    int serial_arg2 = 1;

    Example::Enum1 enum1;
    Example::Enum1 enum2;
    Example::Serial1 serial1;
    enum1 = 0;
    enum2 = 2;
    
    FW_ASSERT(false);
    FW_ASSERT(enum1 == Example::Enum1::Item1);
    FW_ASSERT(enum2 == Example::Enum1::Item3);
    
    // Save copy of enums to test against post-serialization
    Example::Enum1 enum1Save = enum1;
    Example::Enum1 enum2Save = enum2;
    cout << "Created first enum: " << enum1 << endl;
    cout << "Created second enum: " << enum2 << endl;
    
    // Serialize enums
    U8 buffer1[1024];
    Fw::SerialBuffer enumSerial1 = Fw::SerialBuffer(buffer1, sizeof(buffer1));
    FW_ASSERT(enumSerial1.serialize(enum1) == Fw::FW_SERIALIZE_OK);
    
    U8 buffer2[1024];
    Fw::SerialBuffer enumSerial2 = Fw::SerialBuffer(buffer2, sizeof(buffer2));
    FW_ASSERT(enumSerial2.serialize(enum2) == Fw::FW_SERIALIZE_OK);
    
    // Deserialize enums
    FW_ASSERT(enumSerial1.deserialize(enum1) == Fw::FW_SERIALIZE_OK);
    FW_ASSERT(enumSerial2.deserialize(enum2) == Fw::FW_SERIALIZE_OK);
    
    // Extract enum object from buffer starting with MSB then test against original
    FwEnumStoreType deserializedVal1 = ((FwEnumStoreType) enumSerial1.getBuffAddr()[3] << 0)
    | ((FwEnumStoreType) enumSerial1.getBuffAddr()[2] << 8)
    | ((FwEnumStoreType) enumSerial1.getBuffAddr()[1] << 16)
    | ((FwEnumStoreType) enumSerial1.getBuffAddr()[0] << 24);
    FW_ASSERT(deserializedVal1 == static_cast<FwEnumStoreType>(enum1Save.e));
    
    FwEnumStoreType deserializedVal2 = ((FwEnumStoreType) enumSerial2.getBuffAddr()[3] << 0)
    | ((FwEnumStoreType) enumSerial2.getBuffAddr()[2] << 8)
    | ((FwEnumStoreType) enumSerial2.getBuffAddr()[1] << 16)
    | ((FwEnumStoreType) enumSerial2.getBuffAddr()[0] << 24);
    FW_ASSERT(deserializedVal2 == static_cast<FwEnumStoreType>(enum2Save.e));

    FW_ASSERT(enum1Save == enum1);
    FW_ASSERT(enum2Save == enum2);

    // Create serializable and test that enum is saved
    serial1 = Example::Serial1(serial_arg1, serial_arg2, enum2);
    FW_ASSERT(serial1.getMember3() == enum2);
    
    Fw::EightyCharString str1;
    serial1.toString(str1);

    // Invoke ports to test enum usage
    inst1->get_ExEnumIn_InputPort(0)->invoke(enum2, serial1);
    inst2->get_ExEnumIn_InputPort(0)->invoke(enum1, serial1);

    cout << "quit demo!" << endl;
    
    cout << "Deleting components..." << endl;
    delete inst1;
    delete inst2;
    cout << "Delete registration objects..." << endl;
    delete simpleReg_ptr;
    cout << "Completed..." << endl;
}
