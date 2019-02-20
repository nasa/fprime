#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <string.h>
#include <Autocoders/Python/test/serialize2/GncMeasurementSerializableAc.hpp>

#include <unistd.h>

using namespace std;

// Serializable instance pointers
Ref::Gnc::Quaternion* q_ptr = 0;
Ref::Gnc::GncMeasurement* S1_ptr = 0;
Ref::Gnc::GncMeasurement* S2_ptr = 0;
Ref::Gnc::GncMeasurement* S3_ptr = 0;

// Test buffer

class SerializeTestBuffer : public Fw::SerializeBufferBase {
    public:
        I32 getBuffCapacity(void) const { // !< returns capacity, not current size, of buffer
            return sizeof(m_testBuff);
        }

        U8* getBuffAddr(void) { // !< gets buffer address for data filling
            return m_testBuff;
        }
        const U8* getBuffAddr(void) const { // !< gets buffer address for data reading
            return m_testBuff;
        }
    private:
        U8 m_testBuff[255];
};


#ifdef TGT_OS_TYPE_LINUX
extern "C" {
	int main(int argc, char* argv[]);
};
#endif

void show(int n, Ref::Gnc::GncMeasurement *p) {
		cout << "S" << n << ": timeStamp = " << p->gettimeStamp()
						 << ", Q1 = " << p->getquaternion().getQ1()
						 << ", Q2 = " << p->getquaternion().getQ2()
						 << ", Q3 = " << p->getquaternion().getQ3()
						 << ", Q4 = " << p->getquaternion().getQ4() << endl;
}

int main(int argc, char* argv[])  {
	SerializeTestBuffer buffer;

	// Default construction
	S1_ptr = new Ref::Gnc::GncMeasurement();
	cout << "Default construction of S1." << endl;
	show(1,S1_ptr);

    // Constructor with arguments
	q_ptr = new Ref::Gnc::Quaternion(1.2, 3.4, 5.6, 7.8);
    S2_ptr = new Ref::Gnc::GncMeasurement(12345678, *q_ptr);
	cout << "Constructor with arguments of S2.." << endl;
	show(2,S2_ptr);

    // copy constructor with pointer argument
    S3_ptr = new Ref::Gnc::GncMeasurement(S2_ptr);
    cout << "Copy constructor with pointer argument of S2 copied to S3..." << endl;
    show(3,S3_ptr);

    // setting S1 to non-zero values
    q_ptr->setQ1(9.10);
    q_ptr->setQ2(11.12);
    q_ptr->setQ3(13.14);
    q_ptr->setQ4(15.16);
    S1_ptr->settimeStamp(87654321);
    S1_ptr->setquaternion(*q_ptr);
    cout << "Setting S1 to non-zero values...." << endl;
    show(1,S1_ptr);

    // copy constructor with reference argument
    S1_ptr = new Ref::Gnc::GncMeasurement(S2_ptr);
    cout << "Copy constructor with reference argument of S2 copied to S1....." << endl;
    show(1,S1_ptr);

    // setting S3 to new values
    q_ptr->setQ1(17.18);
    q_ptr->setQ2(19.20);
    q_ptr->setQ3(21.22);
    q_ptr->setQ4(23.24);
    S3_ptr->settimeStamp(91011121314);
    S3_ptr->setquaternion(*q_ptr);
    cout << "Setting S3 to new values......" << endl;
    show(3,S3_ptr);

    // = constructor
    S1_ptr = S3_ptr;
    cout << "Setting S1 = S3......." << endl;
    show(1,S1_ptr);

    // Testing serialization here
    cout << "Testing serialization here (S2 -> S1)........" << endl;
    S2_ptr->serialize(buffer);
    S1_ptr->deserialize(buffer);
    show(1,S1_ptr);
    cout << "End of testing serializable." << endl;

    return 0;
}

