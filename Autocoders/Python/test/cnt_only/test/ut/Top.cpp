#include <Autocoders/Python/test/cnt_only/Components.hpp>
#include <Autocoders/Python/test/cnt_only/DuckTopologyAppAc.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <string.h>

#include <Autocoders/Python/test/cnt_only/DuckImpl.hpp>

#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleReg_ptr = 0;

// Component instances
extern Duck::DuckImpl Huey;
extern Duck::DuckImpl Duey;

extern "C" {
	void dumparch(void);
	void dumpobj(const char* objName);
}

#ifdef TGT_OS_TYPE_LINUX
extern "C" {
	int main(int argc, char* argv[]);
};
#endif

int main(int argc, char* argv[])  {

        Huey.init(10);
        Duey.init(10);
	// Construct the topology here.
	constructDuckArchitecture();

        //Start Components
        Huey.start(0, 10 * 1024, 100);
        Duey.start(0, 10 * 1024, 100);

	// Ask for input to huey or duey here.
	char in[80];
	int cmd;
	Fw::EightyCharString *str;
	char str2[80];
	char run[1];
	while ( strcmp(in,"quit") != 0) {

        //
		cout << "Enter cmd number: ";
		cin >> cmd;
		//
		cout << "Enter short string: ";
		//cin.getline(str2,80);
		cin >> str2;
		cout << "The string 2 is: " << str2 << endl;
		str = new Fw::EightyCharString(str2);
		//cout << "The string is: " << str->toChar() << endl;
		//
		cout << "Enter component name for ExtCmdIn execution (huey or duey): ";
		cin >> in;
		//
		if (in[0]=='h') {
			cout << "hueyComp.get_ExtCmdIn_InputPort()->msg1_in(" << cmd << "," << str << ");" << endl;
			Huey.get_CmdIn_InputPort(0)->invoke(cmd,*str);
			cout << "huey ExtCmdIn call completed..." << endl;
		} else if (in[0] == 'd') {
			cout << "dueyComp.get_ExtCmdIn_InputPort()->msg1_in(" << cmd << "," << str << ");" << endl;
			Duey.get_ExtCmdIn_InputPort(0)->invoke(cmd,*str);
			cout << "duey ExtCmdIn call completed..." << endl;
		} else if (strcmp(in,"quit")==0) {
			cout << "quit demo!" << endl;
		} else {
			cout << "Unrecognized component name." << endl;
		}
		delete str;
		// Delay to allow components to print messages
		sleep(2);
        break;
	}

	cout << "Delete registration objects..." << endl;
	delete simpleReg_ptr;
	cout << "Completed..." << endl;
}
