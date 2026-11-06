typedef unsigned int U32;

namespace Svc {
U32 counter = 0;                                 // Violation of cpp/fprime/global-mutable-variable
static U32 cache[4];                             // Violation of cpp/fprime/global-mutable-variable
const U32 LIMIT = 4;                             // Compliant: const
constexpr U32 OTHER = 5;                         // Compliant: constexpr
static const char* NAMES[] = {"a", "b"};         // Violation: array of mutable pointers
static const char* const CNAMES[] = {"a", "b"};  // Compliant: deeply const
U32 next() {
    static U32 calls = 0;       // Violation of cpp/fprime/global-mutable-variable
    static const U32 step = 1;  // Compliant: const static local
    U32 local = calls + step;   // Compliant: local
    calls = local;
    return local;
}
}  // namespace Svc
