/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef LINUXTIMEIMPL_HPP_
#define LINUXTIMEIMPL_HPP_

#include <Svc/Time/TimeComponentAc.hpp>

namespace Svc {

class LinuxTimeImpl: public TimeComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        LinuxTimeImpl(const char* compName);
#else
        LinuxTimeImpl();
#endif
        virtual ~LinuxTimeImpl();
        void init(NATIVE_INT_TYPE instance);
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif /* LINUXTIMEIMPL_HPP_ */
