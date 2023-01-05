#include <Autocoders/Python/test/array_xml/Component1ComponentAc.hpp>
#include <Autocoders/Python/test/array_xml/InternalTypeArrayAc.hpp>
#include <Autocoders/Python/test/array_xml/StringArrayArrayAc.hpp>
#include <Autocoders/Python/test/array_xml/ArrayTypeArrayAc.hpp>
#include <Autocoders/Python/test/array_xml/Port1PortAc.hpp>
#include <Autocoders/Python/test/array_xml/ArrSerialSerializableAc.hpp>

#include <Autocoders/Python/test/array_xml/ExampleArrayImpl.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/Assert.hpp>

#include <bitset>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;

// Component instance pointers
Example::ExampleArrayImpl* inst1 = nullptr;
Example::ExampleArrayImpl* inst2 = nullptr;

#ifdef TGT_OS_TYPE_LINUX
extern "C" {
    int main(int argc, char* argv[]);
};
#endif

int main(int argc, char* argv[]) {
    setbuf(stdout, nullptr);

    cout << "Initialize Arrays" << endl;

    InternalType array1 = InternalType(6,7,120,444);
    Example::ArrayType array2 = Example::ArrayType(array1);
    // Create string array for serializable
    Fw::String mem1 = "Member 1";
    Fw::String mem2 = "Member 2";
    Fw::String mem3 = "Member 3";
    StringArray array3 = StringArray(mem1, mem2, mem3);
    Example::ArrSerial serial1;

    // Print toString outputs for each array
    cout << "Print toString for arrays" << endl;

    Fw::String tostring1;
    Fw::String tostring2;
    Fw::String tostring3;
    array1.toString(tostring1);
    array2.toString(tostring2);
    array3.toString(tostring3);

    cout << "Integer array: " << tostring1 << endl;
    cout << "2D integer array: " << tostring2 << endl;
    cout << "String array: " << tostring3 << endl;

    // Serialize arrays
    cout << "Serialize arrays" << endl;
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


    // Save copy of arrays to test against post-serialization
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
    serial1.setMember4(array3);

    cout << "Invoked ports" << endl;

    cout << "Quitting..." << endl;

    cout << "Deleting components..." << endl;
    delete inst1;
    delete inst2;
    cout << "Delete registration objects..." << endl;
    cout << "Completed..." << endl;

    return 0;
}
