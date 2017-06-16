/*
 * VxWorksLogAssert.hpp
 *
 *  Created on: Sep 9, 2016
 *      Author: tcanham
 */

#ifndef VXWORKSLOGASSERT_HPP_
#define VXWORKSLOGASSERT_HPP_

#include <Fw/Types/Assert.hpp>

namespace Fw {

    class VxWorksLogAssertHook: public Fw::AssertHook {
        public:
            VxWorksLogAssertHook();
            virtual ~VxWorksLogAssertHook();
            void reportAssert(
                                FILE_NAME_ARG file,
                                NATIVE_UINT_TYPE lineNo,
                                NATIVE_UINT_TYPE numArgs,
                                AssertArg arg1,
                                AssertArg arg2,
                                AssertArg arg3,
                                AssertArg arg4,
                                AssertArg arg5,
                                AssertArg arg6
                                );
            void printAssert(const I8* msg);
            void doAssert(void);
    };

}

#endif /* VXWORKSLOGASSERT_HPP_ */
