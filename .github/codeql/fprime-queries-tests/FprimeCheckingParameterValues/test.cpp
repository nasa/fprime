typedef unsigned int U32;

namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

// Violation of cpp/fprime/checking-parameter-values (JPL Rule 15):
// a pointer parameter dereferenced without a prior check.
U32 readUnchecked(const U32* ptr) {
    return *ptr;
}

// Violation: an integer parameter used as an array index without a check.
U32 pickUnchecked(const U32* data, U32 index) {
    FW_ASSERT(data != nullptr);
    return data[index];
}

// Compliant: both parameters are checked with FW_ASSERT before use.
U32 pickChecked(const U32* data, U32 index) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(index < 4);
    return data[index];
}
