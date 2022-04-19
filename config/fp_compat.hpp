#ifndef FP_COMPAT_HPP
#define FP_COMPAT_HPP

#define nullptr 0
#define override 
#define DEPRECATED(func, message) func
#define static_assert(x,y) //COMPILE_TIME_ASSERT(x,"COMP_ASSERT");

#define PRIu8 "%02X"
#define PRId8 "%02X"
#define PRIu16 "%04X"
#define PRId16 "%04X"
#define PRIu32 "%08X"
#define PRId32 "%08X"
#define PRIu64 "%016X"
#define PRId64 "%016X"

#endif // FP_COMPAT_HPP