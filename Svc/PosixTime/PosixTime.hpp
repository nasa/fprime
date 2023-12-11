/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef POSIX_TIME_HPP_
#define POSIX_TIME_HPP_

#include <Svc/PosixTime/PosixTimeComponentAc.hpp>

namespace Svc {

class PosixTime: public PosixTimeComponentBase {
    public:
        explicit PosixTime(const char* compName);
        virtual ~PosixTime();
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif /* POSIX_TIME_HPP_ */
