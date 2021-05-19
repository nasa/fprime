#include <Autocoders/Python/test/app1b/Top.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <string.h>

#include <Autocoders/Python/test/passive/PassiveC1Impl.hpp>
#include <Autocoders/Python/test/passive/PassiveC2Impl.hpp>

#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleRegPtr = 0;

// Component instance pointers
Passive::C1Impl* C1_ptr = 0;
Passive::C2Impl* C2_ptr = 0;

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
	C1_ptr = new Passive::C1Impl("C1");
	C2_ptr = new Passive::C2Impl("C2");

	// Connect C1 output ports to C2 input ports.
	C1_ptr->getVoidVoidOutputPort()->addCallPort(C2_ptr->getVoidVoidInputPort());
	C1_ptr->getMsg1Msg1OutputPort()->addCallPort(C2_ptr->getMsg1Msg1InputPort());

	// Connect C2 output ports to C1 input ports.
	C2_ptr->getMsg2Msg2OutputPort()->addCallPort(C1_ptr->getSyncMsg2InputPort());

	// Active component startup

	// start(identifier, stack size, priority)
	C1_ptr->start(0, 10 * 1024, 100);

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
		cout << "\nEnter interface number (1 or 2 or quit): ";
		cin >> in;
		//
		if (in[0]=='1') {
			C1_ptr->getExt1VoidInputPort()->msg_in();
		} else if (in[0] == '2') {
			cout << "\nEnter (U32 arg1, I32 arg2, F32 arg3): \n";
			cin >> a1;
			cin >> a2;
			cin >> a3;
			cout << "C2_ptr->getExt2Msg1InputPort()->msg_in(" << a1 << "," << a2 << "," << a3 << ")" << endl;
			C1_ptr->getExt2Msg1InputPort()->msg_in(a1,a2,a3);
		} else if (strcmp(in,"quit")==0) {
			cout << "quit demo!" << endl;
		} else {
			cout << "Unrecognized component." << endl;
		};
		// Delay to allow components to print messages
		sleep(2);
	}

	cout << "Deleting components..." << endl;
	delete C1_ptr;
	delete C2_ptr;
	cout << "Delete registration objects..." << endl;
	delete simpleRegPtr;
	cout << "Completed..." << endl;

	return 0;
}

