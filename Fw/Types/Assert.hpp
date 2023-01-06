#ifndef FW_ASSERT_HPP
#define FW_ASSERT_HPP

#include <FpConfig.hpp>

#if FW_ASSERT_LEVEL == FW_NO_ASSERT
    #define FW_ASSERT(...)
#else // ASSERT is defined


#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    #define FILE_NAME_ARG U32
    #define FW_ASSERT(cond, ...) \
        ((void) ((cond) ? (0) : \
        (Fw::SwAssert(ASSERT_FILE_ID, __LINE__, ##__VA_ARGS__))))
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT
    #define FILE_NAME_ARG const CHAR*
    #define FW_ASSERT(cond, ...) \
        ((void) ((cond) ? (0) : \
        (Fw::SwAssert(ASSERT_RELATIVE_PATH, __LINE__, ##__VA_ARGS__))))
#else
    #define FILE_NAME_ARG const CHAR*
    #define FW_ASSERT(cond, ...) \
        ((void) ((cond) ? (0) : \
        (Fw::SwAssert(__FILE__, __LINE__, ##__VA_ARGS__))))
#endif

// F' Assertion functions can technically return even though the intention is for the assertion to terminate the program.
// This breaks static analysis depending on assertions, since the analyzer has to assume the assertion will return.
// When supported, annotate assertion functions as noreturn when statically analyzing.
#ifndef CLANG_ANALYZER_NORETURN
#ifndef __has_feature
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#if __has_feature(attribute_analyzer_noreturn)
#define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#else
#define CLANG_ANALYZER_NORETURN
#endif
#endif

namespace Fw {
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) CLANG_ANALYZER_NORETURN; //!< Assert with no arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1) CLANG_ANALYZER_NORETURN; //!< Assert with one argument
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1, FwAssertArgType arg2) CLANG_ANALYZER_NORETURN; //!< Assert with two arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3) CLANG_ANALYZER_NORETURN; //!< Assert with three arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwAssertArgType arg4) CLANG_ANALYZER_NORETURN; //!< Assert with four arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwAssertArgType arg4, FwAssertArgType arg5) CLANG_ANALYZER_NORETURN; //!< Assert with five arguments
    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwAssertArgType arg4, FwAssertArgType arg5, FwAssertArgType arg6) CLANG_ANALYZER_NORETURN; //!< Assert with six arguments
}

// Base class for declaring an assert hook
// Each of the base class functions can be overridden
// or used by derived classes.

namespace Fw {
    // Base class for declaring an assert hook
    class AssertHook {
        public:
            AssertHook() : previousHook(nullptr) {}; //!< constructor
            virtual ~AssertHook() {}; //!< destructor
            // override this function to intercept asserts
            virtual void reportAssert(
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
            // default reportAssert() will call this when the message is built
            // override it to do another kind of print. printf by default
            virtual void printAssert(const CHAR* msg);
            // do assert action. By default, calls assert.
            // Called after reportAssert()
            virtual void doAssert();
            // register the hook
            void registerHook();
            // deregister the hook
            void deregisterHook();

        protected:
        private:
            // the previous assert hook
            AssertHook *previousHook;
    };
}
#endif // if ASSERT is defined


#endif
