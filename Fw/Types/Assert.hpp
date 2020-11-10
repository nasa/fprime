#ifndef FW_ASSERT_HPP
#define FW_ASSERT_HPP

#include <FpConfig.hpp>
#include <Fw/Types/BasicTypes.hpp>

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#define FW_ASSERT(...)
#define FW_STATIC_ASSERT(...)

#else // ASSERT is defined

#define FW_STATIC_CAT_(a, b) a ## b
#define FW_STATIC_CAT(a, b) FW_STATIC_CAT_(a, b)
#define FW_STATIC_ASSERT(cond) typedef int FW_STATIC_CAT(FW_STATIC_ASSERT,__LINE__)[(cond) ? 1 : -1]

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define FILE_NAME_ARG NATIVE_UINT_TYPE
#define FW_ASSERT(cond, ...) \
    ((void) ((cond) ? (0) : \
    (Fw::SwAssert(ASSERT_FILE_ID, __LINE__, ##__VA_ARGS__))))
#else
#define FILE_NAME_ARG U8*
#define FW_ASSERT(cond, ...) \
    ((void) ((cond) ? (0) : \
    (Fw::SwAssert((U8*)__FILE__, __LINE__, ##__VA_ARGS__))))
#endif


namespace Fw {
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo); //!< Assert with no arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1); //!< Assert with one argument
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1, AssertArg arg2); //!< Assert with two arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1, AssertArg arg2, AssertArg arg3); //!< Assert with three arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1, AssertArg arg2, AssertArg arg3, AssertArg arg4); //!< Assert with four arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1, AssertArg arg2, AssertArg arg3, AssertArg arg4, AssertArg arg5); //!< Assert with five arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, AssertArg arg1, AssertArg arg2, AssertArg arg3, AssertArg arg4, AssertArg arg5, AssertArg arg6); //!< Assert with six arguments

}

// Base class for declaring an assert hook
// Each of the base class functions can be overriden
// or used by derived classes.

namespace Fw {
    // Base class for declaring an assert hook
    class AssertHook {
        public:
            AssertHook() : previousHook(NULL) {}; //!< constructor
            virtual ~AssertHook() {}; //!< destructor
            // override this function to intercept asserts
            virtual void reportAssert(
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
            // default reportAssert() will call this when the message is built
            // override it to do another kind of print. printf by default
            virtual void printAssert(const I8* msg);
            // do assert action. By default, calls assert.
            // Called after reportAssert()
            virtual void doAssert(void);
            // register the hook
            void registerHook(void);
            // deregister the hook
            void deregisterHook(void);

        protected:
        private:
            // the previous assert hook
            AssertHook *previousHook;
    };


}
#endif // if ASSERT is defined


#endif
