/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef POSIXTIMEIMPL_HPP_
#define POSIXTIMEIMPL_HPP_

#include <Svc/PosixTime/PosixTimeComponentAc.hpp>

namespace Svc {

class PosixTime: public PosixTimeComponentBase {
    public:
        PosixTime(const char* compName);
        virtual ~PosixTime();
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif /* POSIXTIMEIMPL_HPP_ */
