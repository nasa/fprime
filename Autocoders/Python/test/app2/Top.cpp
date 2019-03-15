#include <Autocoders/Python/test/app2/Top.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <string.h>

#include <Autocoders/Python/test/app2/App2VoidArgImpl.hpp>

#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleRegPtr = 0;

// Component instance pointers
App2::VoidArgImpl* C1_ptr = 0;
App2::VoidArgImpl* C2_ptr = 0;

extern "C" {
	void dumparch(void);
	void dumpobj(const char* objName);
}


void dumparch(void) {
	simpleRegPtr->dump();
}

void dumpobj(const char* objName) {
	simpleRegPtr->dump(objName);
}


void constructArchitecture(void) {

	Fw::PortBase::setTrace(true);

	simpleRegPtr = new Fw::SimpleObjRegistry();

	// Instantiate the C1 and C2 components
	C1_ptr = new App2::VoidArgImpl("C1");
	C2_ptr = new App2::VoidArgImpl("C2");

	C1_ptr->init(10);
	C2_ptr->init(10);

	// Connect C1 output ports to C2 input ports.
	C1_ptr->set_Out_OutputPort(0,C2_ptr->get_In_InputPort(0));
	C1_ptr->set_MsgOut_OutputPort(0,C2_ptr->get_MsgIn_InputPort(0));

	// Connect C2 output ports to C1 input ports.
	C2_ptr->set_Out_OutputPort(0,C1_ptr->get_In_InputPort(0));
	C2_ptr->set_MsgOut_OutputPort(0,C1_ptr->get_MsgIn_InputPort(0));

	// Active component startup

	// start(identifier, stack size, priority)
	C1_ptr->start(0, 10 * 1024, 100);
	C2_ptr->start(0, 10 * 1024, 100);

	dumparch();

}

#ifdef TGT_OS_TYPE_LINUX
extern "C" {
	int main(int argc, char* argv[]);
};
#endif

int main(int argc, char* argv[])  {
	// Construct the topology here.
	constructArchitecture();
	// Ask for input to huey or duey here.
	char in[80];
	int in2;
	U32 a1;
	I32 a2;
	F32 a3;
	//
	while ( strcmp(in,"quit") != 0) {
		//
		cout << "\nEnter component number (1 or 2 or quit): ";
		cin >> in;
		//
		if (in[0]=='1') {
			cout << "\nEnter 0 for void interface and 1 for args interface: ";
			cin >> in2;
			if (in2 == 0) {
				cout << "C1_ptr->getExtInVoidInputPort(0)->invoke();" << endl;
				C1_ptr->get_ExtIn_InputPort(0)->invoke();
			} else {
				cout << "\nEnter (U32 arg1, I32 arg2, F32 arg3): \n";
				cin >> a1;
				cin >> a2;
				cin >> a3;
				cout << "C1_ptr->getExtMsgInMsgInputPort(0)->invoke(" << a1 << "," << a2 << "," << a3 << ")" << endl;
				C1_ptr->get_ExtMsgIn_InputPort(0)->invoke(a1,a2,a3);
			};
		} else if (in[0] == '2') {
			cout << "\nEnter 0 for void interface and 1 for args interface: ";
			cin >> in2;
			if (in2 == 0) {
				cout << "C2_ptr->getExtInVoidInputPort(0)->invoke();" << endl;
				C2_ptr->get_ExtIn_InputPort(0)->invoke();
			} else {
				cout << "\nEnter (U32 arg1, I32 arg2, F32 arg3): \n";
				cin >> a1;
				cin >> a2;
				cin >> a3;
				cout << "C2_ptr->getExtMsgInMsgInputPort(0)->invoke(" << a1 << "," << a2 << "," << a3 << ")" << endl;
				C2_ptr->get_ExtMsgIn_InputPort(0)->invoke(a1,a2,a3);
			};
		} else if (strcmp(in,"quit")==0) {
			cout << "quit demo!" << endl;
		} else {
			cout << "Unrecognized component." << endl;
		};
//		delete str;
		// Delay to allow components to print messages
		sleep(2);
	}

	cout << "Deleting components..." << endl;
	delete C1_ptr;
	delete C2_ptr;
	cout << "Delete registration objects..." << endl;
	delete simpleRegPtr;
	cout << "Completed..." << endl;
}

