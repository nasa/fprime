/*
 * LogAssert.hpp
 *
 *  Created on: Sep 9, 2016
 *      Author: tcanham
 * Note: this file was originally a  log assert file, under Fw::Types. It now made generic
 * to log asserts to Fw::Logger
 */

#ifndef LOGGER_LOGASSERT_HPP_
#define LOGGER_LOGASSERT_HPP_

#include <Fw/Types/Assert.hpp>

namespace Fw {

    class LogAssertHook: public Fw::AssertHook {
        public:
            LogAssertHook();
            virtual ~LogAssertHook();
            void reportAssert(
                FILE_NAME_ARG file,
                NATIVE_UINT_TYPE lineNo,
                NATIVE_UINT_TYPE numArgs,
                FwAssertArgType arg1,
                FwAssertArgType arg2,
                FwAssertArgType arg3,
                FwAssertArgType arg4,
                FwAssertArgType arg5,
                FwAssertArgType arg6
            );
            void printAssert(const CHAR* msg);
            void doAssert();
    };

}

#endif /* VXWORKSLOGASSERT_HPP_ */
