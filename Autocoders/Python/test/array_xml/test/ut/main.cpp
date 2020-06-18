#include <Autocoders/Python/test/array_xml/Component1ComponentAc.hpp>
#include <Autocoders/Python/test/array_xml/InternalTypeArrayAc.hpp>
#include <Autocoders/Python/test/array_xml/ArrayTypeArrayAc.hpp>
#include <Autocoders/Python/test/array_xml/Port1PortAc.hpp>
#include <Autocoders/Python/test/array_xml/ArrSerialSerializableAc.hpp>

#include <Autocoders/Python/test/array_xml/ExampleArrayImpl.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>

#include <bitset>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleReg_ptr = 0;

// Component instance pointers
Example::ExampleArrayImpl* inst1 = 0;
Example::ExampleArrayImpl* inst2 = 0;

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
    
    // // Instantiate the inst1 and inst2
    // inst1   = new Example::ExampleArrayImpl("inst1");
    // inst2   = new Example::ExampleArrayImpl("inst2");
    
    // // Connect inst1 to inst2
    // inst1->set_EnumOut_OutputPort(0, inst2->get_EnumIn_InputPort(0));
    
    // // Connect inst2 to inst1
    // inst2->set_EnumOut_OutputPort(0, inst1->get_EnumIn_InputPort(0));
    
    // // Instantiate components
    // inst1->init(100);
    // inst2->init(100);
    // inst1->start(0, 100, 10 * 1024);
    // inst2->start(0, 100, 10 * 1024);
    
    dumparch();
}

int main(int argc, char* argv[]) {
    // Construct the topology here.
    constructArchitecture();
    
    setbuf(stdout, NULL);

    cout << "Initialize Arrays";

    InternalType array1 = InternalType(6,7,120,444);
    Example::ArrayType array2 = Example::ArrayType(array1);
    Example::ArrSerial serial1;
            
    // Serialize arrays
    cout << "Serialize arrays";
    U8 buffer1[1024];
    U8 buffer2[1024];
    
    Fw::SerialBuffer arraySerial1 = Fw::SerialBuffer(buffer1, sizeof(buffer1));
    Fw::SerialBuffer arraySerial2 = Fw::SerialBuffer(buffer2, sizeof(buffer2));
    
    if (arraySerial1.serialize(array1) != Fw::FW_SERIALIZE_OK) {
        cout << "ERROR: bad serialization array1." << endl;
    } else {
        cout << "Serialized array1" << endl;
    }
                
    if (arraySerial2.serialize(array2) != Fw::FW_SERIALIZE_OK) {
        cout << "ERROR: bad serialization array2." << endl;
    } else {
        cout << "Serialized array2" << endl;
    }
                
    cout << "Serialized arrays" << endl;


    // Save copy of enums to test against post-serialization
    InternalType array1Save = array1;
    Example::ArrayType array2Save = array2;

    cout << "Deserializing arrays" << endl;
    if (arraySerial1.deserialize(array1Save) != Fw::FW_SERIALIZE_OK) {
        cout << "ERROR: bad deserialization array1." << endl;
    } else {
        cout << "Deserialized array1" << endl;
    }
    if(arraySerial2.deserialize(array2Save) != Fw::FW_SERIALIZE_OK) {
        cout << "ERROR: bad deserialization array2." << endl;
    } else {
        cout << "Deserialized array2" << endl;
    }
            
    cout << "Deserialized arrays" << endl;

    if (array1 != array1Save) {
        cout << "ERROR: bad array1 check." << endl;
    } else {
        cout << "Successful array1 check" << endl;
    }
    if (array2 != array2Save) {
        cout << "ERROR: bad array2 check." << endl;
    } else {
        cout << "Successful array2 check" << endl;
    }
            
    // Create serializable
    int integer1 = 100;
    int integer2 = 10000;
    serial1 = Example::ArrSerial();
    serial1.setMember1(integer1);
    serial1.setMember2(integer2);
    serial1.setMember3(array1);
            
    // // Invoke ports to test enum usage
    // cout << "Invoking input ports..." << endl;
    // cout << "Invoking inst1..." << endl;
    // inst1->get_ExEnumIn_InputPort(0)->invoke(array1, serial1);
            
    // // Wait for port handler to execute
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
    // cout << "Invoking inst2..." << endl;
    // inst2->get_ExEnumIn_InputPort(0)->invoke(array1, serial1);
            
    // // Wait for port handler to execute
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
    cout << "Invoked ports" << endl;

    cout << "Quitting..." << endl;
    
    cout << "Deleting components..." << endl;
    delete inst1;
    delete inst2;
    cout << "Delete registration objects..." << endl;
    delete simpleReg_ptr;
    cout << "Completed..." << endl;
    
    return 0;
}
