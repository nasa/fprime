#include "decls.h"

namespace Fw {
    int SwAssert(unsigned int file, unsigned int lineNo) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1, unsigned int arg2) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5) {
        __coverity_panic__();
        return 0;
    }

    unsigned int  SwAssert(unsigned int file, unsigned int  lineNo, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6) {
        __coverity_panic__();
        return 0;
    }

}

