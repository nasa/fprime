#include <Autocoders/Python/test/enum_xml/ExampleEnumComponentAc.hpp>
#include <Autocoders/Python/test/enum_xml/Enum1EnumAc.hpp>
#include <Autocoders/Python/test/enum_xml/ExampleEnumPortAc.hpp>
#include <Autocoders/Python/test/enum_xml/ExampleEnumSerializableAc.hpp>

#include <Autocoders/Python/test/enum_xml/ExampleEnumImpl.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleReg_ptr = 0;

// Component instance pointers
extern Example::ExampleEnumImpl* inst1 = 0;
extern Example::ExampleEnumImpl* inst2 = 0;

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
    
    // Instantiate the Huey and Duey components
    inst1   = new Example::ExampleEnumImpl("inst1");
    inst2   = new Example::ExampleEnumImpl("inst2");
    
    
    // Connect inst1 to inst2
    inst1->set_ExEnumOut_OutputPort(0, inst2->get_ExEnumIn_InputPort(0));
    
    // Connect inst2 to inst1
    inst2->set_ExEnumOut_OutputPort(0, inst1->get_ExEnumIn_InputPort(0));
    
    // Active component startup?
    
    dumparch();
}

int main(int argc, char* argv[]) {
    // Construct the topology here.
    constructArchitecture();

    // Ask for input to components here.
    char in[80];
    NATIVE_INT_TYPE enum_num;
    int serial_arg1;
    int serial_arg2;
    NATIVE_INT_TYPE serial_arg3;
    char str[80];
    //
    while ( strcmp(in,"quit") != 0) {
        cout << "Enter any button to run or q to quit: ";
        cin >> in;
        if (strcmp(in,"q")==0){
            cout << "Quitting Program";
            break;
        }
        
        Example::Enum1 *enum1;
        Example::Enum1 *enum2;
        Example::Serial1 *serial1;
        
        // Creating enum argument
        cout << "Enter enum number: ";
        cin >> enum_num;
        *enum1 = enum_num;
        
        // Creating serializable argument
        cout << "Enter U32 member1: ";
        cin >> serial_arg1;
        cout << "Enter U32 member2: ";
        cin >> serial_arg2;
        cout << "Enter enum number: ";
        cin >> serial_arg3;
        *enum2 = serial_arg3;
        serial1 = new Example::Serial1(serial_arg1, serial_arg2, *enum2);
        
//        // Executing components
//        cout << "Enter component name for ExtCmdIn execution (huey or duey): ";
//        cin >> in;
//        //
//        if (in[0]=='h') {
//            cout << "hueyComp_ptr->get_ExtCmdIn_InputPort()->msg1_in(" << cmd << "," << str << ");" << endl;
//            Huey_ptr->get_CmdIn_InputPort(0)->invoke(cmd,*str);
//            cout << "huey ExtCmdIn call completed..." << endl;
//        } else if (in[0] == 'd') {
//            cout << "dueyComp_ptr->get_ExtCmdIn_InputPort()->msg1_in(" << cmd << "," << str << ");" << endl;
//            Duey_ptr->get_ExtCmdIn_InputPort(0)->invoke(cmd,*str);
//            cout << "duey ExtCmdIn call completed..." << endl;
//        } else if (strcmp(in,"quit")==0) {
//            cout << "quit demo!" << endl;
//        } else {
//            cout << "Unrecognized component name." << endl;
//        }
        
        delete enum1;
        delete enum2;
        delete serial1;
        
        // Delay to allow components to print messages
        sleep(2);
    }
    
    cout << "Deleting components..." << endl;
    delete inst1;
    delete inst2;
    cout << "Delete registration objects..." << endl;
    delete simpleReg_ptr;
    cout << "Completed..." << endl;
}
