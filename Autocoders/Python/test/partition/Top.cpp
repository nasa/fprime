//--------------------------------------------------------------------------------------
// Top1.cpp
//
// This application contains a Duck and Partition component only
// This is a simple passive test case based on the Partition component
// that only contains serializable port types and is passive.
//
//--------------------------------------------------------------------------------------
#include <Autocoders/Python/test/partition/Top.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <cstring>

#include <Autocoders/Python/test/partition/DuckDuckImpl.hpp>
#include <Autocoders/Python/test/partition/PartitionImpl.hpp>

#include <unistd.h>


using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleRegPtr = 0;

// Component instance pointers
Duck::DuckImpl* hueyComp_ptr = 0;
Partition::PartitionImpl *partitionComp_ptr = 0;


extern "C" {
	void dumparch();
	void dumpobj(const char* objName);
}


void dumparch() {
	simpleRegPtr->dump();
}

void dumpobj(const char* objName) {
	simpleRegPtr->dump(objName);
}


void constructArchitecture() {

	Fw::PortBase::setTrace(true);

	simpleRegPtr = new Fw::SimpleObjRegistry();

	// Instantiate the Huey
	hueyComp_ptr   = new Duck::DuckImpl("Huey");
	//printf("*** Instantiated Huey\n");

	// Instantiate the Partition Base component
	partitionComp_ptr = new Partition::PartitionImpl("HueyPartition");
	//printf("*** Instantiated HueyPartition\n");

	// Connect Huey output port to Partition Comp input port.
	hueyComp_ptr->getoutputPort1Msg1OutputPort()->registerSerialPort(partitionComp_ptr->getinputPort1SerializeInputPort());
	hueyComp_ptr->getoutputPort2Msg3OutputPort()->registerSerialPort(partitionComp_ptr->getinputPort2SerializeInputPort());

	// Connect Partition Comp output port to Huey input port.
	partitionComp_ptr->getoutputPort1SerializeOutputPort()->registerSerialPort(hueyComp_ptr->getinputPort2Msg1InputPort());
	partitionComp_ptr->getoutputPort2SerializeOutputPort()->registerSerialPort(hueyComp_ptr->getinputPort3Msg3InputPort());

	// Active component startup
	// start()
	hueyComp_ptr->start();

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
	char in[80] = {};
	U32 cmd;
	Fw::String *str;
	char str2[80];
	//
	while ( strcmp(in,"quit") != 0) {
		//
		cout << "\nEnter interface number (1 or 3 or quit): ";
		cin >> in;
		//
		if (in[0]=='1') {
			//
			cout << "Enter cmd number: ";
			cin >> cmd;
			//
			cout << "Enter short string: ";
			cin >> str2;
			cout << "The string 2 is: " << str2 << endl;
			str = new Fw::String(str2);
			cout << "hueyComp_ptr->getexternInputPort1InputPort()->msg_in(" << cmd << "," << str2 << ")" << endl;
			hueyComp_ptr->getexternInputPort1Msg1InputPort()->msg_in(cmd,*str);
		} else if (in[0] == '3') {
			//
			cout << "Enter cmd number: ";
			cin >> cmd;
			cout << "hueyComp_ptr->getexternInputPort1InputPort()->msg_in(" << cmd << ")" << endl;
			hueyComp_ptr->getexternInputPort3Msg3InputPort()->msg_in(cmd);
		} else if (strcmp(in,"quit")==0) {
			cout << "quit demo!" << endl;
		} else {
			cout << "Unrecognized component." << endl;
		};
	}
	cout << "Deleting components..." << endl;
	delete hueyComp_ptr;
	delete partitionComp_ptr;
	cout << "Delete registration objects..." << endl;
	delete simpleRegPtr;
	cout << "Completed..." << endl;

    return 0;

}


