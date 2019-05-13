#include <Autocoders/Python/test/active_tester/Simple_ActiveImpl.hpp>
#include <Autocoders/Python/test/active_tester/Simple_Active_TesterImpl.hpp>


#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

extern "C++" void constructActive_TesterArchitecture();

Simple_Active_Tester::Simple_Active_TesterImpl CompA_Tester("CompA_Tester");
Simple_Active::Simple_ActiveImpl CompA("CompA");

void test_U32_Async(){
       int int_to_send;
       cout << "Enter an unsigned-integer to send to CompA: ";
       cin >> int_to_send;
       CompA_Tester.U32_out(0,int_to_send);
}

void test_F32_Sync(){
    float float_to_send;
    cout << "Enter a float to send to CompA: ";
    cin >> float_to_send;
    CompA_Tester.F32_out(0, float_to_send);
}

int main(int argc, char* argv[]){

    char charIn[1];

    constructActive_TesterArchitecture();

    while(true){

        cout << "Enter g to go or q to quit: ";
        cin >> charIn;

        if(strcmp(charIn,"g")==0){
            charIn[0] = 0;
            cout << "Enter u for unsigned-int and f for float:";
            cin >> charIn;

            if(strcmp(charIn,"u") == 0){
                test_U32_Async();
            }else{
                test_F32_Sync();
            }

        }else{
            break;
        }
    }

    return 0;
}
