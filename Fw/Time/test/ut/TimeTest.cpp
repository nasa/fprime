/*
 * TimeTest.cpp
 *
 *  Created on: Apr 22, 2016
 *      Author: tcanham
 */

#include <Fw/Time/Time.hpp>
#include <iostream>

int main(int argc, char* argv) {

    Fw::Time time(TB_NONE,1,2);
    std::cout << time << std::endl;
}
