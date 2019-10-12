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
#include <thread>
#include <chrono>

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
    inst1->set_EnumOut_OutputPort(0, inst2->get_EnumIn_InputPort(0));
    
    // Connect inst2 to inst1
    inst2->set_EnumOut_OutputPort(0, inst1->get_EnumIn_InputPort(0));
    
    // Instantiate components
    inst1->init(100);
    inst2->init(100);
    inst1->start(0, 100, 10 * 1024);
    inst2->start(0, 100, 10 * 1024);
    
    dumparch();
}

Example::Enum1 getEnumItem(int num) {
    Example::Enum1 tempEnum;
    
    switch(num) {
        case 1:
            tempEnum = Example::Enum1::Item1;
            break;
        case 2:
            tempEnum = Example::Enum1::Item2;
            break;
        case 3:
            tempEnum = Example::Enum1::Item3;
            break;
        case 4:
            tempEnum = Example::Enum1::Item4;
            break;
        case 5:
            tempEnum = Example::Enum1::Item5;
            break;
        default:
            cout << "Enum item number should be between 1 and 5";
            exit(-1);
    }
    
    return tempEnum;
}

int main(int argc, char* argv[]) {
    // Construct the topology here.
    constructArchitecture();
    
    setbuf(stdout, NULL);

    char in[80];
    int testNumber = -1;
    
    while ( strcmp(in,"quit") != 0) {
        cout << "Enter any button to run or q to quit: ";
        cin >> in;
        if (strcmp(in,"q")==0){
            cout << "Quitting Program" << endl;
            break;
        }
        
        cout << "Enter test case (1-2): ";
        cin >> testNumber;
        
        if(testNumber < 1 || testNumber > 2) {
            cout << "Test case must be between 1 and 2" << endl;
            continue;
        }
        
        // Test case 1 invokes ExEnumIn port and uses int literals to define
        // enums, test case 2 invokes EnumIn port and uses enum item number to
        // define enums
        if(testNumber == 1) {
            cout << "----------Test Number 1----------" << endl;
            
            Example::Enum1 enum1;
            Example::Enum1 enum2;
            Example::Serial1 serial1;
            
            int enum1_number;
            cout << "Enter first enum item number using int literal (-1952875139, 2, 2000999333, 21, -8324876): ";
            cin >> enum1_number;
            enum1 = enum1_number;
            
            int enum2_number;
            cout << "Enter second enum item number using int literal (-1952875139, 2, 2000999333, 21, -8324876): ";
            cin >> enum2_number;
            enum2 = enum2_number;
            
            cout << "Created first enum: " << enum1.e << endl;
            cout << "Created second enum: " << enum2.e << endl;
            
            // Save copy of enums to test against post-serialization
            Example::Enum1 enum1Save = enum1;
            Example::Enum1 enum2Save = enum2;
            
            int serial_arg1;
            cout << "Enter an unsigned integer for serializable arg1: ";
            cin >> serial_arg1;
            
            int serial_arg2;
            cout << "Enter an unsigned integer for serializable arg2: ";
            cin >> serial_arg2;
            
            // Serialize enums
            cout << "Serialize enums (y/n): ";
            cin >> in;
            U8 buffer1[1024];
            U8 buffer2[1024];
            Fw::SerialBuffer enumSerial1 = Fw::SerialBuffer(buffer1, sizeof(buffer1));
            Fw::SerialBuffer enumSerial2 = Fw::SerialBuffer(buffer2, sizeof(buffer2));
            if(strcmp(in, "y") == 0) {
                if (enumSerial1.serialize(enum1) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad serialization enum1." << endl;
                } else {
                    cout << "Serialized enum1" << endl;
                }
                
                if (enumSerial2.serialize(enum2) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad serialization enum2." << endl;
                } else {
                    cout << "Serialized enum2" << endl;
                }
                
                cout << "Serialized enums" << endl;
            }

            cout << "Deserialize enums (y/n): ";
            cin >> in;
            if(strcmp(in, "y") == 0) {
                // Deserialize enums
                if (enumSerial1.deserialize(enum1Save) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad deserialization enum1." << endl;
                } else {
                    cout << "Deserialized enum1" << endl;
                }
                if(enumSerial2.deserialize(enum2Save) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad deserialization enum2." << endl;
                } else {
                    cout << "Deserialized enum2" << endl;
                }
            }
            
            cout << "Run enum check (y/n): ";
            cin >> in;
            if(strcmp(in, "y") == 0) {
                if (enum1 != enum1Save) {
                    cout << "ERROR: bad enum1 check." << endl;
                } else {
                    cout << "Successful enum1 check" << endl;
                }
                if (enum2 != enum2Save) {
                    cout << "ERROR: bad enum2 check." << endl;
                } else {
                    cout << "Successful enum2 check" << endl;
                }
            }
            
            cout << "Deserialized enums" << endl;
            
            // Create serializable and test that enum is saved
            cout << "Create serializable (y): ";
            cin >> in; // This stops the program so that pexpect gen proper error message if this fails
            serial1 = Example::Serial1(serial_arg1, serial_arg2, enum2);
            if (serial1.getMember3() != enum2) {
                cout << "ERROR: bad serializable with enum arg." << endl;
            } else {
                cout << "Created serializable with enum arg" << endl;
            }
            
            cout << "Created serializable" << endl;
            
            // Invoke ports to test enum usage
            int invoke_no = 1;
            cout << "Invoking input ports..." << endl;
            cout << "Invoking inst1..." << endl;
            cout << "Which enum should be used to invoke inst1 (1-2): ";
            cin >> invoke_no;
            if(invoke_no == 1) {
                inst1->get_ExEnumIn_InputPort(0)->invoke(enum1, serial1);
            } else if(invoke_no == 2) {
                inst1->get_ExEnumIn_InputPort(0)->invoke(enum2, serial1);
            } else {
                cout << "Invalid enum number" << endl;
            }
            
            // Wait for port handler to execute
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            cout << "Invoking inst2..." << endl;
            cout << "Which enum should be used to invoke inst2 (1-2): ";
            cin >> invoke_no;
            if(invoke_no == 1) {
                inst2->get_ExEnumIn_InputPort(0)->invoke(enum1, serial1);
            } else if(invoke_no == 2) {
                inst2->get_ExEnumIn_InputPort(0)->invoke(enum2, serial1);
            } else {
                cout << "Invalid enum number" << endl;
            }
            
            // Wait for port handler to execute
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            cout << "Invoked ports" << endl;
        }
        
        if(testNumber == 2) {
            cout << "----------Test Number 2----------" << endl;
            
            Example::Enum1 enum1;
            Example::Enum1 enum2;
            Example::Serial1 serial1;
            
            int enum1_number;
            cout << "Enter first enum item number using item object(1-5): ";
            cin >> enum1_number;
            enum1 = getEnumItem(enum1_number);
            
            int enum2_number;
            cout << "Enter second enum item number using item object (1-5): ";
            cin >> enum2_number;
            enum2 = getEnumItem(enum2_number);
            
            cout << "Created first enum: " << enum1.e << endl;
            cout << "Created second enum: " << enum2.e << endl;
            
            // Save copy of enums to test against post-serialization
            Example::Enum1 enum1Save = enum1;
            Example::Enum1 enum2Save = enum2;
            
            int serial_arg1;
            cout << "Enter an integer for serializable arg1: ";
            cin >> serial_arg1;
            
            int serial_arg2;
            cout << "Enter an integer for serializable arg2: ";
            cin >> serial_arg2;
            
            // Serialize enums
            cout << "Serialize enums (y/n): ";
            cin >> in;
            U8 buffer1[1024];
            U8 buffer2[1024];
            Fw::SerialBuffer enumSerial1 = Fw::SerialBuffer(buffer1, sizeof(buffer1));
            Fw::SerialBuffer enumSerial2 = Fw::SerialBuffer(buffer2, sizeof(buffer2));
            if(strcmp(in, "y") == 0) {
                if (enumSerial1.serialize(enum1) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad serialization enum1." << endl;
                } else {
                    cout << "Serialized enum1" << endl;
                }
                
                if (enumSerial2.serialize(enum2) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad serialization enum2." << endl;
                } else {
                    cout << "Serialized enum2" << endl;
                }
                
                cout << "Serialized enums" << endl;
            }
            
            cout << "Deserialize enums (y/n): ";
            cin >> in;
            if(strcmp(in, "y") == 0) {
                // Deserialize enums
                if (enumSerial1.deserialize(enum1Save) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad deserialization enum1." << endl;
                } else {
                    cout << "Deserialized enum1" << endl;
                }
                if(enumSerial2.deserialize(enum2Save) != Fw::FW_SERIALIZE_OK) {
                    cout << "ERROR: bad deserialization enum2." << endl;
                } else {
                    cout << "Deserialized enum2" << endl;
                }
            }
            
            cout << "Run enum check (y/n): ";
            cin >> in;
            if(strcmp(in, "y") == 0) {
                if (enum1 != enum1Save) {
                    cout << "ERROR: bad enum1 check." << endl;
                } else {
                    cout << "Successful enum1 check" << endl;
                }
                if (enum2 != enum2Save) {
                    cout << "ERROR: bad enum2 check." << endl;
                } else {
                    cout << "Successful enum2 check" << endl;
                }
            }
            
            cout << "Deserialized enums" << endl;
            
            // Create serializable and test that enum is saved
            cout << "Create serializable (y): ";
            cin >> in; // This stops the program so that pexpect gen proper error message if this fails
            serial1 = Example::Serial1(serial_arg1, serial_arg2, enum2);
            if (serial1.getMember3() != enum2) {
                cout << "ERROR: bad serializable with enum arg." << endl;
            } else {
                cout << "Created serializable with enum arg" << endl;
            }
            
            cout << "Created serializable" << endl;
            
            // Invoke ports to test enum usage
            int invoke_no = 1;
            cout << "Invoking input ports..." << endl;
            cout << "Invoking inst1..." << endl;
            cout << "Which enum should be used to invoke inst1 (1-2): ";
            cin >> invoke_no;
            if(invoke_no == 1) {
               inst1->get_EnumIn_InputPort(0)->invoke(enum1, serial1);
            } else if(invoke_no == 2) {
                inst1->get_EnumIn_InputPort(0)->invoke(enum2, serial1);
            } else {
                cout << "Invalid enum number" << endl;
            }
            // Wait for port handler to execute
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            cout << "Invoking inst2..." << endl;
            cout << "Which enum should be used to invoke inst2 (1-2): ";
            cin >> invoke_no;
            if(invoke_no == 1) {
                inst2->get_EnumIn_InputPort(0)->invoke(enum1, serial1);
            } else if(invoke_no == 2) {
                inst2->get_EnumIn_InputPort(0)->invoke(enum2, serial1);
            } else {
                cout << "Invalid enum number" << endl;
            }
            
            // Wait for port handler to execute
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            cout << "Invoked ports" << endl;
        }
    }

    cout << "quit demo!" << endl;
    
    cout << "Deleting components..." << endl;
    delete inst1;
    delete inst2;
    cout << "Delete registration objects..." << endl;
    delete simpleReg_ptr;
    cout << "Completed..." << endl;
    
    return 0;
}
